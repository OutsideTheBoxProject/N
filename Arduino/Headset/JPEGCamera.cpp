/* Arduino JPEGCamera Library
 * Copyright 2010 SparkFun Electronic
 * Written by Ryan Owens
 * Modified by Emmanuel ALLAUD (2013)
 * Modified by Chris Frauenberger (2015)
*/

#include "JPEGCamera.h"

//Commands for the LinkSprite Serial JPEG Camera
const char GET_SIZE[5] = {0x56, 0x00, 0x34, 0x01, 0x00};
const char RESET_CAMERA[4] = {0x56, 0x00, 0x26, 0x00};
const char TAKE_PICTURE[5] = {0x56, 0x00, 0x36, 0x01, 0x00};
const char STOP_TAKING_PICS[5] = {0x56, 0x00, 0x36, 0x01, 0x03};
char POWER_SAVING[] = {0x56,0x00,0x3E,0x03,0x00,0x01,0x00};
char CHANGE_BAUD_RATE[] = {0x56, 0x00, 0x24, 0x03, 0x01, 0x00 };
char CHANGE_PICT_SIZE[] = {0x56, 0x00, 0x54, 0x01, 0x00};
char READ_DATA[8] = {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A, 0x00, 0x00};

// Utility function
void eat_response(Stream& c)
{
  while (!c.available());
  while (c.available()) c.read();
}

JPEGCamera::JPEGCamera(Stream& comm) : cameraPort(comm)
{
}

void JPEGCamera::powerSaving(bool on) // on = True -> enter power saving mode
{
  int i;

  POWER_SAVING[6] = on?0x01:0x00;

  for (i = 0;i<7;i++)
    cameraPort.write(POWER_SAVING[i]);
  delay(50);

  for (int i=0;i<5;i++)
    Serial.write(cameraPort.read());
}

//Get the size of the image currently stored in the camera
//return: size of the jpeg file

unsigned int JPEGCamera::getSize()
{
  char response[5];
  //Send the GET_SIZE command string to the camera
  sendCommand(GET_SIZE, response, 5);
  
  while(!cameraPort.available());
  //Read 4 characters from the camera and add them to the response string
  for(int i=0; i<4; i++)
    {
      response[i]=cameraPort.read();
    }
  //The size is in the last 2 characters of the response.
  //Parse them and convert the characters to an integer
  return ((byte)response[2])*256 + (byte)response[3];	
}

void JPEGCamera::chBaudRate(byte bd_rate)
{
  char response[7];

  switch(bd_rate) {
  case 0: // 9600
    CHANGE_BAUD_RATE[5] = 0xAE;
    break;
  case 1: // 38400
    CHANGE_BAUD_RATE[5] = 0x2A;
    break;
  case 2: // 57600
    CHANGE_BAUD_RATE[5] = 0x1C;
    break;
  case 3: // 115200
    CHANGE_BAUD_RATE[5] = 0x0D;
    break;
  case 4: // 128000
    CHANGE_BAUD_RATE[5] = 0xAE;
    break;
  case 5: // 256000
    CHANGE_BAUD_RATE[5] = 0x56;
    break;
  default: //Everything else -> 38400
    CHANGE_PICT_SIZE[5] = 0x2A;    
  }

  while (cameraPort.available()) cameraPort.read();

  //Send each character in the command string to the camera through the camera serial port 
  char * command = CHANGE_BAUD_RATE;
  for(int i=0; i<7; i++){
    cameraPort.write(*command++);
  }
  while (cameraPort.available()>0)  cameraPort.read();
}

