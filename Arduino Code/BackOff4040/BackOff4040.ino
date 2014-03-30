/*BackOff4040
 This is the program for the arduino side of the group 2 project "Back off"
 Written by Philippe Leger & Taeler Dixon
 Winter 2014
 */
 
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

byte byteRead; //Incoming data from Android Application
long previousMillis = 0; //Used for timing of tasks without using delay() function
long interval = 2000; //Set task interval in milliseconds
char pin[4]; //Pin code for BT module
const int pingPin = 53; //Digital pin used for distance sensor (range 2-300 cm)
const int buttonPin = 52;     // the number of the pushbutton pin
LiquidCrystal_I2C lcd(0x20,16,2);  // set the LCD address to 0x20(Cooperate with 3 short circuit caps) for a 16 chars and 2 line display
boolean OBDflag = false;

void setup(){
  // opens serial port, sets data rate to 9600 bps
  Serial.begin(9600);     // To PC (via USB)
  Serial1.begin(9600);    // Bluetooth Module port
  Serial2.begin(9600);    // Bluetooth Module port
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.home();
  
  Serial.println("System ready");
  
}

void setUpBT(void){
  
  //Configure BT Module
  Serial.println("Configuring BT Modules...");
  
  //Get pin code from EEPROM
  for(int i=0;i<4;i++){
      pin[i] = EEPROM.read(i);      
    }
  
  Serial1.print("AT&F\r"); //Factory reset
  Serial2.print("AT&F\r");
  delay(2000);
  Serial1.print("AT+BTMODE,3\r"); //Mode 3
  delay(2000);
  Serial1.print("AT+UARTCONFIG,9600,N,1,0\r"); //9600 Baud, N parity, 1 stop bit, no hardware handshaking
  Serial2.print("AT+UARTCONFIG,9600,N,1,0\r"); //9600 Baud, N parity, 1 stop bit, no hardware handshaking
  delay(2000);
  Serial1.print("AT+BTNAME=\"Back-Off-4040\"\r");
  Serial2.print("AT+BTNAME=\"Back-Off-4040-OBD\"\r");

  delay(2000);
  Serial1.print("AT+BTSEC,1,0"); //Set Authentication ON and encryption OFF
  delay(2000);
  Serial1.print("AT+BTKEY=\""); //Set PIN Code
  Serial1.print(pin);
  Serial1.print("\"\r");
  delay(2000);
  Serial1.print("ATZ\r"); //software reset (powercycle) in order to apply changes
  Serial2.print("ATZ\r"); //software reset (powercycle) in order to apply changes
  delay(2000);
  Serial.println("Modules Configured");
  
}

void obdConnect(void){
  OBDflag = false;
  Serial.println("Connecting to OBD");
 
  Serial2.print("ATDAABBCC112233\r");
  waitForOBDConfirmation();
  delay(2000); 
}


void waitForOBDConfirmation() {
  delay(8000);
  int i = 10000; //check some number of times
  Serial.println("Attempting OBD Connection");
  while (i--) {
    if (Serial2.available() > 0) {
      
      // read the incoming byte:
      byteRead = Serial2.read();
      Serial.print((char)byteRead);
      if (byteRead == 'C') {
        OBDflag = true;
        Serial2.print("AT Z\r");
        delay(2000);
        Serial2.print("AT SP 0\r");
        delay(1000);
        Serial2.print("01 00\r");
        delay(5000);  
        for (i = 0; i < 1000; i++) {
        sendReceive(); 
        }
        Serial.println("Connection success");
      return;
      }
    }
    
   }
   Serial.println("No connection detected");
}

void sendReceive() {
  //Send
  if (Serial.available() > 0) {
    // read the incoming byte:
    byteRead = Serial.read();

    // say what you got:
    Serial2.print((char)byteRead);
    Serial.print((char)byteRead);
  }

  // Receive 
  if (Serial2.available() > 0) {
    
    // read the incoming byte:
    byteRead = Serial2.read();

    // say what you got:
    //Serial.write(byteRead);
    Serial.print((char)byteRead);
  }
}


