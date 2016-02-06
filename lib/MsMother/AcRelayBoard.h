#pragma once
#include "MsMother.h"

//============================================================================
// AC relay daughter board class.
//
// Initialize with the main board instance and main.child[i] instance
// for the slot the daughter board is connected to.  Call poll() in
// the loop routine.
//
// Use the public variables to access connectors on the board.
//
class AcRelayBoard
{
public:
   void init( MsMother& main, const MsMother::BoardPins& pins );

   // sensor ID's will be 0, 1, 2 for sensors[i]
   void poll( long currentMillis, DigitalInput::StateChangeCb sensorCb );

   DigitalInput sensor[2];
   DigitalOutput led; 
   DigitalOutput relay; 
};

//============================================================================