void JPEGCamera::chPictureSize(byte size)
{
  char response[5];

  switch(size) {
  case 0: // 160x120
    CHANGE_PICT_SIZE[4] = 0x22;
    break;
  case 1: // 320x240
    CHANGE_PICT_SIZE[4] = 0x11;
    break;
  case 2: // 640x480
    CHANGE_PICT_SIZE[4] = 0x00;
    break;
  case 3: // 800*600
    CHANGE_PICT_SIZE[4] = 0x1D;
    break;
  case 4: // 1024x768
    CHANGE_PICT_SIZE[4] = 0x1C;
    break;
  case 5: // 1280x960
    CHANGE_PICT_SIZE[4] = 0x1B;
    break;
  case 6: // 1600x1200
    CHANGE_PICT_SIZE[4] = 0x2A;
    break;
  default: //Everything else -> 320x240
    CHANGE_PICT_SIZE[4] = 0x11;
  }

  sendCommand(CHANGE_PICT_SIZE, response, 5);
}

//Reset the camera
//Usage: camera.reset();

void JPEGCamera::reset()
{
  const char * command = RESET_CAMERA;

  for(int i=0;i<4;i++)
    cameraPort.write(command[i]);
  delay(25);
  if (cameraPort.available())
    eat_response(cameraPort);
}

//Take a new picture
//pre: response is an empty string

//Usage: camera.takePicture();
void JPEGCamera::takePicture()
{
  sendCommand(TAKE_PICTURE, NULL, 5);
}

//Stop taking pictures
//Usage: camera.stopPictures();

void JPEGCamera::stopPictures()
{
  sendCommand(STOP_TAKING_PICS, NULL, 5);
}

//Get the picture from the camera
//     output is the stream (can be Serial, SD or other) to write the picture to
//Usage: camera.readData(comm);
int JPEGCamera::readData(Stream& output)
{
  unsigned long count=0, address = 0;
  byte buf[32]; // Buffer to hold the current chunk of the picture
  byte flag = 0;
  int i,j;

  //Flush out any data currently in the serial buffer
  while (cameraPort.available()) cameraPort.read();
  while (flag!=3) {
    
    //Send the command to get JPEGCAM_READ_SIZE bytes of data from the current address
    for(i=0; i<8; i++)
      cameraPort.write(READ_DATA[i]);

    cameraPort.write(highByte(address));
    cameraPort.write(lowByte(address));
    cameraPort.write((byte)0x00);
    cameraPort.write((byte)0x00);
    cameraPort.write((byte)0x00);
    cameraPort.write((byte)0x20); // 32 bytes to read
    cameraPort.write((byte)0x00);
    cameraPort.write((byte)0x0A);	
    address += 32;

    delay(5);

    //Discard the response header.
    while(!cameraPort.available());
    for(i=0; i<5; i++)
      cameraPort.read();

    //Now read the actual data
    i=0;
    for (i=0;(i<32) && (flag!=3);i++) {
      count ++;
      while (!cameraPort.available());
      buf[i]=cameraPort.read();
      if (buf[i] == 0xFF) flag |= 1;
      else if ((buf[i] == 0xD9) && (flag == 1)) flag |= 2;
      else flag = 0;
    }
    // Discard the footer
    while (cameraPort.available()>0) cameraPort.read();

    for (j=0;j<i;j++) output.write(buf[j]);
  }

  //Return the number of bytes
  return count;
}

//Send a command to the camera
//pre: cameraPort is a serial connection to the camera set to 3800 baud
//     command is a string with the command to be sent to the camera
//     response is an empty string
//	   length is the number of characters in the command
//post: response contains the camera response to the command
// if response is NULL, the response is just discarded
//return: the number of characters in the response string
//usage: None (private function)
int JPEGCamera::sendCommand(const char * command, char * response, int length)
{
  int i;

  //Clear any data currently in the serial buffer
  while (cameraPort.available()) cameraPort.read();
  //Send each character in the command string to the camera through the camera serial port 
 for(i=0; i<length; i++){
    cameraPort.write(*command++);
  }
  //Get the response from the camera and add it to the response string.
  while(!cameraPort.available());
  for(i=0; i<length; i++)
    {
      byte next_byte = cameraPort.read();
      if (response!=NULL)
	*response++ = next_byte;
    }
  //return the number of characters in the response string
  return length;
}
