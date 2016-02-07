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
#include "DcRelayBoard.h"

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
DcRelayBoard::
init( MsMother& main,
      const MsMother::BoardPins& pins )
{
   // Init changes depending on if it's connected to a shift register
   // or directly to the arduino pins.
   if ( pins.shiftReg )
   {
      relay.initShift( &main.shiftOutBuf, pins.out[0] );
      led[0].initShift( &main.shiftOutBuf, pins.out[1] );
      led[1].initShift( &main.shiftOutBuf, pins.out[2] );
      sensor[0].initShift( &main.shiftInBuf, pins.in[0], HIGH );
      sensor[1].initShift( &main.shiftInBuf, pins.in[1], HIGH );
      sensor[2].initShift( &main.shiftInBuf, pins.in[2], HIGH );
   }
   else
   {
      relay.init( pins.out[0] );
      led[0].init( pins.out[1] );
      led[1].init( pins.out[2] );
      sensor[0].init( pins.in[0], HIGH );
      sensor[1].init( pins.in[1], HIGH );
      sensor[2].init( pins.in[2], HIGH );
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
//                  0 for sensor[0] or 1 for sensor[1].  May be null
//                  to ignore the sensors.
//
void
DcRelayBoard::
poll( long currentMillis,
      DigitalInput::StateChangeCb sensorCb )
{
   sensor[0].poll( currentMillis, sensorCb, 0 );
   sensor[1].poll( currentMillis, sensorCb, 1 );
   sensor[2].poll( currentMillis, sensorCb, 2 );
   relay.poll( currentMillis );
   led[0].poll( currentMillis );
   led[1].poll( currentMillis );
}

//============================================================================
