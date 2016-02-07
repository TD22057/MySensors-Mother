//
// MySensors main mother board v1.0 test sketch
//
// Sensor board wired to card 2 (arduino pins)
//
// 
#include "MsMother.h"
#include "SensorBoard.h"
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
SensorBoard g_sensor;
uint8_t g_lastIn = 0;
uint8_t g_lastOut = 0;
uint32_t g_inputNum = 0;


void
setup()
{
   Serial.begin( 19200 );
   g_main.init();
   g_sensor.init( g_main, MsMother::child[2] );

   Serial.println( "Begin..." );
}

void
loop()
{
   long t = millis();
   g_main.poll( t, mainButtonCb );
   g_sensor.poll( t, sensorCb );
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
   Serial.print( "Sensor " );
   Serial.print( identifier );
   Serial.print( " changed: " );
   switch( status )
   {
   case DigitalInput::CLOSED:
      Serial.println( "ACTIVE" );
      g_sensor.led[identifier].blinkFast();
      break;
   case DigitalInput::OPENED:
   case DigitalInput::OPENED_LONG:
      Serial.println( "CLEAR" );
      g_sensor.led[identifier].off();
      break;
   case DigitalInput::NONE:
      break;
   }
}
