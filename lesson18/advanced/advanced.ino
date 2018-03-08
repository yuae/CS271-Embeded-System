#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x38, 20, 4);
#define MMA8452_ADDRESS 0x1C
#define OUT_X_MSB 0x01
#define XYZ_DATA_CFG  0x0E
#define WHO_AM_I_REG   0x0D
#define CTRL_REG1  0x2A
#define GSCALE 2 // Sets full-scale range to +/-2, 4, or 8g. Used to calc real g values.
String axis[3];
int b = millis();
int OBS = 0;

void setup()
{
  Serial.begin(9600);
  Wire.begin(); //Join the bus as a master
  initMMA8452();
  lcd.init(); // init LCD
  lcd.backlight(); // ensure backlight is ON
}
void loop()
{

  lcd.clear();
  
  if (OBS < 121)
  {
    int accelCount[3];  // Stores the 12-bit signed value
    readAccelData(accelCount);  // Read the x/y/z adc values

    // Now we'll calculate the accleration value into actual g's
    float accelG[3];  // Stores the real accel value in g's
    for (int i = 0 ; i < 3 ; i++)
    {
      // get actual g value, this depends on what GSCALE was set
      accelG[i] = (float) accelCount[i] / ((1 << 12) / (2 * GSCALE));
      //1<<12 generates 2^12 (two to the power of twelve)
    }

    String x = getFloat(accelG[0], 2);
    String y = getFloat(accelG[1], 2);
    String z = getFloat(accelG[2], 2);

    String sentData = "&Uyeyuan3&AC" + x + ":" + y + ":" + z;
    sentData.trim();
    Serial.println(sentData);

  delay(100);

    
    String dataStatus = "STA: " + Serial.readString();
    dataStatus.trim();
    
    lcd.setCursor(0, 0);
    lcd.print("OBS:" + (String)(OBS));

    lcd.setCursor(0, 1);
    lcd.print(dataStatus);
    


    OBS++;

  }
  else
  {
    Serial.println("$TERM");
    while (true) {}
  }
  delay(1000);
}

void readAccelData(int *destination)
{
  byte rawData[6];  // x/y/z accel register data stored here

  readRegisters(OUT_X_MSB, 6, rawData);  // Read the six raw data registers into data array

  // Loop to calculate 12-bit ADC and g value for each axis
  for (int i = 0; i < 3 ; i++)
  {
    int gCount = (rawData[i * 2] << 8) | rawData[(i * 2) + 1]; //Combine the two 8 bit registers into one 12-bit number
    gCount >>= 4; //The registers are left align, here we right align the 12-bit integer

    // If the number is negative, we have to make it so manually (no 12-bit data type)
    if (rawData[i * 2] > 0x7F)
    {
      gCount = ~gCount + 1;
      gCount *= -1;  // Transform into negative 2's complement #
    }

    destination[i] = gCount; //Record this gCount into the 3 int array
  }
}

// Initialize the MMA8452 registers
// See the many application notes for more info on setting all of these registers:
// http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=MMA8452Q
void initMMA8452()
{
  //Serial.println("Initialize IMU");
  byte c = readRegister(WHO_AM_I_REG);  // Read WHO_AM_I register
  //Serial.println("Reading WHOAMI");
  if (c == 0x2A) // WHO_AM_I should always be 0x2A
  {
    //Serial.println("MMA8452Q (GY-45) is online...");
  }
  else
  {
    //Serial.print("Could not connect to MMA8452Q (GY-45): 0x");
    //Serial.println(c, HEX);
    while (true) ; // stall if communication doesn't happen
  }
  //Serial.println("Gyros in standby.");
  MMA8452Standby();  // Must be in standby to change registers

  // Set up the full scale range to 2, 4, or 8g.
  byte fsr = GSCALE;
  if (fsr > 8) fsr = 8; //Easy error check
  fsr >>= 2; // 00 = 2G, 01 = 4A, 10 = 8G
  writeRegister(XYZ_DATA_CFG, fsr);

  //The default data rate is 800Hz;
  //we don't need to modify it in this example code
  //Serial.println("Gyros activated.");
  MMA8452Active();  // Set to active to start reading
}

// Sets the MMA8452 to standby mode. It must be in standby to change most register settings
void MMA8452Standby()
{
  byte c = readRegister(CTRL_REG1);
  //Clear the active bit to go into standby
  writeRegister(CTRL_REG1, c & ~(0x01));
}

// Sets the MMA8452 to active mode. Needs to be in this mode to output data
void MMA8452Active()
{
  byte c = readRegister(CTRL_REG1);
  //Set the active bit to begin detection
  writeRegister(CTRL_REG1, c | 0x01);
}

// Read bytesToRead sequentially, starting at addressToRead into the dest byte array
void readRegisters(byte addressToRead, int bytesToRead, byte * dest)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToRead);
  Wire.endTransmission(false); //endTransmission but keep the connection active

  Wire.requestFrom(MMA8452_ADDRESS, bytesToRead); //Ask for bytes, once done, bus is released by default

  while (Wire.available() < bytesToRead); //Hang out until we get the # of bytes we expect

  for (int x = 0 ; x < bytesToRead ; x++)
    dest[x] = Wire.read();
}

// Read a single byte from addressToRead and return it as a byte
byte readRegister(byte addressToRead)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToRead);
  Wire.endTransmission(false); //endTransmission but keep the connection active

  Wire.requestFrom(MMA8452_ADDRESS, 1); //Ask for 1 byte, once done, bus is released by default

  while (!Wire.available()) ; //Wait for the data to come back
  return Wire.read(); //Return this one byte
}

// Writes a single byte (dataToWrite) into addressToWrite
void writeRegister(byte addressToWrite, byte dataToWrite)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToWrite);
  Wire.write(dataToWrite);
  Wire.endTransmission(); //Stop transmitting
}

// rounding function

// gettFloat returns the float 'value' rounded to 'places' places after the decimal point
String getFloat(float value, int places)
{
  String returnValue = "";

  value *= pow(10, places); // move 2 dp over
  value = (float)(int)value; // truncate
  value /= pow(10, places); // move 2 dp back
  returnValue = String(value);
  return returnValue;
}


