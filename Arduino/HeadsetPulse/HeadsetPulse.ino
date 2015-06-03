/*
  OutsideTheBox 
  June 2015
  
  Based on
  Pulse Sensor Amped 1.4    by Joel Murphy and Yury Gitman   http://www.pulsesensor.com

 ----------------------       ----------------------  ----------------------
*/

//  Variables
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0

// output pins
int bpmPin = 5; // PWM [0..255] -> [0..255]
int signalPin = 6; // PWM [0..255] -> [0..1024]
int qsPin = 7; // digital: high if heartbeat was found

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded! 
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat". 
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.


void setup(){
  Serial.begin(115200);             // we agree to talk fast!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
  
  pinMode(bpmPin, OUTPUT);
  pinMode(signalPin, OUTPUT);
  pinMode(qsPin, OUTPUT);

}


//  Where the Magic Happens
void loop(){
  
  if (QS == true) {     //  A Heartbeat Was Found
                        // BPM and IBI have been Determined
    analogWrite(signalPin, Signal / 2);                       
    analogWrite(bpmPin, BPM);
    digitalWrite(qsPin, HIGH);    
    
    Serial.println("BPM: " + String(BPM));                   
                 
    QS = false;                      // reset the Quantified Self flag for next time    
  }
  else {
    analogWrite(signalPin, 0);                       
    analogWrite(bpmPin, 0);
    digitalWrite(qsPin, LOW);    
  }
  
  delay(50);                             //  take a break
}

