#pragma once

#include "Arduino.h"
#include "SPI.h"
#include "DigitalPin.h"
#include "Timer.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"

//============================================================================
// Main board class
//
// This class controls the components on the main MySensors mother
// board.  Call poll() in the loop() method to process inputs (switch)
// and push data to and from the shift registers.
//
// For a given build, construct the child board classes with one of
// the child[i] variables which will configure them to correctly
// initialize their connections.
//
class MsMother
{
public:
   void init();

   void poll( long currentMillis, DigitalInput::StateChangeCb switchCb=0,
              int8_t identifier=0 );
   
   // Shift register buffers.  Each of the 8 input and output fields
   // is one bit in the buffers.  Buffers are read in and written out
   // each time poll() is called.
   uint8_t shiftInBuf;
   uint8_t shiftOutBuf;

   // Main board components.
   DigitalOutput piezo;
   DigitalOutput led;
   DigitalInput button;

   // External child board definition.  Each board has outputs 1, 2, 3
   // and inputs 1, 2, 3.  These are used to construct child board
   // classes by passing them child[i] (defined below) which tells
   // them which connector that are plugged in at on the main board.
   struct BoardPins {
      // True if the board is using shift registers.  In that case, the
      // fields are bit indeces into the shift register byte buffer.  So
      // if out1 is 2, then the first board output is bit 2 (of 0...7) in
      // the output shift register byte.  If false, then the fields are
      // arduino pin numbers for the actual pins the board is using.
      bool shiftReg;
      uint8_t out[3];
      uint8_t in[3];
   };
   
   static BoardPins child[3];
   /* child 0 and 1 are connected to the input and output shift registers.
      child[0] = {
         true,    // shift register bit indeces
         3, 2, 1, // out1..3 maps to output shift ports 4, 3, 2 (bit is port-1)
         3, 2, 1, // in1..3 maps to input shift ports 4, 3, 2 (bit is port-1)
         }; 
      child[1] = {
         true,    // shift register bit indeces
         5, 6, 7, // out1..3 maps to output shift ports 6, 7, 8 (bit is port-1)
         5, 6, 7, // in1..3 maps to input shift ports 6, 7, 8 (bit is port-1)
         };
      child 2 is linked to actual Arduino pins.
      child[2] = {
         true,       // pin values
         6,  5,  3, // D6 (PWM), D5 (PWM), D3 (IRQ)
         17, 18, 19, // A3, A4/SDA, A5/SCL
         };
   */

private:
   // Arduino Por mini on the main board has these connections:
   //  2, D2 : radio IRQ pin
   //  3, D3 : IRQ pin on child[2].out[2]
   //  4, D4 : output shift enable (low=off, high=on, hardware pull down)
   //  5, D5 : PWM1 pin on child[2].out[1]
   //  6, D6 : PWM2 pin on child[2].out[0]
   //  7, D7 : shift (in and out) register SPI chip select
   //  8, D8 : input shift register load/rck
   //  9, D9 : main LED
   // 10,D10 : main piezo
   // 11,D11 : MOSI
   // 12,D12 : MISO
   // 13,D13 : SCK
   // 14, A0 : flash memory SPI chip select
   // 15, A1 : RFM69 SPI chip select
   // 16, A2 : hardware crypto chip
   // 17, A3 : ANALOG3 pin on child[2].in[0]
   // 18, A4 : SDA pin on child[2].in[1]
   // 19, A5 : SCL pin on child[2].in[2]
   // 20, A6 : main switch (analog only input pin)
   // 21, A7 : unused
   
   // Output shift register enable pin.  Needed because otherwise on
   // boot up the output register may set random data to the outputs.
   // So it's held low by a pulldown resistor.  We can set this high
   // to enable the register after boot up.
   DigitalPin< 4 > m_shiftOutEnable;

   // In and out shift register chip select pin.
   DigitalPin< 7 > m_shiftCS;

   // Input shift register RCK and SLOAD pin.  Using the same pin
   // works fine (might delay input by one clock cycle but that
   // doesn't matter much.
   DigitalPin< 8 > m_shiftInLoad;

   // Flash memory chip select pin.
   DigitalPin< 14 > m_memoryCS;

   // RFM69 radio ship select pin.
   DigitalPin< 15 > m_radioCS;
};
   
//============================================================================
