#pragma once
#include "MsMother.h"

//============================================================================
// Sensor daughter board class.
//
// Initialize with the main board instance and main.child[i] instance
// for the slot the daughter board is connected to.  Call poll() in
// the loop routine.
//
// Use the public variables to access connectors on the board.
//
class SensorBoard
{
public:
   // 0 for digital sensor control.  Otherwise it's the analog value to use
   // to for tripping the sensor.  This is only valid when the board is
   // connected to directly to the Arduino pins via MsMother::child[3].
   void init( MsMother& main, const MsMother::BoardPins& pins );

   // sensor ID's will be 0, 1, 2 for sensors[i]
   void poll( long currentMillis, DigitalInput::StateChangeCb sensorCb );

   DigitalInput sensor[3];
   DigitalOutput led[3];

private:
};

//============================================================================

