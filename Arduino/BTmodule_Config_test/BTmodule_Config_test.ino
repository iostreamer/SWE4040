int incomingByte = 0;   // for incoming serial data
void setup() {
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
  Serial1.begin(9600);
  

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
