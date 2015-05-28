char gBtMsg[256];
char gBTAdr[13];
char gBtCmd[256];

int gBtKnownMACTotal = 2;
char* gBtKnownMAC[2]; //This is set to hold only two MAC adresses

void BtReceive(void){
  bool keepReading = true;
  int index = 0;
  
  gBtMsg[0] = '\0';
  
  while(keepReading){
    keepReading = false;
    
    if (Serial.available() > 0) {
      // read the incoming byte:
      incomingbyte = Serial.read();
      if(incomingbyte != 13){
        gBtMsg[index++] = incomingbyte;
        keepReading = true;
      }
    }
  }
  
  gBtMsg[index] = '\0';
}

void BtSend(char* i_pBtCmd, bool i_ln = true){

  if(i_ln){
    Serial.println(i_pBtCmd);
  } else{
    Serial.print(i_pBtCmd);
  }
  
  delay(100);
  BtReceive();
  
}


void BtInit(void){
  bool btConnect = false;
   
  Serial.begin(115200); 
  BtSend("$$$", false);
  BtSend("SF,1"); // factory defaults
  BtSend("SN,Headband"); // set bt name
//  BtSend("SC,0024"); //Service Class
//  BtSend("SD,0704"); //Device Class
//  BtSend("ST,253"); //Configuration Timer
//  BtSend("SO,%"); //Connect/Disconnect Status String
//  BtSend("T,1"); //Pass Received Data (from UART or Bluetooth) while in Command Mode.
//  BtSend("SM,0"); //Mode (0 = slave)
  BtSend("SA,1"); //Enable authentication 
  BtSend("---"); 
  
  while(!btConnect){
    delay(1000);
    BtSend("$$$", false);
    BtSend("GK"); // receives connection status
      
    int numVal = 0;
    
    if(strlen(gBtMsg) > 0) {
        numVal= atoi(gBtMsg);
      }
      
      if(numVal == 1){                  
        btConnect = true;
      }

      BtSend("---");              
  }
}


