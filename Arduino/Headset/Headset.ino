//*******************************************************
// OutsideTheBox (http://outsidethebox.at)
// @author Christopher Frauenberger
// @created 27 May 2015
//
//*******************************************************

#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

// soft serial for camera (TXD = 5, RX = 6)
SoftwareSerial camSerial(5,6);   

boolean paired = 0;   // true if paired with base station
boolean trigger = 0;  // true if shooting picture
boolean endPic = 0;   // true if no more data from camera is available

File picFile;
int picindex = 0;    // picture index for filename
char filename[10];

byte incomingbyte;

long int block = 0x0000;  // data pointer for reading from camera
byte packageSize = 0x40; // chunks of data read from the camera

unsigned int s1, s2;


void setup() {
  // Start BT
  if (false)
    BtInit(); // this is only to configure, just starting the Serial should be anough...
  else 
    Serial.begin(115200);

  // maximum speed for SPI pins (11,12,13)
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  
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
  delay(200);
  SendResetCmd();//Wait 2-3 second to send take picture command
  delay(2000);
  SetBaudRateCmd(0x2A);
  delay(100);
  SetImageSizeCmd(0x1C);
  delay(100);

  Serial.println("Initialised camera.");
  
  trigger = 1;
}


void loop() {
  // Check if paired
  
  
  // *** Paired with base station
  if (paired) {
  
  } 
  // *** Headset mode
  else {
    
    // shoot a picture
    if (trigger) {
      byte a[2];
      long int j, k, count;
      do {
        //create a filename to store photo at
        sprintf(filename, "%i.jpg", picindex++);
      }
      while(SD.exists(filename)); //loop if it exists already

      s1 = millis();
      Serial.println("Taking picture");
  
      SendTakePhotoCmd();
      delay(200);
      while(camSerial.available()>0)
        incomingbyte=camSerial.read();

      Serial.println("Getting data, writing file");
      picFile = SD.open(filename, FILE_WRITE); 
      while(!endPic)  {
        k=0;
        count=0;
        SendReadDataCmd();
        delay(5);
        // read data from camera
        while(camSerial.available()>0)  {
          incomingbyte=camSerial.read();
          k++;
          if((k>5)&&(count<packageSize)&&(!endPic)) {
            a[0]=a[1];
            a[1]=incomingbyte;
            if((a[0]==0xFF)&&(a[1]==0xD9))     //tell if the picture is finished
              endPic=1;
            picFile.write(a[1]);
            count++; 
          }
        }
//        Serial.println("read / wrote " + String(count));
      }
      picFile.close();
      endPic=0;
      block=0x0000;
    } 

    s2 = millis();
    Serial.println("Done in " + String(s2-s1));
    delay(1000);
  }
}
