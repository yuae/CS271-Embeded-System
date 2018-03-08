#include <SPI.h>
#include <SD.h>
File myFile;
void setup()
{
 // Open serial communications and wait for port to open:
 Serial.begin(9600);
// Disable the Ethernet interface
 pinMode(10, OUTPUT);
 digitalWrite(10, HIGH);

 // proceed
 Serial.print("Initializing SD card communications...");
 if (!SD.begin(4)) {
 Serial.println("initialization failed!");
 return;
 }
 Serial.println("initialization done.");
 if (SD.exists("test.txt"))
 {
 SD.remove("test.txt");
 Serial.println("Previous test.txt removed.");
 }
 // open the file. note that only one file can be open at a time,
 // so you have to close this one before opening another.
 myFile = SD.open("test.txt", FILE_WRITE);
 // if the file opened okay, write to it:
 if (myFile) {
 Serial.print("Writing to test.txt...");
 myFile.println("Ph'nglui mgwl'nafh Cthulhu “+ “R'lyeh wgah'nagl ftahgn.");
 // close the file:
 myFile.close();
 Serial.println("done.");
 } else {
 // if the file didn't open, print an error:
 Serial.println("error opening test.txt");
 }
 // re-open the file for reading:
 myFile = SD.open("test.txt");
 if (myFile) {
 Serial.println("test.txt:");
 // read from the file until there's nothing else in it:
 while (myFile.available()) {
 Serial.write(myFile.read());
 }
 // close the file:
 myFile.close();
 } else {
 // if the file didn't open, print an error:
 Serial.println("error opening test.txt");
 }
}
void loop()
{
 // nothing happens after setup
}
