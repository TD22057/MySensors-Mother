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
