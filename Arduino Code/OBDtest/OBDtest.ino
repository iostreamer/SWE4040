int incomingByte = 0;   // for incoming serial data
void send_and_receive_bullshit();
void waitForOBDConfirmation();
unsigned long interval = 1000;
unsigned long ptime = 0;
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
  delay(2000);
  Serial.println("Connecting to OBD");
  int i;
  
  Serial1.print("ATDAABBCC112233\r");
  waitForOBDConfirmation();
  delay(2000);
}
void waitForOBDConfirmation() {
  
  while (1) {
    // Receive 
    if (Serial1.available() > 0) {
      
      // read the incoming byte:
      incomingByte = Serial1.read();
      Serial.print((char)incomingByte);
      if (incomingByte == 'C') {
        break;
      }
    }
  }
  
  Serial.println("Connection success");
  Serial1.print("AT Z\r");
  delay(2000);
  Serial1.print("AT SP 0\r");
  int i;
  
  for (i = 0; i < 1000; i++) {
     send_and_receive_bullshit(); 
  }
}

int ascii2int(char* string, int nDigit) {
  int i = 0;
  int retval = 0;
  for (; i < nDigit; i++) {
      char sval = string[i];
      int hval = 0;
      if (sval <= '9') {
         hval = sval - '0'; 
      } else {
        hval = sval - 'A' + 10; 
      }
      int digitPos = nDigit - i - 1;
      retval += hval << (4*digitPos);
  }
  return retval;
}

void checkSpeed() {
  char inputBuffer[16] = {0}; 
  int cpos = 0;
  boolean responseBegins = true;
  Serial1.print("010D\r\n"); 
  while (1) {
    if (Serial1.available() > 0) {
      incomingByte = Serial1.read();
      if (incomingByte == '4') {
         responseBegins = true; 
      }
      if (responseBegins) {
         inputBuffer[cpos++] = (char)incomingByte;
      }
      if (incomingByte == '>') {
         break; 
      }
    }
  }
  int speedval = ascii2int(&inputBuffer[6], 2);
  for (cpos = 0; cpos < 16; cpos++) {
     Serial.print(inputBuffer[cpos]); 
  }
  Serial.println();
  Serial.println(speedval);
}

void loop() {
  long ctime = millis();
  if ((ctime - ptime) > interval) {
    ptime = ctime;
    checkSpeed();
  }
  send_and_receive_bullshit();
}

void send_and_receive_bullshit() {
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

