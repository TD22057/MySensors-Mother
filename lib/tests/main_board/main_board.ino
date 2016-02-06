//
// MySensors main mother board v1.0 test sketch
//
// 
#include "MsMother.h"
#include "SPI.h"
#include "DigitalPin.h"
#include "LoopFreq.h"
#include "Timer.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"

static void mainButtonCb( DigitalInput::Status status, int8_t identifier );

MsMother g_main;
uint8_t g_lastIn = 0;
uint8_t g_lastOut = 0;
uint32_t g_inputNum = 0;

void
setup()
{
   Serial.begin( 19200 );
   g_main.init();

   Serial.println( "Begin..." );
}

void
loop()
{
   long t = millis();
   g_main.poll( t, mainButtonCb );

   if ( 0 ) {
   if ( g_main.shiftInBuf != g_lastIn )
   {
      g_lastIn = g_main.shiftInBuf;
      Serial.print( ++g_inputNum );
      Serial.print( " Read: " );
      for ( uint8_t i = 0; i < 8; i++ )
      {
         Serial.print( bitRead( g_lastIn, i ) );
         Serial.print( " " );
      }
      Serial.print( "\n" );
   }
   }
}


static void
mainButtonCb( DigitalInput::Status status,
              int8_t identifier )
{
   static long last = 0;
   const long DBL_CLICK = 500;
   
   Serial.print( "Main button: " );
   switch( status )
   {
   case DigitalInput::CLOSED:
      Serial.println( "PRESSED" );
      break;
   case DigitalInput::OPENED:
      {
         Serial.print( "OPENED" );
         g_main.piezo.off();
         
         long t = millis();
         long dt = t - last;
         Serial.print( " after (ms) " );
         Serial.println( dt );
         if ( dt < DBL_CLICK )
         {
            g_main.led.off();
         }
         else
         {
            g_main.led.blinkSlow( 5 );
         }
         last = t;
      }
      break;
   case DigitalInput::OPENED_LONG:
      Serial.println( "OPENED LONG" );
      g_main.piezo.blinkSlow( 5 );
      break;
   case DigitalInput::NONE:
      break;
   }
}
