#include "SensorBoard.h"

//============================================================================
// Initialize the board
//
// This will use the input pins object to initialize itself to which
// ever connector the relay board is attached to.
//
//= INPUTS
//- main   The main board instance.
//- pins   main.child instance the board is connected to.
//
void
SensorBoard::
init( MsMother& main,
      const MsMother::BoardPins& pins )
{
   // Init changes depending on if it's connected to a shift register
   // or directly to the arduino pins.
   if ( pins.shiftReg )
   {
      for ( int8_t i = 0; i < 3; i++ )
      {
         led[i].initShift( &main.shiftOutBuf, pins.out[i], HIGH );
         sensor[i].initShift( &main.shiftInBuf, pins.in[i], HIGH );
      }
   }
   else
   {
      for ( int8_t i = 0; i < 3; i++ )
      {
         led[i].init( pins.out[i] );
         sensor[i].init( pins.in[i], HIGH );
      }
   }
}

//============================================================================
// Poll the board
//
// This will poll each of the items on the board for processing.
//
//= INPUTS
//- currentMillis   The current elapsed time in milliseconds.
//- sensorCb        Sensor callback function.  Will be called if the
//                  sensor status changes.  The identifier will be either
//                  i=0,1,2 for sensor[i].  May be null to ignore the sensors.
//
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

//============================================================================
