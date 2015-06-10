/*
  OutsideTheBox 
  June 2015
  
  Based on
  Pulse Sensor Amped 1.4    by Joel Murphy and Yury Gitman   http://www.pulsesensor.com

 ----------------------       ----------------------  ----------------------

  Wiring:
  A0  PulseIn
  A4  data, SDA -> HeadbandCamera arduino A4
  A5  clk, SCL, -> HeadbandCamera arduino A5

*/
#include <Wire.h>
// i2c device id of pulse arduino (slave)
#define pulseID 2

//  Variables
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded! 
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat". 
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.


void setup(){
  Serial.begin(115200);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
  
  Wire.begin(pulseID); 
  
  Wire.onRequest(requestEvent);
}

void loop()
{
  delay(300);
  if (QS) Serial.print(" " + String(BPM));
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  Serial.println("\n\nReceived request, sending bpm " + String(BPM));
  uint8_t buf[6];
  buf[0] = (int)QS;
  buf[1] = (int)QS >>  8;
  buf[2] = BPM;
  buf[3] = BPM >> 8;
  buf[4] = Signal;
  buf[5] = Signal >> 8;
  Wire.write(buf, 6); 


}

