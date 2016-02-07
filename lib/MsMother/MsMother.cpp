// Copyright 2016 by Ted Drain
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
#include "MsMother.h"

//#define MSMOTHER_DBG 

// SPI settings to use - just use the default.
static SPISettings s_spiSettings;

//============================================================================
// Global board pin instantation.
MsMother::BoardPins MsMother::child[3] = {
   // child[0] 
   {
      true,    // shift register bit indeces
      3, 2, 1, // out1..3 maps to output shift ports 4, 3, 2 (bit is port-1)
      3, 2, 1, // in1..3 maps to input shift ports 4, 3, 2 (bit is port-1)
   },
   // child[1] 
   {
      true,    // shift register bit indeces
      5, 6, 7, // out1..3 maps to output shift ports 6, 7, 8 (bit is port-1)
      5, 6, 7, // in1..3 maps to input shift ports 6, 7, 8 (bit is port-1)
   },
   // child[2] is linked to actual Arduino pins.  
   {
      false,       // pin values
       6,  5,  3, // D6 (PWM), D5 (PWM), D3 (IRQ)
      17, 18, 19, // A3, A4/SDA, A5/SCL
   },
};

//============================================================================
// Initialize the board.
//
// This will initialize the main board pins including the SPI bus (and
// call SPI.begin()).
//
void
MsMother::
init()
{
   shiftInBuf = 0;
   shiftOutBuf = 0;

   // SPI pin init
   pinMode( 10, OUTPUT ); // set arduino as SPI master
   pinMode( 11, OUTPUT ); // MOSI
   pinMode( 12, INPUT );  // MISO
   pinMode( 13, OUTPUT ); // SCK

   // main board components.
   piezo.init( 10, HIGH );
   led.init( 9, HIGH );
   button.init( 20, LOW, DigitalInput::ANALOG ); // A6, external pull up

   m_shiftOutEnable.mode( OUTPUT );
   m_shiftCS.mode( OUTPUT );
   m_shiftInLoad.mode( OUTPUT );
   m_memoryCS.mode( OUTPUT );
   m_radioCS.mode( OUTPUT );

   // Enable shift register outputs.  Need to do this here because at
   // boot up, the output shift register sets its outputs to all 1's
   // or garbage so we get weird behavior.  The enable pin has a pull
   // up resistor so it turns off the register during boot up.  Set it
   // low here to enable the output shift register now.
   m_shiftOutEnable.low();

   // Deselect all the chip select pins.
   m_shiftCS.high(); // disable 
   m_memoryCS.high(); // disable 
   m_radioCS.high(); // disable 
   m_shiftInLoad.low();

   piezo.off();
   led.off();
   
   SPI.begin();
}

//============================================================================
// Poll the main board.
//
// This should be called in each loop().  Will update shift registers
// and process the main board button and piezo.
//
//= INPUTS
//- currentMillis   The current elapsed time in milliseconds.
//- buttonCb        Optional callback function.  Will be called if the
//                  status changes.
//- identifier      Optional, arbitrary identifer to pass to the callback.
//                  Useful for allowing one callback to handle multiple
//                  switches by changing the identifier.
//
void
MsMother::
poll( long currentMillis,
      DigitalInput::StateChangeCb buttonCb,
      int8_t buttonId )
{
   // Read and write from/to the shift registers.  
   SPI.beginTransaction( s_spiSettings );
   
   // Activate the input shift register load/rck pins and set the chip
   // select to low to turn on both registers.
   m_shiftInLoad.high();
   m_shiftCS.low();

   // Shift the bits in and out.
   shiftInBuf = SPI.transfer( shiftOutBuf );

   // Deactivate the chips.
   m_shiftCS.high();
   m_shiftInLoad.low();

   SPI.endTransaction();

#ifdef MSMOTHER_DBG
   printShift();
#endif
   
   // Poll the other main board components.
   piezo.poll( currentMillis );
   led.poll( currentMillis );
   button.poll( currentMillis, buttonCb, buttonId );
}

//============================================================================
#ifdef MSMOTHER_DBG
static void
printShift()
{
   static int count = 0;
   static uint8_t lastIn = 255;
   static uint8_t lastOut = 255;
   
   if ( shiftInBuf != lastIn )
   {
      Serial.print( ++count );
      Serial.print( " Read : " );
      for ( uint8_t i = 0; i < 8; i++ )
      {
         Serial.print( bitRead( shiftInBuf, i ) );
         Serial.print( " " );
      }
      Serial.print( "\n" );
      lastIn = shiftInBuf;
   }
   if ( shiftOutBuf != lastOut )
   {
      Serial.print( ++count );
      Serial.print( " Write: " );
      for ( uint8_t i = 0; i < 8; i++ )
      {
         Serial.print( bitRead( shiftOutBuf, i ) );
         Serial.print( " " );
      }
      Serial.print( "\n" );
      lastOut = shiftOutBuf;
   }
}
#endif // MSMOTHER_DBG
