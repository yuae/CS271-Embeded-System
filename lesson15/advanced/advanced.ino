int flame=A5;
int val=0;
void setup() 
{ 
  pinMode(flame,INPUT);
  Serial.begin(9600);
}
void loop() 
{ 
  val=analogRead(flame);
  Serial.println(val);
  if(val>=600）
  { digitalWrite(13,HIGH); } 
  else 
  { digitalWrite(13,LOW); } 
}
