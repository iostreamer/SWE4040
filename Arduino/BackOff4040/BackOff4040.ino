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
}

void androidReceive(void){

  byteRead = Serial1.read(); //Read byte from Android Application

  switch (byteRead){
  case 68: //"D" for distance measurment request
    int distance = getDistance();
    // Send the value to the Serial Monitor
    Serial.print("The car behind me is ");
    Serial.print(distance);
    Serial.println(" meters away.");

    // Send the distance value to the Android Application
    Serial1.print(distance);
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



