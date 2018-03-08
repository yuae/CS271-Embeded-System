#define SER 2 // serial data input to chip
#define OEn 3 // output enable
#define RCLK 4 // strobe to transfer shift data to storage (output) register
#define SRCLK 5 // strobe to shift current bit(s) over and accept new input at SER
#define SRCLRn 6 // strobe low to clear the storage register in one operation
// global LED status
unsigned int dataLED[] = {1,1,1,0,0,0,0,0};
void setup()
{
 // define all chip interface pins as OUTPUT
 for (int i = SER; i <= SRCLRn; i++)
 pinMode(i, OUTPUT);
 // set negative logic outputs to HIGH, as they
 // default to LOW on boot
 digitalWrite (OEn, HIGH);
 digitalWrite (SRCLRn, HIGH);
 // clear any spurious data in the storage register
 clearStorage();
 // enable output
 outputEnable(true);
}
void loop()
{

 for (int i = 0; i < 8; i++)
 {
 loadABit(dataLED[i]);
 }
 transferToStorage();
 advance();
 delay(200);
}
void advance()
{
 unsigned int hold = dataLED[7]; // hold rightmost bit
 // shift other bits right
 for (int i = 6; i >= 0; i--)
 dataLED[i+1] = dataLED[i];
 // rightmost bit enters at left
 dataLED[0] = hold;
}
void clearStorage()
{
 digitalWrite(SRCLRn, LOW); // strobe the clear line negative
 delay(2);
 digitalWrite(SRCLRn, HIGH); // return to default HIGH
}
void outputEnable(boolean isEnabled)
{
 digitalWrite(OEn, !isEnabled); // negative logic
}
void transferToStorage()
{
 // strobe data into the storage (output) register
 digitalWrite(RCLK, HIGH);
 delay(2);
 digitalWrite(RCLK, LOW);
}
void loadABit(unsigned int bitValue)
{
 // load at bit into the shift accumulator register
 digitalWrite(SER, bitValue); // set output data
 digitalWrite(SRCLK, HIGH); // strobe bit shift line
 delay(2);
 digitalWrite(SRCLK, LOW);
}
