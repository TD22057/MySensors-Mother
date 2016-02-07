//
// MySensors main mother board v1.0 test sketch
//
// input board wired to card 1 (shift registers)
// 12V switches wired to 3 inputs
// 12V connectsion wired to 3 outputs (test w/ multimeter)
// 
#include "MsMother.h"
#include "InputBoard.h"
#include "SPI.h"
#include "DigitalPin.h"
#include "LoopFreq.h"
#include "Timer.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"

static void inputCb( DigitalInput::Status status, int8_t identifier );

#define PR( x ) Serial.print( x )
#define PRLN( x ) Serial.println( x )

MsMother g_main;
InputBoard g_input;

void
setup()
{
   Serial.begin( 19200 );
   g_main.init();
   g_input.init( g_main, MsMother::child[1] );

   PRLN( "Begin..." );
   PR( "Input[0] status: " ); PRLN( g_input.input[0].isOn() );
   PR( "Input[1] status: " ); PRLN( g_input.input[1].isOn() );
   PR( "Input[2] status: " ); PRLN( g_input.input[2].isOn() );
}

void
loop()
{
   long t = millis();
   g_main.poll( t );
   g_input.poll( t, inputCb );
}

static void
inputCb( DigitalInput::Status status,
         int8_t identifier )
{
   switch( status )
   {
   case DigitalInput::CLOSED:
      PR( "Input[" ); PR( identifier ); PRLN( "] active" );
      PR( "DigitalInputing output[" ); PR( identifier ); PRLN( "] on" );
      g_input.output[identifier].on();
      break;
   case DigitalInput::OPENED:
   case DigitalInput::OPENED_LONG:
      PR( "Input[" ); PR( identifier ); PRLN( "] inactive" );
      PR( "DigitalInputing output[" ); PR( identifier ); PRLN( "] off" );
      g_input.output[identifier].off();
      break;
   case DigitalInput::NONE:
      break;
   }
}

