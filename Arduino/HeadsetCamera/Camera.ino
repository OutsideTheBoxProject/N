byte ZERO = 0x00;
uint8_t MH,ML;
long int i=0x0000;

void CamTakePicture() {
  boolean EndFlag=0;
  byte incomingbyte;
  long int j=0,k=0,count=0;

  byte a[32];
  int ii;       
  unsigned int s1, s2;

  s1 = millis();   
  SendTakePhotoCmd();
  delay(500);
  while(camSerial.available()>0)
  {
     incomingbyte=camSerial.read();
  }
  while(!EndFlag)
  {
    j=0;
    k=0;
    count=0;
    SendReadDataCmd();
    delay(5);
    while(camSerial.available()>0)
    {
      incomingbyte=camSerial.read();
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
        picFile.write(a[ii]); 
  }
  i=0x0000;
  EndFlag=0;
  s2 = millis();
  Serial.println("Done in " + String(s2-s1));
}


void SendResetCmd()
{
  camSerial.write(0x56);
  camSerial.write(ZERO);
  camSerial.write(0x26);
  camSerial.write(ZERO);
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
  camSerial.write(0x56);
  camSerial.write(ZERO);
  camSerial.write(0x54);
  camSerial.write(0x01);
  camSerial.write(Size);
}
 
/*************************************/
/* Set BaudRate :
/* <1>　0xAE  :   9600
/* <2>　0x2A  :   38400
/* <3>　0x1C  :   57600
/* <4>　0x0D  :   115200
/* <5>　0xAE  :   128000
/* <6>　0x56  :   256000
/*************************************/
void SetBaudRateCmd(byte baudrate)
{
  camSerial.write(0x56);
  camSerial.write(ZERO);
  camSerial.write(0x24);
  camSerial.write(0x03);
  camSerial.write(0x01);
  camSerial.write(baudrate);
}
 
void SendTakePhotoCmd()
{
camSerial.write(0x56);
camSerial.write(ZERO);
camSerial.write(0x36);
camSerial.write(0x01);
camSerial.write(ZERO);
}
 
void SendReadDataCmd()
{
MH=i/0x100;
ML=i%0x100;
camSerial.write(0x56);
camSerial.write(ZERO);
camSerial.write(0x32);
camSerial.write(0x0c);
camSerial.write(ZERO);
camSerial.write(0x0a);
camSerial.write(ZERO);
camSerial.write(ZERO);
camSerial.write(MH);
camSerial.write(ML);
camSerial.write(ZERO);
camSerial.write(ZERO);
camSerial.write(ZERO);
camSerial.write(0x20);
camSerial.write(ZERO);
camSerial.write(0x0a);
i+=0x20;
}
 
void StopTakePhotoCmd()
{
camSerial.write(0x56);
camSerial.write(ZERO);
camSerial.write(0x36);
camSerial.write(0x01);
camSerial.write(0x03);
}

