//*******************************************************
// OutsideTheBox (http://outsidethebox.at)
// @author Christopher Frauenberger
// @created 27 May 2015
//
//*******************************************************

#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <DS1307RTC.h>
#include <Time.h>
 
// soft serial for camera (TXD = 5, RX = 6)
SoftwareSerial camSerial(5,6);          // Set Arduino pin 4 and 5 as softserial 

File  picFile;      // file for pictures
char filename[12];  // filename
int picindex = 0;
int sdCSPin = 8;

int trigger = 1; // take a picture

// global time variables to be set by RTC
tmElements_t time;

File logFile;

// input pins to read pulse data
int bpmPin = 2; // A2 [0..255] -> [0..255]
int signalPin = 3; // A3 [0..255] -> [0..1024]
int qsPin = 3; // D3: high if heartbeat was found
int bpm = 0;
int signal = 0;
int qs = 0;
 
void setup()
{
  Serial.begin(115200);
 
   // initialise SD and wipe if true
  Serial.print("Initializing SD card...");
  if (SDinit(true)) Serial.println("done!");
  else Serial.println("failed...");
  
  // test the RTC
  testRTC();
  
  // init Camera
  camSerial.begin(115200);
  delay(200);
  SendResetCmd();//Wait 2-3 second to send take picture command
  delay(2000);
  SetBaudRateCmd(0x2A);
  delay(100);
  camSerial.begin(38400);
  delay(100);
  SetImageSizeCmd(0x1D);
  delay(100);
  
  // set up pin to read pulse
  pinMode(qsPin, INPUT);   
  
}
 
void loop()
{
  // get the time
  RTC.read(time);
  
  if (trigger) {
    // take a picture
    do {
      //create a filename to store photo at
      sprintf(filename, "%i.jpg", picindex++);
    }
    while(SD.exists(filename)); //loop if it exists already    
  
    picFile = SD.open(filename, FILE_WRITE);  
    if (picFile) {
      logFile = SD.open("log.txt");
      printTimestamp(logFile, time);
      logFile.print(";"); // delimiter
      logFile.println(filename); 
      logFile.close();
      CamTakePicture();
      picFile.close(); 
    }
  }
  else {
    // log pulse data 
    bpm = analogRead(bpmPin);
    signal = analogRead(signalPin);
    qs = digitalRead(qsPin);
    if (qs) {
      logFile = SD.open("pulse.txt");
      printTimestamp(logFile, time);
      logFile.print(";"); // delimiter
      logFile.print(String(signal));
      logFile.print(";"); // delimiter
      logFile.println(String(bpm));    
      logFile.close();
    }
  }
}
