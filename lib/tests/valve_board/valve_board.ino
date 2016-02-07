//
// MySensors main mother board v1.0 test sketch
//
// dc board wired to card 0 (shift registers)
// switch on sensor 0, led on output 1, dc relay on output 0
//
// 
#include "MsMother.h"
#include "ValveBoard.h"
#include "SPI.h"
#include "DigitalPin.h"
#include "LoopFreq.h"
#include "Timer.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "Valve.h"

static void mainButtonCb( DigitalInput::Status status, int8_t identifier );
static void sensorCb( DigitalInput::Status status, int8_t identifier );
static void valveCb( Valve::Status status, int8_t identifier );

#define PR( x ) Serial.print( x )
#define PRLN( x ) Serial.println( x )


MsMother g_main;
ValveBoard g_valve;

void
setup()
{
   Serial.begin( 19200 );
   g_main.init();
   g_valve.init( g_main, MsMother::child[1] );

   PRLN( "Begin..." );
   PR( "Valve status: " ); PRLN( g_valve.valve.status() );
}

void
loop()
{
   long t = millis();
   g_main.poll( t, mainButtonCb );
   g_valve.poll( t, sensorCb, 0, valveCb, 0 );
}

static void
mainButtonCb( DigitalInput::Status status,
              int8_t identifier )
{
   static long last = 0;
   const long DBL_CLICK = 500;
   
   PR( "Main button: " );
   switch( status )
   {
   case DigitalInput::CLOSED:
      PRLN( "CLOSED" );
      break;
   case DigitalInput::OPENED:
      {
         PR( "OPENED" );
         g_main.piezo.off();
         
         long t = millis();
         long dt = t - last;
         PR( " after (ms) " );
         PRLN( dt );
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
      PRLN( "OPENED LONG" );
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
   case DigitalInput::OPENED_LONG:
      PR( "Current status: " ); PRLN( g_valve.valve.status() );
      PRLN( "Toggling..." );
      g_valve.valve.toggle();
      g_valve.led.toggle();
      break;
   case DigitalInput::NONE:
      break;
   }
}

static void
valveCb( Valve::Status status,
         int8_t identifier )
{
   PR( "Valve changed: " );
   switch( status )
   {
   case Valve::OPENED:
      Serial.println( "opened!" );
      break;
   case Valve::OPENING:
      Serial.println( "opening..." );
      break;
   case Valve::CLOSED:
      Serial.println( "closed!" );
      break;
   case Valve::CLOSING:
      Serial.println( "closing..." );
      break;
   case Valve::UNKNOWN:
      Serial.println( "unknown?" );
      break;
   case Valve::STALLED:
      Serial.println( "stalled?" );
      break;
   case Valve::NONE:
      Serial.println( "none?" );
      break;
   }
}
