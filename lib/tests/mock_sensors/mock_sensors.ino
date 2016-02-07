/*
* MockMySensors
*
* This skecth is intended to crate fake sensors which register and respond to the controller
* 
* Barduino 2015
*/

#include <MySigningNone.h>
#include <MyTransportRFM69.h>
//#include <MyTransportNRF24.h>
#include <MyHwATMega328.h>
//#include <MySigningAtsha204Soft.h>
//#include <MySigningAtsha204.h>
#include "MsMother.h"
#include "DigitalPin.h"
#include "LoopFreq.h"
#include "Timer.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"

#include <SPI.h>
#include <MySensor.h>  
#include <MyMessage.h>

static void mainButtonCb( DigitalInput::Status status, int8_t identifier );
static void switchCb( DigitalInput::Status status, int8_t identifier );

#define PR( x ) Serial.print( x )
#define PRLN( x ) Serial.println( x )

#define RADIO_ERROR_LED_PIN 4  // Error led pin
#define RADIO_RX_LED_PIN    6  // Receive led pin
#define RADIO_TX_LED_PIN    5  // the PCB, on board LED

// Wait times
#define LONG_WAIT 500  
#define SHORT_WAIT 50
#define SLEEP_TIME 1000 // Sleep time between reads (in milliseconds)

#define SKETCH_NAME "FakeMySensors "
#define SKETCH_VERSION "v0.2"

// Global Vars
boolean metric = true;
long randNumber;

// Instanciate MySersors Gateway
//MyTransportNRF24 transport(RF24_CE_PIN, RF24_CS_PIN, RF24_PA_LEVEL_GW);
//MyTransportRFM69 transport;

#define RF69_SPI_CS 15 // A1 main board radio CS

MyTransportRFM69 transport(RFM69_FREQUENCY, RFM69_NETWORKID, RF69_SPI_CS, RF69_IRQ_PIN, false,
                           RF69_IRQ_NUM);

// Message signing driver (signer needed if MY_SIGNING_FEATURE is turned on in MyConfig.h)
//MySigningNone signer;
//MySigningAtsha204Soft signer;
//MySigningAtsha204 signer;

// Hardware profile 
MyHwATMega328 hw;

// Construct MySensors library (signer needed if MY_SIGNING_FEATURE is turned on in MyConfig.h)
// To use LEDs blinking, uncomment WITH_LEDS_BLINKING in MyConfig.h
//#ifdef WITH_LEDS_BLINKING
//MySensor gw(transport, hw /*, signer*/, RADIO_RX_LED_PIN, RADIO_TX_LED_PIN, RADIO_ERROR_LED_PIN);
//#else
MySensor gw(transport, hw /*, signer*/);
//#endif

//Instanciate Messages objects
MyMessage msg_S_DOOR(0,V_TRIPPED);
MyMessage msg_S_LIGHT(0,V_LIGHT);
MyMessage msg_S_TEMP(0,V_TEMP);

MsMother g_main;
DigitalInput g_sw[2];
DigitalOutput g_led[2];

void setup()  
{ 
   g_main.init();
   g_sw[0].initShift( &g_main.shiftInBuf, MsMother::child[0].in[0], HIGH );
   g_sw[1].initShift( &g_main.shiftInBuf, MsMother::child[1].in[0], HIGH );
   g_led[0].initShift( &g_main.shiftOutBuf, MsMother::child[0].out[0] );
   g_led[1].initShift( &g_main.shiftOutBuf, MsMother::child[1].out[0] );
   Serial.println( "Begin..." );
   
   // Start the gateway
   gw.begin(incomingMessage,254);
   gw.wait(500);
   Serial.println("GW Started");
  
   // Send the Sketch Version Information to the Gateway
   Serial.print("Send Sketch Info: ");
   gw.sendSketchInfo(SKETCH_NAME, SKETCH_VERSION);
   Serial.print(SKETCH_NAME);
   Serial.println(SKETCH_VERSION);
   gw.wait(500);
  
   // Register all sensors to gw (they will be created as child devices)
   Serial.println("Presenting Nodes");
   Serial.println("________________");
  
   Serial.println("  S_DOOR");
   gw.present(0,S_DOOR);
   gw.wait(50);
  
   Serial.println("  S_LIGHT");
   gw.present(0,S_LIGHT);
   gw.wait(50);

   Serial.println("  S_TEMP");
   gw.present(0,S_TEMP);
   gw.wait(50);

   Serial.println("________________");
}

void loop()      
{ 
   gw.process();

   long t = millis();
   g_main.poll( t, mainButtonCb );
   //g_sensor.poll( t, sensorCb );

   g_sw[0].poll( t, switchCb, 0 );
   g_sw[1].poll( t, switchCb, 1 );
   g_led[0].poll( t );
   g_led[1].poll( t );
}

//gw.send(msg_S_DOOR.set(1));
//gw.send(msg_S_DOOR.set(0));

    /*
      bool isLightOn = gw.loadState(S_LIGHT);
      if (isLightOn) {
      Serial.println("On");
      gw.send(msg_S_LIGHT.set(1));
      } else {
      Serial.println("Off");
      gw.send(msg_S_LIGHT.set(0));
      }*/
  
static void
switchCb( DigitalInput::Status status,
          int8_t identifier )
{
   long randNumber=random(0,101);

   Serial.print( "DigitalInput: " );
   Serial.print( identifier );
   switch( status )
   {
   case DigitalInput::CLOSED:
      Serial.println( " CLOSED" );
      break;
   case DigitalInput::OPENED:
      Serial.println( " OPENED" );
      gw.send(msg_S_TEMP.set(map(randNumber,1,100,0,45)));
      break;
   case DigitalInput::OPENED_LONG:
      Serial.println( " OPENED LONG" );
      break;
   case DigitalInput::NONE:
      break;
   }
}


static void
mainButtonCb( DigitalInput::Status status,
              int8_t identifier )
{
   Serial.print( "Main button: " );
   switch( status )
   {
   case DigitalInput::CLOSED:
      Serial.println( "CLOSED" );
      break;
   case DigitalInput::OPENED:
      gw.send(msg_S_DOOR.set(1));
      break;
   case DigitalInput::OPENED_LONG:
      break;
   case DigitalInput::NONE:
      break;
   }
}

// This is called when a new time value was received
void receiveTime(unsigned long controllerTime) {

  Serial.print("Time value received: ");
  Serial.println(controllerTime);
}

void incomingMessage(const MyMessage &message) {
  switch (message.type) {
    case V_LIGHT:
          gw.saveState(S_LIGHT, message.getBool());
          Serial.print("Incoming change for S_LIGHT:");
          Serial.print(message.sensor);
          Serial.print(", New status: ");
          Serial.println(message.getBool());
    break;
  }
}
