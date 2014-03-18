int incomingByte = 0;   // for incoming serial data
void setup() {
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
  Serial1.begin(9600);
  
  Serial.println("Configuring BT Module...");
  
  Serial1.print("AT&F\r");
  delay(2000);
  Serial1.print("AT+UARTCONFIG,9600,N,1,0\r");
  delay(2000);
  Serial1.print("ATZ\r");
  
  Serial.println("Module Configured");
  

}

void loop() {
    //Send
    if (Serial.available() > 0) {
      

       
      
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial1.print((char)incomingByte);
    Serial.print((char)incomingByte);
    

  }

  // Receive 
  if (Serial1.available() > 0) {
    
    // read the incoming byte:
    incomingByte = Serial1.read();

    // say what you got:
    //Serial.write(incomingByte);
    Serial.print((char)incomingByte);
    

  }
  
  
  
}

