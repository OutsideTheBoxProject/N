char gBTAdr[13];
char gBtCmd[256];

void BtReceive(void) {
  bool keepReading = true;
  byte incomingbyte;
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

void BtSendW(char* x){
  BtSend(x);
}

void BtSendO(char* x){
  BtSend(x, false);
}

void setupBt(void) {
   
  BtSend("$$$", false);
  BtSend("SF,1"); // factory defaults
  BtSend("SN,Headband"); // set bt name
//  BtSend("SC,0024"); //Service Class
//  BtSend("SD,0704"); //Device Class
//  BtSend("ST,253"); //Configuration Timer
//  BtSend("SO,%"); //Connect/Disconnect Status String
//  BtSend("T,1"); //Pass Received Data (from UART or Bluetooth) while in Command Mode.
//  BtSend("SM,0"); //Mode (0 = slave)
//  BtSend("U,115200,N"); // set baudrate temporary
  BtSend("SA,1"); //Enable authentication 
  BtSend("---"); 
  
    
}


boolean BtAvailable(void) {
  BtSend("$$$", false);
  BtSend("GK"); // receives connection status
      
  int numVal = 0;  
  if(strlen(gBtMsg) > 0) 
     numVal= atoi(gBtMsg);
   
   BtSend("---");              
      
   if(numVal == 1)
     return true;
   return false;
}  

