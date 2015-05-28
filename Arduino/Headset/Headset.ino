//*******************************************************
// OutsideTheBox (http://outsidethebox.at)
// @author Christopher Frauenberger
// @created 27 May 2015
//
//*******************************************************

#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include "JPEGCamera.h"

// soft serial for camera (TXD = 5, RX = 6)
SoftwareSerial camSerial(5,6);   
JPEGCamera cam(camSerial);

boolean paired = 0;   // true if paired with base station
boolean trigger = 0;  // true if shooting picture
boolean endPic = 0;   // true if no more data from camera is available

File picFile;
int picIndex = 0;    // picture index for filename

long int block = 0x0000;  // data pointer for reading from camera
byte packageSize = 0x40; // chunks of data read from the camera

unsigned int s1, s2;
char filename[10];

void setup() {
  // debug output for now, BT later
  Serial.begin(115200);
  
  // Setup SD card
  // pin 10 is chipselect
  Serial.print("Preparing SD card... ");
  pinMode(10, OUTPUT);
  if (!SD.begin(10))
  {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println(" done.");
  
  // initialise camera
  camSerial.begin(38400);
  delay(50);
  //reset cam and wait.
  cam.reset();
  delay(2000);
  cam.chPictureSize(3);
  delay(50);
//  // not changing the baudrate for now
//  cam.chBaudRate(2);
//  delay(50);
//  //change serial to match new camera baud rate
//  camSerial.end();
//  // match the new baudrate
//  camSerial.begin(38400);
//  delay(50);  
  
  Serial.println("Initialised camera.");
  
  trigger = 1;
}


void loop() {
  
  // *** Paired with base station
  if (paired) {
  
  } 
  // *** Headset mode
  else {
    
    // shoot a picture
    if (trigger) {
      do {
        //create a filename to store photo at
        sprintf(filename, "%i.jpg", picIndex++);
      }
      while(SD.exists(filename)); //loop if it exists already

      s1 = millis();
      Serial.println("Taking picture");
      //must call stopPictures before a new photo is taken.
      cam.stopPictures();
      delay(50);
      cam.takePicture();
      delay(50);

      picFile = SD.open(filename, FILE_WRITE);
      //if it opens ok
      if(picFile)
      {
        //save to file
        Serial.println("Info:Writing to SD,");
        cam.readData(picFile);
      }
      else //else it didn't open ok
        Serial.println("Error:Error opening file on SD,");
    
      //must close file to finish writing.
      picFile.close();
      s2 = millis();
      Serial.println("Done in " + String(s2-s1));
      delay(1000);
      
    } // trigger
    
  } // not paired
  
} // loop
