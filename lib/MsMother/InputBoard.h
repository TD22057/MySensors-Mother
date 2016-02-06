#pragma once
#include "MsMother.h"

//============================================================================
// Protected input daughter board class.
//
// Initialize with the main board instance and main.child[i] instance
// for the slot the daughter board is connected to.  Call poll() in
// the loop routine.
//
// Use the public variables to access connectors on the board.
//
class InputBoard
{
public:
   void init( MsMother& main, const MsMother::BoardPins& pins );

   // Callback identifiers will be 0, 1, 2 for input[i]
   void poll( long currentMillis, DigitalInput::StateChangeCb inputCb );

   DigitalInput input[3];
   DigitalOutput output[3]; 
};

//============================================================================

