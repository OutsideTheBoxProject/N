//*******************************************************
//              www.linksprite.com
// Note:
// 1. SD must be formated to FAT16
// 2. As the buffer of softserial has 64 bytes,
//    so the code read 32 bytes each time
// 3. Please add the libaray to the lib path
//
// * SD card attached to SPI bus as follows:
// * MOSI - pin 11
// * MISO - pin 12
// * CLK - pin 13
// * CS - pin 4
//*******************************************************
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
 
SoftwareSerial mySerial(5,6);          // Set Arduino pin 4 and 5 as softserial
 
byte ZERO = 0x00;
byte incomingbyte;
long int j=0,k=0,count=0,i=0x0000;
uint8_t MH,ML;
boolean EndFlag=0;
File  myFile;
int pic = 0;
 
void SendResetCmd()
{
    mySerial.write(0x56);
    mySerial.write(ZERO);
    mySerial.write(0x26);
    mySerial.write(ZERO);
}
 
/*************************************/
/* Set ImageSize :
/* <1> 0x22 : 160*120
/* <2> 0x11 : 320*240
/* <3> 0x00 : 640*480
/* <4> 0x1D : 800*600
/* <5> 0x1C : 1024*768
/* <6> 0x1B : 1280*960
/* <7> 0x21 : 1600*1200
/************************************/
void SetImageSizeCmd(byte Size)
{
    mySerial.write(0x56);
    mySerial.write(ZERO);  
    mySerial.write(0x54);
    mySerial.write(0x01);
    mySerial.write(Size);
}
 
/*************************************/
/* Set BaudRate :
/* <1>¡¡0xAE  :   9600
/* <2>¡¡0x2A  :   38400
/* <3>¡¡0x1C  :   57600
/* <4>¡¡0x0D  :   115200
/* <5>¡¡0xAE  :   128000
/* <6>¡¡0x56  :   256000
/*************************************/
void SetBaudRateCmd(byte baudrate)
{
    mySerial.write(0x56);
    mySerial.write(ZERO);
    mySerial.write(0x24);
    mySerial.write(0x03);
    mySerial.write(0x01);
    mySerial.write(baudrate);
}
 
void SendTakePhotoCmd()
{
    mySerial.write(0x56);
    mySerial.write(ZERO);
    mySerial.write(0x36);
    mySerial.write(0x01);
    mySerial.write(ZERO); 
}
 
void SendReadDataCmd()
{
    MH=i/0x100;
    ML=i%0x100;
    mySerial.write(0x56);
    mySerial.write(ZERO);
    mySerial.write(0x32);
    mySerial.write(0x0c);
    mySerial.write(ZERO);
    mySerial.write(0x0a);
    mySerial.write(ZERO);
    mySerial.write(ZERO);
    mySerial.write(MH);
    mySerial.write(ML);
    mySerial.write(ZERO);
    mySerial.write(ZERO);
    mySerial.write(ZERO);
    mySerial.write(0x20);
    mySerial.write(ZERO);
    mySerial.write(0x0a);
    i+=0x20;
}
 
void StopTakePhotoCmd()
{
    mySerial.write(0x56);
    mySerial.write(ZERO);
    mySerial.write(0x36);
    mySerial.write(0x01);
    mySerial.write(0x03);
}
 
void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for Leonardo only
    }
 
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    
    Serial.print("Initializing SD card...");
    // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
    // Note that even if it's not used as the CS pin, the hardware SS pin
    // (10 on most Arduino boards, 53 on the Mega) must be left as an output
    // or the SD library functions will not work.
    pinMode(10, OUTPUT);
    if (!SD.begin(10))
    {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");
    Serial.println("please waiting ....");
 
    mySerial.begin(115200);
    delay(100);
    SendResetCmd();
    delay(2000);
    SetBaudRateCmd(0x0D);
    delay(500);
    SetImageSizeCmd(0x1D);
    delay(500);                            //Wait 2-3 second to send take picture command
}
 
void loop()
{
    byte a[32];
    int ii;
    pic++;
    String filename = String(String(pic) + ".jpg");
    char __filename[sizeof(filename)];
    filename.toCharArray(__filename, sizeof(__filename));
 
    Serial.println("Starting Loop");
    SendResetCmd();
    delay(500);
    SendTakePhotoCmd();
    delay(500);
    
    Serial.println("Taken picture");

    while(mySerial.available()>0)
    {
        incomingbyte=mySerial.read();
    }
 
    myFile = SD.open(__filename, FILE_WRITE); //The file name should not be too long
 
   Serial.println("Getting image and writing file");
   unsigned long start = micros();

    while(!EndFlag)
    {
        j=0;
        k=0;
        count=0;
        SendReadDataCmd();
        delay(2);
        while(mySerial.available()>0)
        {
          incomingbyte=mySerial.read();
          k++;
          delayMicroseconds(100); 
          if((k>5)&&(j<32)&&(!EndFlag))
          {
            a[j]=incomingbyte;
            if((a[j-1]==0xFF)&&(a[j]==0xD9))     //tell if the picture is finished
              EndFlag=1;
            j++;
           count++; 
          }
         }

        for(ii=0; ii<count; ii++)
          myFile.write(a[ii]);
    }
 
    myFile.close();
    unsigned long end = micros();
    unsigned long delta = end-start;
    Serial.println("Finished writing data to file in " + String(delta));
    delay(2000);
    EndFlag=0;
    i=0x0000;

}
