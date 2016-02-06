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
#pragma once
#include "MsMother.h"
#include "Valve.h"

//============================================================================
// Valve daughter board class.
//
// Initialize with the main board instance and main.child[i] instance
// for the slot the daughter board is connected to.  Call poll() in
// the loop routine.
//
// Use the public variables to access connectors on the board.
//
class ValveBoard
{
public:
   void init( MsMother& main, const MsMother::BoardPins& pins );

   // sensor ID's will be 0, 1, 2 for sensors[i]
   void poll( long currentMillis, DigitalInput::StateChangeCb sensorCb,
              int8_t sensorId, Valve::StateChangeCb valveCb, int8_t valveId );

   Valve valve;
   DigitalInput sensor;
   DigitalOutput led;

private:
};

//============================================================================

