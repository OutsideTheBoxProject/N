/*
 OutsideTheBox (http://outsidethebox.at)
 @author Christopher Frauenberger
 @created 27 May 2015

 Wiring
 D4  Detect Mode (Low = BaseStation / High = PictureMode)
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

// if false, no debug messages are written to Serial
#define DEBUG 0

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
int modePin = 4;


// global time variables to be set by RTC
tmElements_t time;


unsigned long triggerMillis = 0;

File logFile;
 
void setup()
{
  Serial.begin(115200);
 
   // initialise SD and wipe if true
  if (DEBUG) Serial.println("Initializing SD card...");
  if (SDinit()) 
    if (DEBUG) Serial.println("done!");
  else 
    if (DEBUG) Serial.println("failed...");
  
  // test the RTC
  setupRTC();
  
  // setup BT
//  Serial.print("$$$");
//  Serial.println("SI,0000");  // disable discovery mode to save energy
//  Serial.println("SW,8320");  // enable deep slepp with 0.5sec sniff 
//  Serial.println("---");
   
  // init Camera
  camSerial.begin(115200);
  delay(200);
  SendResetCmd();//Wait 2-3 second to send take picture command
  delay(3000);
  SetImageSizeCmd(0x1D);
  delay(100);
  SetBaudRateCmd(0x1C);
  delay(200);  
  camSerial.begin(57600);
  
  pinMode(modePin, INPUT);
  
}
 
void loop()
{
  unsigned long currentMillis = millis();

  if (digitalRead(modePin) == HIGH) { // if this is 0, its picture mode - if it is high, its BT mode
    String fn;
    byte a[64];
    int i=0;
    
    //wake up BT
//    Serial.print("$");
//    delay(100);
    
    if (Serial.available() > 0) {
      while(Serial.available()>0) Serial.read(); // discard everything incoming, we are connected
      Serial.print("$HSStart-end");  
      delay(100);
      File root = SD.open("/");
      root.rewindDirectory();
      File entry = root.openNextFile();
      while (entry) {
        if (!entry.isDirectory()) {
          fn = entry.name();
          if ((fn.indexOf(".JPG") >= 0) || fn == "PULSE.TXT" || fn == "LOG.TXT") {
            Serial.print("$HSFile:");
            Serial.print(fn);
            Serial.print("|");
            Serial.print(entry.size());
            Serial.print("-end");
            delay(100);
            i = 0;
            while (entry.available() > 0) {
              if (i > 63) {
                i = 0;
                Serial.write(a, 64);
                delayMicroseconds(100); 
              }
              else {
                a[i++] = entry.read();
                delayMicroseconds(100); 
              }
            }
            Serial.write(a, i);
          }
          else {
            // no file of ours, jus let it go...
          }
            
        }
        else {
          // direcotry, dont follow
        }
        entry.close();
        entry = root.openNextFile();
        fn.toCharArray(filename,12);
        SD.remove(filename);
      }
      root.close();
      Serial.print("$HSEnd-end");
      delay(100);
    }
    delay(500);
  }
  else {
    
    // get the time
    RTC.read(time);
  
    if ((currentMillis - triggerMillis) > 15000) {
      triggerMillis = currentMillis;
      // take a picture
      do {
        //create a filename to store photo at
        sprintf(filename, "%i.jpg", picindex++);
      }
      while(SD.exists(filename)); //loop if it exists already    
   
      picFile = SD.open(filename, FILE_WRITE);  
      if (picFile) {
        if (DEBUG) Serial.println("Taking picture " + String(picFile.name()));
          CamTakePicture();
        picFile.close(); 
        logFile = SD.open("log.txt", FILE_WRITE);
        printTimestamp(logFile, time);
        logFile.print(","); // delimiter
        logFile.println(filename); 
        logFile.close();
      }
    }
    else {
      // log pulse data 
      uint8_t buf[6];
      int bpm = 0x0;
      int signal = 0x0;
      int qs = 0x0;
      int i = 0;
    
      Wire.requestFrom(pulseID, 6); // request 6 bytes from pulse arduino

      while(Wire.available())    // slave may send less than requested
        buf[i++] = Wire.read(); // receive a byte as character
      
      if (i == 6) {    
        qs = buf[0] | (buf[1]<<8);
        bpm =  buf[2] | (buf[3]<<8);
        signal =  buf[4] | (buf[5]<<8);

        if (DEBUG) Serial.println("Logging Pulse, qs:" + String(qs) + " BPM:" + String(bpm));
    
        if (qs) {
          logFile = SD.open("pulse.txt", FILE_WRITE);
          printTimestamp(logFile, time);
          logFile.print(","); // delimiter
          logFile.println(String(bpm));    
          logFile.close();
        }
      }
      else 
        if (DEBUG) Serial.println("Pulse arduino not available, no BPM received");        
    }
  }
  delay(100);
}
