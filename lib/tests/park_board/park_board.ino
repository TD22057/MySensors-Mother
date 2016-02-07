//
// MySensors main mother board v1.0 test sketch
//
// dc board wired to card 0 (shift registers)
// switch on sensor 0, led on output 1, dc relay on output 0
//
// 
#include "MsMother.h"
#include "SPI.h"
#include "DigitalPin.h"
#include "LoopFreq.h"
#include "Timer.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "Adafruit_NeoPixel.h"
#include "DigitalPin.h"
#include "MedianFilter.h"
#include "Sonar.h"

static void buttonCb( DigitalInput::Status status, int8_t identifier );
static void sonarCb( uint16_t dist_cm );

#define PR( x ) Serial.print( x )
#define PRLN( x ) Serial.println( x )

const uint8_t PIXEL_BRIGHTNESS = 32; // 0->255
const int NUM_PIXELS = 24; // 24 led's in ring
const long MAX_TIME_USEC = 500 * 58; // 5 m

class ParkingBoard
{
public:
   void init( uint16_t distRate_hz );
   uint16_t poll( long currentMillis,
                  DigitalInput::StateChangeCb buttonCb=0,
                  int16_t buttonIdent=0,
                  SonarChangeCb sonarCb=0 );

   // 0=off, 255=on
   void setBrightness( uint8_t b );
   void setPixel( uint8_t index, uint8_t red, uint8_t green, uint8_t blue );
   void clearPixels();

   // Echo on pin 3, trigger on pin 5, 7 samples for median.
   Sonar< 3, 5, 7 > sonar;
   DigitalInput button;
private:
   bool m_needToShow;
   Adafruit_NeoPixel m_pixels;
};

inline
void
ParkingBoard::
setBrightness( uint8_t b )
{
   m_pixels.setBrightness( b );
   m_needToShow = true;
}

inline
void
ParkingBoard::
setPixel( uint8_t index,
          uint8_t red,
          uint8_t green,
          uint8_t blue )
{
   m_pixels.setPixelColor( index, red, green, blue );
   m_needToShow = true;
}

inline
void
ParkingBoard::
clearPixels()
{
   m_pixels.clear();
   m_needToShow = true;
}

void
ParkingBoard::
init( uint16_t distRate_hz )
{
   button.init( MsMother::child[2].in[0] );
   sonar.init( distRate_hz );
   
   m_pixels.setPin( 6 );
   m_pixels.updateLength( NUM_PIXELS );
   m_pixels.updateType( NEO_GRB + NEO_KHZ800 );
   m_pixels.clear();
   m_pixels.begin();
   m_needToShow = true;
}

uint16_t
ParkingBoard::
poll( long currentMillis,
      DigitalInput::StateChangeCb buttonCb,
      int16_t buttonIdent,
      SonarChangeCb sonarCb )
{
   //PR( "poll() at " ); PRLN( currentMillis );
   
   button.poll( currentMillis, buttonCb, buttonIdent );
   uint16_t dist_cm = sonar.poll( sonarCb );

   if ( m_needToShow )
   {
      PRLN( "showing..." );
      m_pixels.show();
      m_needToShow = false;
      PRLN( "done" );
   }
   return dist_cm;
}

MsMother g_main;
ParkingBoard g_park;
const int DIST_RATE_HZ = 10;

void
setup()
{
   Serial.begin( 19200 );
   g_main.init();
   g_park.init( DIST_RATE_HZ );
   Serial.println( "Begin..." );
}

void
loop()
{
   long t = millis();
   g_main.poll( t, buttonCb, 0 );
   g_park.poll( t, buttonCb, 1, sonarCb );
}

static void
sonarCb( uint16_t dist_cm )
{
   static bool m_active = false;
   
   PR( "Distance (cm) = " ); PRLN( dist_cm );

   const uint16_t minDist = 2;
   const uint16_t maxDist = 60;
   const uint16_t noDist = 200;
   
   if ( dist_cm >= noDist )
   {
      if ( m_active )
      {
         g_park.clearPixels();
         m_active = false;
      }
      return;
   }

   if ( ! m_active )
   {
      for ( uint8_t i = 0; i < NUM_PIXELS; i++ )
      {
         g_park.setPixel( i, 0, 2, 0 );
      }
      m_active = true;
   }

   int32_t n = NUM_PIXELS;
   if ( dist_cm < maxDist )
   {
      int32_t d = dist_cm - minDist;
      n = ( NUM_PIXELS * d ) / ( maxDist - minDist );
   }
   if ( n > NUM_PIXELS )
   {
      PRLN( "============================" );
      PRLN( "============================" );
      PRLN( "============================" );
      n = NUM_PIXELS;
   }
   PR( "Lighting # pixels: " ); PRLN( n );
   for ( uint8_t i = 0; i < NUM_PIXELS; i++ )
   {
      if ( i < n )
      {
         g_park.setPixel( i, 0, 48, 0 );
      }
      else
      {
         g_park.setPixel( i, 0, 2, 0 );
      }
   }
}

static void
buttonCb( DigitalInput::Status status,
          int8_t identifier )
{
   static long last = 0;
   const long DBL_CLICK = 500;
   
   Serial.print( "Main button: " );
   switch( status )
   {
   case DigitalInput::CLOSED:
      Serial.println( "CLOSED" );
      break;
   case DigitalInput::OPENED:
      {
         Serial.print( "OPENED" );
         g_main.piezo.off();
         
         long t = millis();
         long dt = t - last;
         Serial.print( " after (ms) " );
         Serial.println( dt );
         if ( dt < DBL_CLICK )
         {
            g_main.led.off();
         }
         else
         {
            g_main.led.blinkSlow();
         }
         last = t;
      }
      break;
   case DigitalInput::OPENED_LONG:
      Serial.println( "OPENED LONG" );
      g_main.piezo.blinkFast( 5 );
      break;
   case DigitalInput::NONE:
      break;
   }
}
