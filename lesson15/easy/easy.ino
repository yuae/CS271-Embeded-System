int pinSwitch = 3; //pin 3 connects to switch  
int lastValue = 0; // last value of pin  
int checkTimes = 0; // check times for key jitter  
  
void setup() {  
  pinMode(pinSwitch, INPUT); //set pinSwitch INPUT  
  lastValue = digitalRead(pinSwitch); //get last value  
}  
  
void loop() {   
  int value = digitalRead(pinSwitch); //read the switch  
    
  if (value != lastValue) { //if value changed   
    //eliminate key jitter: check 4 times   
    checkTimes = 0;   
    delay(100); //delay 100 ms  
    while ( value == digitalRead(pinSwitch) && checkTimes++ < 4 )  
      checkTimes++;  
        
    //if continous 4 times  
    if (checkTimes >= 4 ) {  
      if(value==1)
        digitalWrite(13,HIGH); 
      else
        digitalWrite(13,HIGH);
      lastValue = value;  
    }    
  }  
    
  delay(10); //delay in between reads for stability    
} 
