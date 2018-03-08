void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000);
  analogReference(DEFAULT);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(3,~OUTPUT);
  pinMode(5,~OUTPUT);
  pinMode(6,~OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int value = (analogRead(A0)+analogRead(A1)+analogRead(A2))/3;
  Serial.println(value);
  if(value<=250)
  {
    analogWrite(3,255);
    analogWrite(5,0);
    analogWrite(6,0);
  }
  else if(value<=300)
  {
    analogWrite(3,0);
    analogWrite(5,255);
    analogWrite(6,0);
  }
  else
  {
    analogWrite(3,0);
    analogWrite(5,0);
    analogWrite(6,255);
  }
}
