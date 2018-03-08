// inspired by the examples from internet see supplyment codes for 
// referanced examples

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x38,20,4);
int count = 0;
int ledstate = LOW;
void setup()
{

  // set byte speed
  Serial.begin(9600);
  
  //set led display
  lcd.init();
  lcd.backlight();
  
  //set led pin
  pinMode(8, OUTPUT);
  pinMode(13, OUTPUT);

  //call set timer1
  setTimer1();

  //set pin 2 as a internal pullup pin so it's getting a HIGH signal itself
  //attach external interrupt pin 0(digital pin 2) with a handler function "yesterday"
  //because pin 2 is already a pullup itself so when button pressed it will invoke a LOW
  //signal to pin 2 which will interrupt all process and start "yesterday"
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(0, Yesterday, FALLING);
}

void setTimer1()
{
  // because timer0 is 8 bit it can only count to 254 so we are using timer 1
  // which is a 16 bit timer and can count up to 65535 so we can set the interruption
  // happen every second instead of every 1ms
  // Also note because used timer 1 Servo library and pin 9 and 10 will not repond
  // to us any more
  cli();//disable interruption
  TCCR1A = 0;
  TCCR1B = 0;//clear timer control register for both A and B
  TCCR1B = (1 << WGM12); //set CTC mode(Clear Timer on Compare)
  TCCR1B |= (1 << CS10) | (1 << CS12); //use 1024 prescaler
  OCR1A = 15624;//1sec when use 1024 as prescaler. most boards are 16mHz
  //so it tick at a 16000000/1024 Hz, and so 1024/16000000(0.000064) sec per tick
  //note I used 1024 as prescaler so it's devided by 1024
  //so we can calculate how many ticks it will have after 1sec
  //and 1 second is 1/0.000064-1(15624) ticks(minus 1 because start from 0)
  TIMSK1 |= (1 << OCIE0A);//enable timer1 mask register to check TIMER1_COMPA_vect
  sei();//enable interruption
}

// Interruption is called once a second, looks for any new GPS data, and stores it
ISR(TIMER1_COMPA_vect)
{
  count++;//increment time (in second)
  if (count == 60) //every minute we update our data
  {
    sendRequest();
    count = 0;// reinitialize count to 0
  }
  //flash led every second
  ledstate = 1-ledstate;
  digitalWrite(8, ledstate);
  digitalWrite(13,LOW);//clear yellow led
}
void sendRequest()
{
  Serial.println("?update");
  //String message = Serial.readString();
  String message = "TEMP:26.00/nHUM:50.00%/nDATE:2018-05-13 SAT";
  displayMessage(message);
}
//handler for external interruption
void Yesterday()
{
  //send request
  Serial.println("?summary");
  //flash yellow led when button pressed
  digitalWrite(8, LOW);
  digitalWrite(13, HIGH);
  //String history = Serial.read();
  String history = "TEMP:28.00/nHUM:46.00%/nDATE:2018-05-12 FRI";
  displayMessage(history);
}

//display message on the 2004 Display
void displayMessage(String data)
{
  lcd.clear();
  String temp = data.substring(0,9);
  String hum = data.substring(11,20);
  String date = data.substring(22);
  lcd.setCursor(0,0);
  lcd.print(temp);
  lcd.setCursor(0,1);
  lcd.print(hum);
  lcd.setCursor(0,2);
  lcd.print(date);
}

void loop()
{}
