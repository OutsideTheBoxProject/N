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
SoftwareSerial camSerial(5,6);          // Set Arduino pin 4 and 5 as softserial 

File  picFile;      // file for pictures
int picindex = 0;   // picture index for filename
char filename[10];  // filename
int sdCSPin = 8;

 
void setup()
{
  Serial.begin(115200);
 
   // initialise SD and wipe if true
  Serial.print("Initializing SD card...");
  if (SDinit(false)) Serial.println("done!");
  else Serial.println("failed...");
  
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
  do {
    //create a filename to store photo at
    sprintf(filename, "%i.jpg", picindex++);
  }
  while(SD.exists(filename)); //loop if it exists already
  
  picFile = SD.open(filename, FILE_WRITE);
  if (picFile)
    CamTakePicture();
  else 
    delay(100);

}

