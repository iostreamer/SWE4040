//DFRobot.com
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.home(); 
  // Print a message to the LCD.
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.setCursor(0,1);
}

String line1 = String("your speed is 2");
String line2 = String("Some really really long text   ");
  int msgWidth = 16;
  int msgoffset = 0;
  
void lcdShow(String l1, String l2) {
  lcd.setCursor(0,0);
  lcd.print(l1);
  lcd.setCursor(0,1);
  lcd.print(l2);
}

void loop()
{
  delay(500);
  int strlen = line2.length();
  if (strlen <= msgWidth) {
    lcdShow(line1, line2);
  } else {
    int msgleft = strlen - msgoffset;
    String msgpart;
    if (msgleft >= msgWidth) {
      msgpart = line2.substring(msgoffset, msgoffset+msgWidth);
    } else {
      msgpart = line2.substring(msgoffset, strlen);
      int remainder = strlen - msgoffset;
      msgpart += line2.substring(0, msgWidth-remainder);
    }
    lcdShow(line1, msgpart);
    msgoffset++;
    if (msgoffset > strlen-1) {
       msgoffset = 0; 
    }
  }
}
