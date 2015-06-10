/*
 OutsideTheBox (http://outsidethebox.at)
 @author Christopher Frauenberger
 @created 27 May 2015

 Wiring
 D5  Camera TX    // Camera SoftwareSerial
 D6  Camera RX
 D10 SD CS        // SD Card SPI 
 D11 SD DI
 D12 SD DO
 D13 SD CLK
 A4  data, SDA -> HeadbandPulse arduino A4 / RTC SDA 
 A5  clk, SCL -> HeadbandPulse arduino A5 / RTC SCL
 
 Bluetooth SilverMate on UART

*******************************************************
*/

#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <DS1307RTC.h>
#include <Time.h>

// i2c device id of pulse arduino (slave)
#define pulseID 2
 
// soft serial for camera (TXD = 5, RX = 6)
SoftwareSerial camSerial(5,6);          // Set Arduino pin 4 and 5 as softserial 

File  picFile;      // file for pictures
char filename[12];  // filename
int picindex = 0;
int sdCSPin = 10;

int trigger = 1; // take a picture

// global time variables to be set by RTC
tmElements_t time;

File logFile;

// Pulse variables
int bpm = 0;
int signal = 0;
int qs = 0;
 
void setup()
{
  Serial.begin(115200);
 
   // initialise SD and wipe if true
  Serial.println("Initializing SD card...");
  if (SDinit()) Serial.println("done!");
  else Serial.println("failed...");
  
  // test the RTC
  setupRTC();
  
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
      CamTakePicture();
      picFile.close(); 
      logFile = SD.open("log.txt", FILE_WRITE);
      printTimestamp(logFile, time);
      logFile.print(";"); // delimiter
      logFile.println(filename); 
      logFile.close();
    }
  }
  else {
    // log pulse data 
    uint8_t buf[6];
    bpm = 0x0;
    signal = 0x0;
    qs = 0x0;
    int i = 0;
    
    Wire.requestFrom(pulseID, 6); // request 6 bytes from pulse arduino

    while(Wire.available())    // slave may send less than requested
      buf[i++] = Wire.read(); // receive a byte as character
      
    if (i == 6) {
          
      qs = buf[0] | (buf[1]<<8);
      bpm =  buf[2] | (buf[3]<<8);
      signal =  buf[4] | (buf[5]<<8);

      Serial.println("\nLogging Pulse, qs:" + String(qs) + " BPM:" + String(bpm));
    
      if (qs) {
        Serial.println("actually logging, qs is true");
        logFile = SD.open("pulse.txt", FILE_WRITE);
        printTimestamp(logFile, time);
        logFile.print(";"); // delimiter
        logFile.print(String(signal));
        logFile.print(";"); // delimiter
        logFile.println(String(bpm));    
        logFile.close();
      }
    }
    else 
      Serial.println("Pulse arduino not available, no BPM received");
    
  }
  
  trigger = !trigger;
}
