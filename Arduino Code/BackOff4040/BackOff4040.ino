/*ECE4040Master
 This is the program for the arduino side of the group 2 project "Back off"
 Written by Philippe Leger
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

void setup(){
  // opens serial port, sets data rate to 9600 bps
  Serial.begin(9600);     // To PC (via USB)
  Serial1.begin(9600);    // Bluetooth Module port
  setUpBT();
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.home();
  
}

void setUpBT(void){
  
  //Configure BT Module
  Serial.println("Configuring BT Module...");
  
  //Get pin code from EEPROM
  for(int i=0;i<4;i++){
      pin[i] = EEPROM.read(i);      
    }
  
  Serial1.print("AT&F\r");
  delay(2000);
  Serial1.print("AT+BTMODE,3\r");
  delay(2000);
  Serial1.print("AT+UARTCONFIG,9600,N,1,0\r");
  delay(2000);
  Serial1.print("AT+BTNAME=\"Back-Off-4040\"\r");
  delay(2000);
  Serial1.print("AT+BTKEY=\"");
  Serial1.print(pin);
  Serial1.print("\"\r");
  delay(2000);
  Serial1.print("ATZ\r");
  
  Serial.println("Module Configured");
  
}

void androidReceive(void){

  delay(10);
  byteRead = Serial1.read(); //Read byte from Android Application
  int distance, Speed, sd, i=0;
  switch (byteRead){
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
    
  case 'N': //"N" for set new pin code in EEPROM. Must be followed by 4 digit pin code
    if(Serial1.available()<4)
      break;
    
    for(i=0;i<4;i++){
      byteRead = Serial1.read();
      EEPROM.write(i,byteRead);
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
}



int getDistance(void){
  //Replaced with ping ultrasonic sensor as of Feb 9 2014. 
  // Get the distance of the car behind the user in meters
  // Note that currently the sensor is a potentiometer and does not actually measure distance
  //int sensorVal = map(analogRead(A0),0,1023,0,30); //Maps pot input to distance measurment of 0 to 30 meters
  //return sensorVal;
  
  
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

int getSpeed(void){
  //Ask the car OBD for the current speed in km/h
  int Speed = 50; //Test value
  return Speed;
}

int getSafeDistance(void){
  int Speed = getSpeed();
  int safeDistance = 0.75*Speed/3.6; //Safe distance is distance a car travels in 3/4 of a second.
  return safeDistance;
}

void backOffCheck(void){
  //print to serial monitor
  Serial.print("You are ");
  Serial.print(getDistance());
  Serial.println(" meters behind me.");
  //print to lcd
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("You are ");
  lcd.print(getDistance());
  lcd.setCursor(0,1);
  lcd.print("meters behind me.");
  
  //Compare current speed with distance of the following car and display appropriate message
  if(getDistance() < getSafeDistance()){
    //Car behind needs to back off
    
    //print to serial monitor
    Serial.print("Please Back Off to a safe distance of ");  
    Serial.print(getSafeDistance());
    Serial.println(" meters.");
    
    //print to lcd
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Back off to ");
    lcd.print(getSafeDistance());
    lcd.setCursor(0,1);
    lcd.print("meters behind me.");
  }
 }

void resetPin(void){
  Serial.println("Resetting pin code to \"1234\"");
  char c = '1';
  for(int i=0;i<4;i++){
    EEPROM.write(i,c++); 
  } 
  delay(1500);
}

void loop(){

  //incoming Android App data
  if(Serial1.available() > 0){
    byteRead = Serial1.read(); //Read byte from Android Application
    if(byteRead == '$')//Check for "$" indicating a command is received
      androidReceive(); 
  }
  
  if (digitalRead(buttonPin) == HIGH) {
    resetPin();    
  }

  //The following code runs tasks only once every interval
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval){
    previousMillis = currentMillis;
    
    //Run tasks
    backOffCheck(); //Run back off check
    
  }//End intervaled tasks   

}



