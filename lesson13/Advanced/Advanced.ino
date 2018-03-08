#include <SPI.h>
#include <SD.h>

#include <DS1302.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define RST 4
#define DAT 3
#define CLK 2

boolean setClock = false;
DS1302 rtc(RST, DAT, CLK);

File myFile;

const int pingPin = 11;
const int echoPin = 12;

display
LiquidCrystal_I2C lcd(0x38,16,2); 

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  // Disable the Ethernet interface
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // proceed
  Serial.print("Initializing SD card communications...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  if (SD.exists("ADVNCD_SD.TXT"))
  {
    SD.remove("ADVNCD_SD.TXT");
    Serial.println("Previous ADVNCD_SD.TXT removed.");
  }
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("ADVNCD_SD.TXT", FILE_WRITE);
  if (!myFile) {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
 
  if (setClock)
  {
    // set the clock from a Time object
    Time t(2017, 10, 11, 11, 40, 00, Time::kWednesday);
    rtc.time(t); // when given a parameter, sets the time from
    // a time object
  }

  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight 
}
void loop()
{
  long duration, cm;
  Time t = rtc.time();
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  cm = microsecondsToCentimeters(duration);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to EASY_SD.TXT...");
    myFile.println(String(t.yr) + "-" + String(t.mon) + "-" +
                   String(t.date) + " " + String(t.hr) + ":" + String(t.min) +
                   ":" + String(t.sec)+" : "+cm);
    // close the file:
    //myFile.close();
    Serial.println("done.");
    lcd.setCursor(0,0);
    lcd.print("*THIS IS A TEST*");
    lcd.setCursor(0,1);
    lcd.print("TESTING LCD DISP");
    delay(1000);
  }

}
long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.

  return microseconds / 29 / 2;
}
