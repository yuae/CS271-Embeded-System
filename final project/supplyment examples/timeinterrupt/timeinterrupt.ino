// codes explaining how to use timer1 with CTC mode to generate a signal that allows interruption happens exactly 0.1ms per time
// some comments were wrote in traditional chinese briefly translated
// web site address : http://www.arduino.cn/thread-12445-1-1.html used the last example
// author also mentioned how to stop CTC in the comment of the website 
// void stopT1( ) {
//    TIMSK1 &= ( ~ (1 << OCIE1A) );  // 禁止(disable) CTC for TIMER1_COMPA_vect
// }
//void resumeT1( ) {
//    TIMSK1 |= (1 << OCIE1A);  // enable CTC for TIMER1_COMPA_vect
// }

// 控制 LED on pin 13亮滅, 每秒閃爍 2 次: 亮 0.25 秒滅 0.25 秒 ...(control led on pin 13 on and off : on 0.25 sec off 0.25 sec)
// LED on pin 8 每秒閃爍 1 次: 亮 0.5 秒滅 0.5 秒 ...(control led on pin 8 on 0.5sec and off 0.5sec)
const int intA = 2500;   // 2500 * 0.1 ms = 250ms
const int intB = 5000;   // 5000 * 0.1 ms = 500ms = 0.5秒
// Prescaler 用 64 (using 64 for prescaler)
volatile int ggyy = 1;  // 使用這當 Flag 給  ISR 使用 !(flag used by ISR)
int ledPin =13;
int led8 = 8;  // pin 8
/// For Prescaler == 64
///  1 秒(sec) / (16 000 000 / 64) = 1/250000 =  0.000004 sec / per cycle
/// 0.1 sec / 0.000004 sec -1 = 25000 -1 = 24999
/// 0.0001 sec / 0.000004 sec -1 = 25 -1 = 24
const int myTOP = 24;  // 0.0001 sec when Prescaler == 64
///// Interrupt Service Routine for TIMER1 CTC on OCR1A as TOP
/// 注意以下名稱是有意義的, 不可亂改 !(all name under has a meaning, do not change!)
ISR(TIMER1_COMPA_vect)
{
   static unsigned int aaa = 0;
   static unsigned int bbb = 0;
   ++aaa; bbb++;
   if(aaa == intA){
      aaa=0; myJobOne( );
   }
   if(bbb == intB){
      bbb=0; myJobTwo( );
   }
}
void setup( ) {
  pinMode(ledPin, OUTPUT);
  pinMode(led8, OUTPUT); digitalWrite(led8, 1); // 故意(not very sure why the authot wrote a "intentionally" here)
  digitalWrite(ledPin, LOW); // turn Off the LED
  setMyTimerOne( );
}
void loop() {
  //... 做其他事(other staff)
  // if( ggyy == 1) ...
}
void myJobOne( ) {
  digitalWrite(ledPin, ggyy);  // ggyy 是(is) 0 或(or) 1
  ggyy = 1 - ggyy; //  給下次進入 用(for next entry)
}
void myJobTwo( ) {
  digitalWrite(led8, ! digitalRead(led8));  // Toggle led8
}
////////
void setMyTimerOne( ){
  cli();  // 禁止中斷(disallow the interruption)
  TCCR1A = 0;
  TCCR1B = (1<<WGM12);  // CTC mode; Clear Timer on Compare
  TCCR1B |= (1<<CS10) | (1<<CS11);  // Prescaler == 64
  /////////
  OCR1A = myTOP;  // TOP count for CTC, 與 prescaler 有關(have something to do with prescaler)
  TCNT1=0;  // counter 歸零(reinitialize counter to zero) 
  TIMSK1 |= (1 << OCIE1A);  // enable CTC for TIMER1_COMPA_vect
  sei();  // 允許中斷(allow interruption)
}
//////////////////////
