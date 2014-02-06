/*ECE4040Master
 This is the program for the arduino side of the group 2 project "Back off"
 Written by Philippe Leger
 Winter 2014
 */

byte byteRead; //Incoming data from Android Application
long previousMillis = 0; //Used for timing of tasks without using delay() function
long interval = 2000; //Set task interval to 2 seconds

void setup(){
  // opens serial port, sets data rate to 9600 bps
  Serial.begin(9600);     // To PC (via USB)
  Serial1.begin(9600);    // Bluetooth Module port
  
  //Configure BT Module
  Serial.println("Configuring BT Module...");
  
  Serial1.print("AT&F\r");
  delay(2000);
  Serial1.print("AT+BTMODE,3\r");
  delay(2000);
  Serial1.print("AT+UARTCONFIG,9600,N,1,0\r");
  delay(2000);
  Serial1.print("ATZ\r");
  
  Serial.println("Module Configured");
}

void getPw() { //in progress
 char in;
 char pw[17] = {0};
 int pwloc = 0;
 delay(10);
 while (!Serial1.available()); 
 while((in = Serial1.read()) != '&') {
    pw[pwloc++] = in;
    if (pwloc >= 16)
      break;  
   while (!Serial1.available()); 
 }
 String rs = String("AT+whatever" + pw);
 }

void androidReceive(void){

  delay(10);
  byteRead = Serial1.read(); //Read byte from Android Application
  int distance, Speed, sd;
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
    
  case 'S': //"S" for current speed
    Speed = getSpeed();  
    // Send the value to the Serial Monitor
    Serial.print("This vehicle is going ");
    Serial.print(Speed);
    Serial.println(" km/h.");

    // Send the distance value to the Android Application
    Serial1.print(Speed);
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
  }
}

int getDistance(void){
  // Get the distance of the car behind the user in meters
  // Note that currently the sensor is a potentiometer and does not actually measure distance
  int sensorVal = map(analogRead(A0),0,1023,0,30); //Maps pot input to distance measurment of 0 to 30 meters
  return sensorVal;
} 

int getSpeed(void){
  //Ask the car OBD for the current speed in km/h
  int Speed = 80; //Test value
  return Speed;
}

int getSafeDistance(void){
  int Speed = getSpeed();
  int safeDistance = 0.75*Speed/3.6; //Safe distance is distance a car travels in 3/4 of a second.
  return safeDistance;
}

void backOffCheck(void){
  Serial.print("You are ");
  Serial.print(getDistance());
  Serial.println(" meters behind me.");
  //Compare current speed with distance of the following car and display appropriate message
  if(getDistance() < getSafeDistance()){
    //Car behind needs to back off
    Serial.print("Please Back Off to a safe distance of ");  
    Serial.print(getSafeDistance());
    Serial.println(" meters.");
  }
  

}

void loop(){

  //incoming Android App data
  if(Serial1.available() > 0){
    byteRead = Serial1.read(); //Read byte from Android Application
    if(byteRead == '$')//Check for "$" indicating a command is received
      androidReceive(); 
  }

  //The following code runs tasks only once every interval
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval){
    previousMillis = currentMillis;
    
    //Run tasks
    backOffCheck(); //Run back off check
    
  }//End intervaled tasks   

}