void androidReceive(void){

  delay(10);
  byteRead = Serial1.read(); //Read byte from Android Application
  Serial1.print('<'); //Acknowledge command received
  int distance, Speed, sd, i=0;
  switch (byteRead){
    case 'C': //"C" for attempt recconect to OBD
      obdConnect();
      if(OBDflag){
        Serial1.print("Success");  
      }else{
        Serial1.print("Failure");  
      }
    
    case 'D': //"D" for distance measurment request
      distance = getDistance();
      // Send the value to the Serial Monitor
      Serial.print("The car behind me is ");
      Serial.print(distance);
      Serial.println(" meters away.");
  
      // Send the distance value to the Android Application
      Serial1.print(distance);
      
      break;
      
    case 'F': //"F" for current safe following distance
      sd = getSafeDistance(); 
      // Send the value to the Serial Monitor
      Serial.print("The current safe following distance for the vehicle behind is ");
      Serial.print(sd);
      Serial.println(" meters.");
  
      // Send the distance value to the Android Application
      Serial1.print(sd);
      
      break;
    
    case 'M': //"M" for new custom message
      byteRead = Serial1.read();
      if(byteRead == 'B'){ //New "Back Off" message
        Serial.print("New \"Back Off\" Message is ");
        i = 10; //Starting location of "Back Off" message
        while(Serial1.peek()){
          if(i>49)
            break;
          byteRead = Serial1.read(); //Get next byte
          Serial.print((char)byteRead);
          EEPROM.write(i++, byteRead);
          delay(4);
        }
        EEPROM.write(i, 0);
        Serial.println("");
        
        break;
      }else if(byteRead == 'H'){ //New "Have a nice day" message
        Serial.print("New \"Have a nice day\" Message is ");
        i = 50; //Starting location of "Have a nice day" message
        while(Serial1.peek()){
          if(i>89)
            break;
          byteRead = Serial1.read(); //Get next byte
          Serial.print((char)byteRead);
          EEPROM.write(i++, byteRead);
          delay(4);
        }
        EEPROM.write(i, 0);
        Serial.println("");
        break;  
      }
      break;
      
    case 'N': //"N" for set new pin code in EEPROM. Must be followed by 4 digit pin code
      if(Serial1.available()<4)
        break;
      
      for(i=0;i<4;i++){
        byteRead = Serial1.read();
        EEPROM.write(i,byteRead);
      }   
      
      break;
      
    case 'O': //"O" for OBDII command
      if(OBDflag){ //Only if OBD connection is available
        while(byteRead = Serial1.read()){ //Give OBD command to OBD until NULL character is found
          Serial2.print(byteRead);   
        }
        Serial2.print("\r");
              
        while(!Serial2.available()){ //Wait for response from OBD
        }
        
        while(byteRead = Serial2.read() != '>'){ //Give OBD response back to Android app
          Serial1.print(byteRead);  
        }
        
      }else{
        Serial1.print("No OBDII");
        Serial.println("No OBDII connection");
      } 
      break; 
      
    case 'P': //"P" for current pin code stored in EEPROM
      for(i=0;i<4;i++){
        pin[i] = EEPROM.read(i);      
      }
      
      Serial.print("Current PIN code stored in EEPROM for BT module is \"");
      Serial.print(pin);
      Serial.println("\".");
      
      Serial1.println(pin);
      break;  
      
    case 'S': //"S" for current speed
      Speed = getSpeed();  
      // Send the value to the Serial Monitor
      Serial.print("This vehicle is going ");
      Serial.print(Speed);
      Serial.println(" km/h.");
  
      // Send the distance value to the Android Application
      Serial1.print(Speed);
      break;
      
    case 'T': //"T" for terminate BlueTooth connection
      
      // Send the terminate command to the BT module
      Serial1.print("+++\r");
      
      break;
      
    
  }
  Serial1.print('>'); //End of command execution
}



int getDistance(void){
  //Parts of the following code is provided and openly available from http://arduino.cc/en/Tutorial/ping
  long duration, cm, distance;
  
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  
  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  // convert the time into a distance
    
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  cm =  duration / 29 / 2;
  
  //Since this is a scalled down model, we will convert this value in cm to a value in meter using 
  //10 cm = 5m.
  distance = cm/2;
  
  return(distance);
} 

