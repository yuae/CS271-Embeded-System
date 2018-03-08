int potPin = 4;                     
float temperature = 0;               
long val=0;                       
 
void setup()
{
Serial.begin(9600);
}
 
void loop ()                     
{
   val=analogRead(potPin);
   temperature = (val*0.0048828125*100);
   Serial.print("Tep=  ");
   Serial.print(temperature);
   Serial.println(" C");
   delay(500);
 
}
