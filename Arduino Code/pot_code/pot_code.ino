  int sensorVal = 0;  

  void setup() {
 // Setup Serial communication with computer
    Serial.begin(9600);
  }

  void loop() {
 // Read the value from the sensor:
    sensorVal = map(analogRead(A0),0,1023,0,30); //Maps pot input to distance measurment of 0 to 30 meters
 
 // Send the value to the Serial Monitor
    Serial.print("The car behind me is ");
    Serial.print(sensorVal);
    Serial.println(" meters away.");

 // Interval between readings = 1 second
    delay(1000);                
  }
