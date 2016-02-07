//
// MySensors main mother board v1.0 test sketch
//
// dc board wired to card 0 (shift registers)
// switch on sensor 0, led on output 1, dc relay on output 0
//
// 
//#include "MsMotherV1.h"
//#include "SPI.h"
#include "DigitalPin.h"
#include "NewPing.h"
//#include "LoopFreq.h"
//#include "Timer.h"
//#include "Switch.h"
//#include "DigitalOutput.h"
//#include "Adafruit_NeoPixel.h"

#define PR( x ) Serial.print( x )
#define PRLN( x ) Serial.println( x )

const long MAX_TIME_USEC = 500 * 58; // 5 m

// Echo on pin 3, trigger on pin 5, 7 samples for median.
//Sonar< 3, 5, 7 > sonar;
const uint8_t ECHO = 3;
const uint8_t TRIGGER = 5;

const int DIST_RATE_HZ = 3;

DigitalPin< ECHO > g_echo;
DigitalPin< TRIGGER > g_trigger;

bool g_sent = false;
volatile uint32_t s_pingBeg_us = 0;
volatile uint32_t s_pingEnd_us = 0;

static void s_echoRise()
{
   //PRLN( "Rising..." );
   s_pingBeg_us = micros();
   attachInterrupt( digitalPinToInterrupt( ECHO ), s_echoFall, FALLING );
}   
static void s_echoFall()
{
   //PRLN( "Falling..." );
   s_pingEnd_us = micros();
   detachInterrupt( digitalPinToInterrupt( ECHO ) );
}
long ping()
{
   PRLN( "===" );
   s_pingBeg_us = s_pingEnd_us = 0;


   g_trigger.high();
   delayMicroseconds( 10 );
   g_trigger.low();
   attachInterrupt( digitalPinToInterrupt( ECHO ), s_echoRise, RISING );

   int i = 0;
   while ( i++ < 200 && s_pingEnd_us == 0 )
   {
      delay( 1 );
   }
   long dt = s_pingEnd_us - s_pingBeg_us;
   if ( i == 200 || dt < 0 )
   {
      return 0;
   }

   PR( "Beg: " ); PRLN( s_pingEnd_us );
   PR( "End: " ); PRLN( s_pingBeg_us );
   PR( "DT : " ); PRLN( dt );

   long dist_cm = dt / 58;
   return dist_cm;
}
void
setup()
{
   Serial.begin( 19200 );
   Serial.println( "Begin..." );

   g_echo.mode( INPUT );
   g_trigger.mode( OUTPUT );
   g_trigger.low();
}
void
loop()
{
   static long last = 0;
   long t = millis();
   if ( ( t-last ) > 500 && g_echo.read() == LOW )
   {
      long dist = ping();
      PR( "Ping: " ); PR( dist ); PRLN( " cm" );
      last = t;
   }
}

/*
NewPing ping( TRIGGER, ECHO );
void
setup()
{
   Serial.begin( 19200 );
   Serial.println( "Begin..." );

   g_echo.mode( INPUT );
   g_trigger.mode( OUTPUT );
   g_trigger.low();
}
void
loop()
{
   static long last = 0;
   long t = millis();
   if ( ( t-last ) > 300 )
   {
      long dist = ping.ping_cm();
      PR( "Ping: " ); PR( dist ); PRLN( " cm" );
      last = t;
   }
}
*/

/*
static void s_interrupt();

void
setup()
{
   Serial.begin( 19200 );
   Serial.println( "Begin..." );

   g_echo.mode( INPUT );
   g_trigger.mode( OUTPUT );
   g_trigger.low();

   attachInterrupt( digitalPinToInterrupt( 3 ), s_interrupt, CHANGE);
}

void
s_interrupt()
{
   long t = micros();

   // high = start of distance pulse
   if ( g_start == 0 )
   {
      g_start = t;
   }
   else
   {
      g_stop = t;
      //detachInterrupt( digitalPinToInterrupt( 3 ) );
   }
}

void
loop()
{
   static long last = 0;
   long t = millis();
   if ( ! g_sent && g_echo.read() == LOW && ( t - last ) > 500 )
   {
      g_start = 0;
      g_stop = 0;
      
      g_trigger.high();
      delayMicroseconds( 10 );
      g_trigger.low();

      g_sent = true;
      last = t;
   }
   else if ( g_stop )
   {
      long dt = g_stop - g_start;
      long dist_cm = dt / 58;
      PR( "Beg: " ); PRLN( g_start );
      PR( "End: " ); PRLN( g_stop );
      PR( "DT : " ); PRLN( dt );

      PR( "dist: " ); PRLN( dist_cm );
      PRLN( "===" );
      g_start = 0;
      g_stop = 0;
      g_sent = false;
   }
}

*/

