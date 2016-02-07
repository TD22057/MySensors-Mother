//
// MySensors main mother board v1.0 test sketch
//
// dc board wired to card 0 (shift registers)
// switch on sensor 0, led on output 1, dc relay on output 0
//
// 
#include "MsMother.h"
#include "DcRelayBoard.h"
#include "SPI.h"
#include "DigitalPin.h"
#include "LoopFreq.h"
#include "Timer.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"

static void mainButtonCb( DigitalInput::Status status, int8_t identifier );
static void sensorCb( DigitalInput::Status status, int8_t identifier );

#define PR( x ) Serial.print( x )
#define PRLN( x ) Serial.println( x )

MsMother g_main;
DcRelayBoard g_relay;
uint8_t g_lastIn = 0;
uint8_t g_lastOut = 0;
uint32_t g_inputNum = 0;


void
setup()
{
   Serial.begin( 19200 );
   g_main.init();
   g_relay.init( g_main, MsMother::child[0] );

   Serial.println( "Begin..." );
}

void
loop()
{
   long t = millis();
   g_main.poll( t, mainButtonCb );
   g_relay.poll( t, sensorCb );
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
      Serial.println( "CLOSED" );
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
            g_main.led.blinkSlow();
         }
         last = t;
      }
      break;
   case DigitalInput::OPENED_LONG:
      Serial.println( "OPENED LONG" );
      g_main.piezo.blinkFast( 5 );
      break;
   case DigitalInput::NONE:
      break;
   }
}

static void
sensorCb( DigitalInput::Status status,
          int8_t identifier )
{
   switch( status )
   {
   case DigitalInput::CLOSED:
      break;
   case DigitalInput::OPENED:
      Serial.print( "Sensor " );
      Serial.print( identifier );
      if ( identifier == 0 )
      {
         Serial.print( " relay change to: " );
         Serial.println( ! g_relay.relay.isOn() );
         g_relay.relay.toggle();
      }
      else
      {
         Serial.print( " LED change to: " );
         Serial.println( ! g_relay.led[identifier-1].isOn() );
         g_relay.led[identifier-1].toggle();
      }
      break;
   case DigitalInput::OPENED_LONG:
      break;
   case DigitalInput::NONE:
      break;
   }
}
