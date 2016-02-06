#include "ValveBoard.h"

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
ValveBoard::
init( MsMother& main,
      const MsMother::BoardPins& pins )
{
   // Initialize these controls w/ the pins, then pass them to the
   // Valve object which will copy their configurations to use inside
   // the valve.
   DigitalOutput valveOpen, valveClose;
   DigitalInput valveIsOpened, valveIsClosed;
   
   // Init changes depending on if it's connected to a shift register
   // or directly to the arduino pins.
   if ( pins.shiftReg )
   {
      // Connect the open/close outputs.
      valveOpen.initShift( &main.shiftOutBuf, pins.out[0] );
      valveClose.initShift( &main.shiftOutBuf, pins.out[1] );

      // Connect the opened/closed seconds.
      valveIsOpened.initShift( &main.shiftInBuf, pins.in[0], LOW );
      valveIsClosed.initShift( &main.shiftInBuf, pins.in[1], LOW );
      
      led.initShift( &main.shiftOutBuf, pins.out[2] );
      sensor.initShift( &main.shiftInBuf, pins.in[2], HIGH );
   }
   else
   {
      valveOpen.init( pins.out[0] );
      valveClose.init( pins.out[1] );

      valveIsOpened.init( pins.in[0], LOW );
      valveIsClosed.init( pins.in[1], LOW );

      led.init( pins.out[2] );
      sensor.init( pins.in[2], HIGH );
   }

   valve.init( valveOpen, valveClose, valveIsOpened, valveIsClosed,
               10000,   // 10 sec - max power on time
               10000 ); // 10 sec - duty cycle 
}

//============================================================================
// Poll the board
//
// This will poll each of the items on the board for processing.
//
//= INPUTS
//- currentMillis   The current elapsed time in milliseconds.
//- sensorCb        Sensor callback function.  Will be called if the
//                  sensor status changes.
//- sensorId        Identifier to pass to the sensorCb function.
//- valveCb         Valve state change callback function.  Will be called
//                  when anything about the valve changes.
//- valveId         Identifier to pass to the valveCb function.
//
void
ValveBoard::
poll( long currentMillis,
      DigitalInput::StateChangeCb sensorCb,
      int8_t sensorId,
      Valve::StateChangeCb valveCb,
      int8_t valveId )
{
   sensor.poll( currentMillis, sensorCb, sensorId );
   valve.poll( currentMillis, valveCb, valveId );
   led.poll( currentMillis );
}

//============================================================================