//int getSpeed(void){
//  //Ask the car OBD for the current speed in km/h
//  int Speed = 50; //Test value
//  return Speed;
//}

int getSpeed(void){
  //Ask the car OBD for the current speed in km/h
  if(!OBDflag){ //Not connected to OBD, use test value
    int Speed = 50; //Test value
    return Speed;
  }else{
    //read from obd
    char inputBuffer[46] = {0}; 
    int cpos = 0;
    boolean responseBegins = true;
    Serial2.print("010D\r\n"); 
    while (1) {
      if (Serial2.available() > 0) {
        byteRead = Serial2.read();
        if (byteRead == '4') {
          responseBegins = true; 
        }
        if (responseBegins) {
          inputBuffer[cpos++] = (char)byteRead;
        }
        if (byteRead == '>') {
          break; 
        }
      }
    }
    int Speed = ascii2int(&inputBuffer[11], 2);
    for (cpos = 0; cpos < 16; cpos++) {
     Serial.print(inputBuffer[cpos]); 
    }
    Serial.println();
    Serial.println(Speed);
    return(Speed);
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

int getSafeDistance(void){
  int Speed = getSpeed();
  int safeDistance = 0.75*Speed/3.6; //Safe distance is distance a car travels in 3/4 of a second.
  return safeDistance;
}

void backOffCheck(void){
  int i = 0;
  char inbuf[17] = "               \0";
  inbuf[16] = 0;
    
  //print to serial monitor
  Serial.print("We are ");
  Serial.print(getDistance());
  Serial.println("m apart ");
  //print to lcd
  
  lcd.setCursor(0,0);
  lcd.print("We are ");
  lcd.print(getDistance());
  lcd.print("m apart ");
  
  //Compare current speed with distance of the following car and display appropriate message
  if(getDistance() < getSafeDistance()){
    //Car behind needs to back off
    
    //print to serial monitor
    Serial.print("Please Back Off to a safe distance of ");  
    Serial.print(getSafeDistance());
    Serial.println(" meters.");
    
    //"Back Off" message
    lcd.setCursor(0,1);
    i = 10;
    while ((i < 26) && (byteRead = EEPROM.read(i++))) {
       inbuf[i-11] = byteRead;
    }
    lcd.print(inbuf);
    Serial.print(inbuf); Serial.print(".");
    Serial.println("");
    
  }
  else{
    //"Have a nice day" message
    lcd.setCursor(0,1);
    i = 50;
    while ((i < 66) && (byteRead = EEPROM.read(i++))) {
       inbuf[i-51] = byteRead; 
    }
    lcd.print(inbuf);
    Serial.print(inbuf); Serial.print(".");
    Serial.println("");
  }
 }

void configButton(void){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Configuring...");
  setUpBT();
  
  //reset pin code
  Serial.println("Resetting pin code to \"1234\"");
  char c = '1';
  for(int i=0;i<4;i++){
    EEPROM.write(i,c++); 
  } 
  
  resetMessages();
  
  obdConnect();
 
  Serial.println("Setup Complete");
  lcd.setCursor(0,0);
  lcd.print("Setup Complete  ");
   delay(1500);
}

void resetMessages(){
  int i = 0;
  char msg1[16] = "Please Back Off";
  char msg2[16] = "Have a nice day";
  
  for(i=0;i<=16;i++){//Reset Back Off message
    EEPROM.write(i+10,msg1[i]);  
  }
  for(i=0;i<=16;i++){//Reset Back Off message
    EEPROM.write(i+50,msg2[i]);  
  }
    
}

void loop(){

  //incoming Android App data
  if(Serial1.available() > 0){
    byteRead = Serial1.read(); //Read byte from Android Application
    if(byteRead == '$'){//Check for "$" indicating a command is received
      Serial.println("Receiving Android Command");
      androidReceive();
    } 
    else{
      Serial.print((char)byteRead); //Print to serial monitor
    }  
  }
  
  if (digitalRead(buttonPin) == HIGH) { //Check for button press
    configButton();    
  }

  //The following code runs tasks only once every interval
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval){
    previousMillis = currentMillis;
    
    //Run tasks
    backOffCheck(); //Run back off check
    
  }//End intervaled tasks   

}



