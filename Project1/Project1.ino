//libraries
#include <SPI.h>
#include <SD.h>
#include <DS1302.h>
#include <GY_85.h>
#include <Wire.h>

//constants
#define RST A0
#define DAT A1
#define CLK A2

//Objects
DS1302 rtc(RST, DAT, CLK);
GY_85 GY85;

//variables
File myFile;
int startPin = 2;
int stopPin = 3;
int greenPin = 5;
int yellowPin = 6;
int redPin = 7;
int count = 0;
int breakTime = 2000;
long initTime = millis();
boolean setClock = false;
boolean pStatus = false;
int pExit = 0;

void setup() {
  Wire.begin();
  delay(10);

  Serial.begin(9600);
  delay(10);

  GY85.init();
  delay(10);
  Serial.println("\n\nGY-85 online. Commencing display.");

  if (setClock)
  {
    Time t(2017, 11, 5, 14, 30, 00, Time::kSunday);
    rtc.time(t);
    Serial.println("\n\nTime set.");
  }

  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  pinMode(yellowPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  pinMode(startPin, INPUT);
  pinMode(stopPin, INPUT);

  // proceed
  Serial.print("\n\nInitializing SD card communications...");
  if (!SD.begin(4))
  {
    Serial.println("initialization failed!");
    while (true)
    {
      digitalWrite(yellowPin, LOW);
      digitalWrite(redPin, HIGH);
      delay(500);
      digitalWrite(redPin, LOW);
      digitalWrite(yellowPin, HIGH);
      delay(500);
    }
  }
  Serial.println("initialization done.");
  
  if (SD.exists("accelbus.csv"))
  {
    SD.remove("accelbus.csv");
    Serial.println("Previous accelbus.csv removed.");
  }
  myFile = SD.open("accelbus.csv", FILE_WRITE);
  
  if (myFile)
  {

    myFile.print("sequence, time, avgAX, minAX, maxAX, avgAY, minAY, maxAY, avgAZ, minAZ, maxAZ, ");
    myFile.print("avgGX, minGX, maxGX, avgGY, minGY, maxGY, avgGZ, minGZ, maxGZ, avgCX, minCX, ");
    myFile.println("maxCX, avgCY, minCY, maxCY, avgCZ , minCZ, maxCZ");

    myFile.close();
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.println("error opening accelbus.csv");
    while (true)
    {
      digitalWrite(yellowPin, LOW);
      digitalWrite(redPin, HIGH);
      delay(500);
      digitalWrite(redPin, LOW);
      digitalWrite(yellowPin, HIGH);
      delay(500);
    }
  }
}

void loop()
{
  while (!pStatus)
  {
    digitalWrite(yellowPin, HIGH);
    if (digitalRead(startPin) == HIGH)
    {
      digitalWrite(yellowPin, LOW);
      pStatus = true;
    }
  }




  float maxAX,
        minAX,
        totalAX = 0.0,
        maxAY,
        minAY,
        totalAY = 0.0,
        maxAZ,
        minAZ,
        totalAZ = 0.0,
        maxGX,
        minGX,
        totalGX = 0.0,
        maxGY,
        minGY,
        totalGY = 0.0,
        maxGZ,
        minGZ,
        totalGZ = 0.0,
        maxCX,
        minCX,
        totalCX = 0.0,
        maxCY,
        minCY,
        totalCY = 0.0,
        maxCZ,
        minCZ,
        totalCZ = 0.0;

  int times = 0;


  initTime = millis();
  while (millis() - initTime < breakTime)
  {

    float ax = 0.0039 * GY85.accelerometer_x(GY85.readFromAccelerometer());
    if (times == 0)
    {
      maxAX = ax;
      minAX = ax;
    }
    else
    {
      maxAX = max(maxAX, ax);
      minAX = min(minAX, ax);
    }
    totalAX += ax;

    float ay = 0.0039 * GY85.accelerometer_y(GY85.readFromAccelerometer());
    if (times == 0)
    {
      maxAY = ay;
      minAY = ay;
    }
    else
    {
      maxAY = max(maxAY, ay);
      minAY = min(minAY, ay);
    }
    totalAY += ay;

    float az = 0.0039 * GY85.accelerometer_z(GY85.readFromAccelerometer());
    if (times == 0)
    {
      maxAZ = az;
      minAZ = az;
    }
    else
    {
      maxAZ = max(maxAZ, az);
      minAZ = min(minAZ, az);
    }
    totalAZ += az;


    float cx = fmod((0.92 * GY85.compass_x(GY85.readFromCompass()) - 18), 360);
    if (times == 0)
    {
      maxCX = cx;
      minCX = cx;
    }
    else
    {
      maxCX = max(maxCX, cx);
      minCX = min(minCX, cx);
    }
    totalCX += cx;

    float cy = fmod((0.92 * GY85.compass_y(GY85.readFromCompass()) - 18), 360);
    if (times == 0)
    {
      maxCY = cy;
      minCY = cy;
    }
    else
    {
      maxCY = max(maxCY, cy);
      minCY = min(minCY, cy);
    }
    totalCY += cy;

    float cz = fmod((0.92 * GY85.compass_z(GY85.readFromCompass()) - 18), 360);
    if (times == 0)
    {
      maxCZ = cz;
      minCZ = cz;
    }
    else
    {
      maxCZ = max(maxCZ, cz);
      minCZ = min(minCZ, cz);
    }
    totalCZ += cz;


    float gx = GY85.gyro_x(GY85.readGyro()) / 14.375;
    if (times == 0)
    {
      maxGX = gx;
      minGX = gx;
    }
    else
    {
      maxGX = max(maxGX, gx);
      minGX = min(minGX, gx);
    }
    totalGX += gx;

    float gy = GY85.gyro_y(GY85.readGyro()) / 14.375;
    if (times == 0)
    {
      maxGY = gy;
      minGY = gy;
    }
    else
    {
      maxGY = max(maxGY, gy);
      minGY = min(minGY, gy);
    }
    totalGY += gy;

    float gz = GY85.gyro_z(GY85.readGyro()) / 14.375;
    if (times == 0)
    {
      maxGZ = gz;
      minGZ = gz;
    }
    else
    {
      maxGZ = max(maxGZ, gz);
      minGZ = min(minGZ, gz);
    }
    totalGZ += gz;

    times++;

  }

  //check valid data
  if (times > 1)
  {
    Time t = rtc.time();
    // check valid time
    if (t.yr == 2017)
    {
      count++;
      myFile = SD.open("accelbus.csv", FILE_WRITE);
      // if the file opened okay, write to it:
      if (myFile)
      {
        Serial.print("Writing to accelbus.csv...");

        myFile.print(count);
        myFile.print(", ");
        myFile.print(String(t.yr) + "-" + String(t.mon) + "-" +
                     String(t.date) + " " + String(t.hr) + ":" + String(t.min) +
                     ":" + String(t.sec));
        myFile.print(", ");
        myFile.print(totalAX / times);
        myFile.print(", ");
        myFile.print(minAX);
        myFile.print(", ");
        myFile.print(maxAX);
        myFile.print(", ");
        myFile.print(totalAY / times);
        myFile.print(", ");
        myFile.print(minAY);
        myFile.print(", ");
        myFile.print(maxAY);
        myFile.print(", ");
        myFile.print(totalAZ / times);
        myFile.print(", ");
        myFile.print(minAZ);
        myFile.print(", ");
        myFile.print(maxAZ);
        myFile.print(", ");
        myFile.print(totalGX / times);
        myFile.print(", ");
        myFile.print(minGX);
        myFile.print(", ");
        myFile.print(maxGX);
        myFile.print(", ");
        myFile.print(totalGY / times);
        myFile.print(", ");
        myFile.print(minGY);
        myFile.print(", ");
        myFile.print(maxGY);
        myFile.print(", ");
        myFile.print(totalGZ / times);
        myFile.print(", ");
        myFile.print(minGZ);
        myFile.print(", ");
        myFile.print(maxGZ);
        myFile.print(", ");
        myFile.print(fmod((totalCX / times), 360));
        myFile.print(", ");
        myFile.print(minCX);
        myFile.print(", ");
        myFile.print(maxCX);
        myFile.print(", ");
        myFile.print(fmod((totalCY / times), 360));
        myFile.print(", ");
        myFile.print(minCY);
        myFile.print(", ");
        myFile.print(maxCY);
        myFile.print(", ");
        myFile.print(fmod((totalCZ / times), 360));
        myFile.print(", ");
        myFile.print(minCZ);
        myFile.print(", ");
        myFile.println(maxCZ);

        // close the file:
        myFile.close();
        Serial.println("done.");
      }
      else
      {
        // if the file didn't open, print an error:
        Serial.println("error opening accelbus.csv");
        while (true)
        {
          digitalWrite(yellowPin, LOW);
          digitalWrite(redPin, HIGH);
          delay(500);
          digitalWrite(redPin, LOW);
          digitalWrite(yellowPin, HIGH);
          delay(500);
        }
      }
    }
  }

  //flash green led
  digitalWrite(greenPin, HIGH);
  delay(500);
  digitalWrite(greenPin, LOW);
  initTime = millis();
  while (millis() - initTime < 4500)
  {
    if (digitalRead(stopPin) == HIGH)
    {
      while (true)
      {
        digitalWrite(redPin, HIGH);
      }
    }
  }
}


