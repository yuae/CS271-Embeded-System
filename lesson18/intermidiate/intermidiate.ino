// required libraries
#include <Wire.h>
int xPin = A2;
int yPin = A3;
int pcPin = A0;
int b = millis();
int greenPin = 2;
int redPin = 3;

void setup()
{
  Serial.begin(9600);
  pinMode(INPUT, xPin);
  pinMode(INPUT, yPin);
  pinMode(INPUT, pcPin);
}
void loop()
{
  
  while (millis() - b < 6000)
  {
    
      
      int x = analogRead(xPin);
      int y = analogRead(yPin);
      int pc = analogRead(pcPin);
      
      String sentData = "&Uyeyuan2&JY"+String(x)+":"+String(y)+"&PC"+String(pc);
      sentData.trim();
      Serial.println(sentData);

      String dataStatus = Serial.readString();
      dataStatus.trim();
      
      
      if (dataStatus.equals("ACK"))
      {
        digitalWrite(greenPin, HIGH);
        digitalWrite(redPin, LOW);
      }
      else if (dataStatus.equals("NAK"))
      {
        while(true)
        Serial.println(dataStatus);
        digitalWrite(greenPin, LOW);
        digitalWrite(redPin, HIGH);
        delay(50);
        digitalWrite(redPin, LOW);
        delay(450);
      }
      delay(600);
}
  delay(3000);
  b = millis();
}