/**************


// ECHO_PIN must have hardware interrupt capability (2 or 3 on
// arduino).  
typedef void (*SonarChangeCb)( uint16_t distance_cm );

template < uint8_t ECHO_PIN, uint8_t TRIGGER_PIN, uint8_t NUM_SAMPLE >
class Sonar
{
public:
   void init( uint16_t rate_hz );

   void setRate( uint16_t rate_hz );

   void sendPing();
   uint16_t poll( long currentMillis, SonarChangeCb callback=0 );

private:
   DigitalPin< ECHO_PIN > m_echo;
   uint8_t m_interrupt;
   DigitalPin< TRIGGER_PIN > m_trigger;
   int32_t m_rate_us;
   bool m_sent;
   uint16_t m_lastDist;
};


template < uint8_t ECHO_PIN, uint8_t TRIGGER_PIN, uint8_t NUM_SAMPLE >
void
Sonar< ECHO_PIN, TRIGGER_PIN, NUM_SAMPLE >::
init( uint16_t rate_hz )
{
   m_echo.mode( INPUT );
   m_interrupt = digitalPinToInterrupt( ECHO_PIN );
   m_trigger.mode( OUTPUT );
   m_trigger.low();
   m_lastDist = 0;
   m_sent = false;
   setRate( rate_hz );
}

template < uint8_t ECHO_PIN, uint8_t TRIGGER_PIN, uint8_t NUM_SAMPLE >
void
Sonar< ECHO_PIN, TRIGGER_PIN, NUM_SAMPLE >::
setRate( uint16_t rate_hz )
{
   m_rate_us = 1000000 / rate_hz; 
}

volatile uint32_t s_pingEnd_us = 0;
volatile uint32_t s_pingBeg_us = 0;

static void s_echoRise();
static void s_echoFall();

static void
s_echoRise()
{
   //PRLN( "Rising..." );
   s_pingBeg_us = micros();
   // interrupt 1 on pin D3
   attachInterrupt( digitalPinToInterrupt( 3 ), s_echoFall, FALLING ); 
}   

static void
s_echoFall()
{
   //PRLN( "Falling..." );
   s_pingEnd_us = micros();
   // interrupt 1 on pin D3
   detachInterrupt( digitalPinToInterrupt( 3 ) );
}

template < uint8_t ECHO_PIN, uint8_t TRIGGER_PIN, uint8_t NUM_SAMPLE >
void
Sonar< ECHO_PIN, TRIGGER_PIN, NUM_SAMPLE >::
sendPing()
{
   PRLN( "Sending..." );
   m_sent = true;

   s_pingBeg_us = 0;
   s_pingEnd_us = 0;
   
   m_trigger.high();
   delayMicroseconds( 10 );
   m_trigger.low();

   // Interrupt when the echo pin goes from low->high.  Interrupt 1 on
   // pin D3
   attachInterrupt( digitalPinToInterrupt( 3 ), s_echoRise, RISING );
}

template < uint8_t ECHO_PIN, uint8_t TRIGGER_PIN, uint8_t NUM_SAMPLE >
uint16_t
Sonar< ECHO_PIN, TRIGGER_PIN, NUM_SAMPLE >::
poll( long currentMillis,
      SonarChangeCb callback )
{
   if ( ! m_sent )
   {
      if ( m_echo.read() == LOW &&
           (int32_t)( micros() - s_pingEnd_us ) > m_rate_us )
      {
         sendPing();
      }
      // NOTE: if we get a time out - it can take a long time (200
      // msec) to "reset" the sensor and have the echo line go low
      // again.  Not sure why that is, but it we try to send a ping
      // which echo is high, it will lock up the arduino somehow.
      return 0;
   }
   else if ( ! s_pingBeg_us )
   {
      return 0;
   }
   PR( "Beg: " ); PRLN( s_pingBeg_us );

   int32_t dt_us = micros() - s_pingBeg_us;
   //if ( dt_us > MAX_TIME_USEC )
   if ( 0 )
   {
      detachInterrupt( digitalPinToInterrupt( 3 ) );
      PR( "TIME OUT: " ); PRLN( dt_us );
      m_sent = false;
      return 0;
   }
   uint16_t meanDist = 0;

   // ping finished
   if ( s_pingEnd_us )
   {
      PR( "Rec: " ); PRLN( s_pingEnd_us );
      
      detachInterrupt( digitalPinToInterrupt( 3 ) );
      m_sent = false;

      dt_us = s_pingEnd_us - s_pingBeg_us;
      PR( "DT : " ); PRLN( dt_us );
      if ( dt_us > MAX_TIME_USEC )
      {
      PRLN( "--" );
         return 0;
      }
      else if ( dt_us > 0 )
      {
         // from data sheet, dist_in_usec / 58 == dist_in_cm
         uint16_t distance = dt_us / 58;
         PR( "Dist cm: " ); PRLN( distance );
      PRLN( "--" );

         uint16_t meanDist = distance;
         if ( callback && meanDist != m_lastDist )
         {
            callback( meanDist );
         }
         
         m_lastDist = meanDist;
      }
   }

   return meanDist;
}

*****/

/************
#include "HC_SR04.h"

//HC_SR04 *HC_SR04::_instance=NULL;
HC_SR04 *HC_SR04::_instance(NULL);

HC_SR04::HC_SR04(int trigger, int echo, int interrupt, int max_dist)
    : _trigger(trigger), _echo(echo), _int(interrupt), _max(max_dist), _finished(false)
{
  if(_instance==0) _instance=this;    
}

void HC_SR04::begin(){
  pinMode(_trigger, OUTPUT);
  digitalWrite(_trigger, LOW);
  pinMode(_echo, INPUT);  
  attachInterrupt(_int, _echo_isr, CHANGE);
}

void HC_SR04::start(){
  _finished=false;
  digitalWrite(_trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigger, LOW);  
}

unsigned int HC_SR04::getRange(bool units){
  return (_end-_start)/((units)?58:148);
}

void HC_SR04::_echo_isr(){
  HC_SR04* _this=HC_SR04::instance();
  
  switch(digitalRead(_this->_echo)){
    case HIGH:
      _this->_start=micros();
      break;
    case LOW:
      _this->_end=micros();
      _this->_finished=true;
      break;
  }   
}
 ***********/
