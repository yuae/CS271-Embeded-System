#include <SPI.h>
#include <SD.h>
File myFile;
const int pingPin = 11;
const int echoPin = 12;
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
  if (SD.exists("EASY_SD.TXT"))
  {
    SD.remove("EASY_SD.TXT");
    Serial.println("Previous EASY_SD.TXT removed.");
  }
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("EASY_SD.TXT", FILE_WRITE);
  if(!myFile) {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
void loop()
{
  long duration, cm;
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
    if(cm<=15)
    myFile.println("Danger");
    else if(cm>15&&cm<=30)
    myFile.println("Warning");
    else
    myFile.println("OK");
    // close the file:
    //myFile.close();
    Serial.println("done.");
  }
  
}
long microsecondsToCentimeters(long microseconds)
{
 // The speed of sound is 340 m/s or 29 microseconds per centimeter.
 // The ping travels out and back, so to find the distance of the
 // object we take half of the distance travelled.

 return microseconds / 29 / 2;
}
