//
// MySensors main mother board v1.0 test sketch
//
// Sensor board wired to card 2 (arduino pins)
//
// Shift register testing:
// Wire a switch (pull down) to input 1 on card 0 and 1
// Wire an LED with resistor to output 1 on card 0 and 1
// 
#include "MsMother.h"
#include "SPI.h"
#include "DigitalPin.h"
#include "LoopFreq.h"
#include "Timer.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"

static void mainButtonCb( DigitalInput::Status status, int8_t identifier );
static void sensorCb( DigitalInput::Status status, int8_t identifier );
static void switchCb( DigitalInput::Status status, int8_t identifier );

#define PR( x ) Serial.print( x )
#define PRLN( x ) Serial.println( x )

class SensorBoard
{
public:
   void init( MsMother& mainBoard, const MsMother::BoardPins& board );

   // sensor ID's will be 0, 1, 2 for sensors[i]
   void poll( long currentMillis, DigitalInput::StateChangeCb sensorCb );
   
   DigitalOutput led[3];
   DigitalInput sensor[3];
};

void
SensorBoard::
init( MsMother& mainBoard,
      const MsMother::BoardPins& board )
{
   if ( board.shiftReg )
   {
      for ( int8_t i = 0; i < 3; i++ )
      {
         led[i].initShift( &mainBoard.shiftOutBuf, board.out[i], HIGH );
         sensor[i].initShift( &mainBoard.shiftInBuf, board.in[i], HIGH );
      }
   }
   else
   {
      for ( int8_t i = 0; i < 3; i++ )
      {
         led[i].init( board.out[i] );
         sensor[i].init( board.in[i], HIGH );
      }
   }
}

void
SensorBoard::
poll( long currentMillis,
      DigitalInput::StateChangeCb sensorCb )
{
   for ( int8_t i = 0; i < 3; i++ )
   {
      sensor[i].poll( currentMillis, sensorCb, i );
   }
   for ( int8_t i = 0; i < 3; i++ )
   {
      led[i].poll( currentMillis );
   }
}


MsMother g_main;
//SensorBoard g_sensor;
uint8_t g_lastIn = 0;
uint8_t g_lastOut = 0;
uint32_t g_inputNum = 0;

DigitalInput g_sw[2];
DigitalOutput g_led[2];

void
setup()
{
   Serial.begin( 19200 );
   g_main.init();
   //g_sensor.init( g_main, MsMother::child[2] );

   g_sw[0].initShift( &g_main.shiftInBuf, MsMother::child[0].in[0], HIGH );
   g_sw[1].initShift( &g_main.shiftInBuf, MsMother::child[1].in[0], HIGH );
   g_led[0].initShift( &g_main.shiftOutBuf, MsMother::child[0].out[0] );
   g_led[1].initShift( &g_main.shiftOutBuf, MsMother::child[1].out[0] );

   Serial.println( "Begin..." );
}

void
loop()
{
   long t = millis();
   g_main.poll( t, mainButtonCb );
   //g_sensor.poll( t, sensorCb );

   g_sw[0].poll( t, switchCb, 0 );
   g_sw[1].poll( t, switchCb, 1 );
   g_led[0].poll( t );
   g_led[1].poll( t );
}

static void
switchCb( DigitalInput::Status status,
          int8_t identifier )
{
   Serial.print( "DigitalInput: " );
   Serial.print( identifier );
   switch( status )
   {
   case DigitalInput::CLOSED:
      Serial.println( " CLOSED" );
      break;
   case DigitalInput::OPENED:
      Serial.println( " OPENED" );
      g_led[identifier].blinkSlow();
      break;
   case DigitalInput::OPENED_LONG:
      Serial.println( " OPENED LONG" );
      g_led[identifier].off();
      break;
   case DigitalInput::NONE:
      break;
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
      //g_sensor.led[identifier].blinkFast();
      break;
   case DigitalInput::OPENED:
   case DigitalInput::OPENED_LONG:
      Serial.println( "CLEAR" );
      //g_sensor.led[identifier].off();
      break;
   case DigitalInput::NONE:
      break;
   }
}
