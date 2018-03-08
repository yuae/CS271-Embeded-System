// inspired from internet examples, see supplyment examples for detials of the examples

//All libraries needed for this server sketch
#include "DHT.h"
#include <DS1302.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

//objects, variables decleration and initialization
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);
EthernetClient client;
String readString = "";
String lastTime;
String summary;
String history;
String date;
float yTemp = 0;
float yHum = 0;
int count = 0;
boolean setClock = false;
int second = 0;
int ledState = LOW;
#define ECHO 22
#define TRIG 23
#define RST 24
#define DAT 25
#define CLK 26
#define DHTPIN 30
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
DS1302 rtc(RST, DAT, CLK);
File logfile;

//set up enviroment
void setup()
{
  //set byte value
  Serial.begin(9600);

  //specify mac and ip address for ethernet
  Ethernet.begin(mac, ip);
  //start server
  server.begin();
  //print server infomation
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());

  //start initialize sd card reader
  Serial.print("Initializing SD card communications...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  if (SD.exists("datalog.txt"))
  {
    SD.remove("datalog.txt");
    Serial.println("Previous datalog.txt removed.");
  }
  //set led pin
  pinMode(31, OUTPUT);

  //call set timer1
  //this was oringinally for the client side but the client side did not
  //work at the end, so it was moved here to update the data every 5 mins
  //however by initialize timer interruption it crashed the data collected from dht
  //so this is commented out
  //setTimer1();

  //setting sensors
  sensorSet();
}

//setting timer
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
  second++;//increment time (in second)
  if (second == 300) //every 5 minute we update data
  {
    Update();
    second = 0;// reinitialize second to 0
  }
  //flash led every second
  ledState = 1 - ledState;
  digitalWrite(31, ledState);
}
//stop and resume interruption from timer1
void stopT1( ) {
  TIMSK1 &= ( ~ (1 << OCIE1A) );  // disable CTC for TIMER1_COMPA_vect
}
void resumeT1( ) {
  TIMSK1 |= (1 << OCIE1A);  // enable CTC for TIMER1_COMPA_vect
}
//setting sensors
void sensorSet()
{
  //start dht sensor
  dht.begin();

  // The following code would set the time to
  // December 14, 2017 at 16:57:00 pm, a Thursday
  if (setClock)
  {
    // set the clock from a Time object
    Time t(2017, 12, 14, 16, 57, 00, Time::kThursday);
    Serial.print("set");
    rtc.time(t); // when given a parameter, sets the time from
    // a time object
  }
  //get start time for later use
  Time t = rtc.time();
  while (int(t.yr) < 2017)
  {
    t = rtc.time();
  }
  date = (String(t.yr) + "-" + String(t.mon) + "-" + String(t.date));//store it in a variale
  //set ultronic distance sensor
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  //get first data
  Update();
}

//update data when request by client
void Update()
{
  //stopT1();
  long duration, inches, cm;
  // The ping is triggered by a HIGH pulse of 2 or more microseconds.
  // The documentation says 10uS, but 5 seems to suffice.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIG, LOW);
  // A different pin is used to read the return echo: a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  duration = pulseIn(ECHO, HIGH); // pulseIn is a predefined function
  // convert the time into a distance
  cm = duration / 29 / 2;

  //if noting is bloking the sensor
  if (cm > 30)
  {
    //start reading the temperature and humidity
    // Wait a few seconds between measurements.
    delay(2000);
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float hum = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float temp = dht.readTemperature();
    // Check if any reads failed and try again until data correctly collected
    while (isnan(hum) || isnan(temp))
    {
      Serial.println("NAN");
      delay(2000);
      float hum = dht.readHumidity();
      float temp = dht.readTemperature();
    }

    //get current time
    Time t = rtc.time();
    //checking wrong time
    while (int(t.yr) < 2017)
    {
      t = rtc.time();
    }
    String Time = (String(t.yr) + "-" + String(t.mon) + "-" +
                   String(t.date) + " " + String(t.hr) + ":" + String(t.min) +
                   ":" + String(t.sec));

    //if it's still in a day increment the temp and humidity
    if (date.indexOf(String(t.date)) > 0)
    {
      yTemp += temp;
      yHum += hum;
      count++;
    }
    //if a day is past, summarize the data and write it into the log on the sd card
    else
    {
      history = "TEMP:" + String(yTemp / count) + " HUM:" + String(yHum / count) + "% Recorded at:" + date;
      writeLog(history);
      yTemp = 0;
      yHum = 0;
      count = 0;
      date = String(t.yr) + "-" + String(t.mon) + "-" + String(t.date);
    }

    //update data
    lastTime = "TEMP:" + String(temp) + " HUM:" + String(hum) + "% Recorded at:" + Time;
  }
  //if sensor is blocked set data to error message
  else
  {
    lastTime = "Sensor Blocked. Please check the sensor soon!";
  }
  //debug line
  Serial.println("update:" + lastTime);
  //resumeT1();
}
void Summary()
{
  //stopT1();
  //get current time
  Time t = rtc.time();
  while (int(t.yr) < 2017)
  {
    t = rtc.time();
  }
  String Time = (String(t.yr) + "-" + String(t.mon) + "-" +
                 String(t.date) + " " + String(t.hr) + ":" + String(t.min) +
                 ":" + String(t.sec));
  //summarize data collected until now and store it in the summary variable
  summary = "TEMP:" + String(yTemp / count) + " HUM:" + String(yHum / count) + "% Summaried at:" + Time;
  //debug line
  Serial.println(summary);
  //resumeT1();
}

//method writng the log data to sd card
void writeLog(String str)
{
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  logfile = SD.open("datalog.txt", FILE_WRITE);
  // if the file opened okay, write to it:
  if (logfile) {
    Serial.print("Writing to datalog.txt...");
    logfile.println(history);
    // close the file:
    logfile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening datalog.txt");
  }
}

//checking for client request constantly
void loop() {
  client = server.available();
  if (client) {
    Serial.println("new client");

    //if client tried connecting
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();//receiving request
        readString += c;
        if (c == '\n') {
          //debug line to show the data received
          Serial.println(readString);
          //checking "update" request
          if (readString.indexOf("?update") > 0) {
            Update();
            //send data back to client
            client.println(lastTime);
            break;
          }
          //checking if "summary" reuest received
          if (readString.indexOf("?summary") > 0) {
            Summary();
            //send data back to client
            client.println(summary);
            break;
          }
          //send HTML file
          SendHTML();
          break;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("client disonnected");
    //reinitialize reading data
    readString = "";
  }
}

// HTML text function, revised from ethernetLed examole
void SendHTML()
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html><head><meta charset=\"UTF-8\"><title>Arduino Web Server</title><script type=\"text/javascript\">");
  client.println("function sendUpdate(){var xmlhttp;if (window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();else xmlhttp=new ActiveXObject(\"Microsoft.XMLHTTP\");xmlhttp.open(\"GET\",\"?update\",true); xmlhttp.send();xmlhttp.onreadystatechange=function(){if (xmlhttp.readyState==4 && xmlhttp.status==200)document.getElementById(\"info\").innerHTML=xmlhttp.responseText;};}");
  client.println("function getSummary(){var xmlhttp;if (window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();else xmlhttp=new ActiveXObject(\"Microsoft.XMLHTTP\");xmlhttp.open(\"GET\",\"?summary\",true); xmlhttp.send();xmlhttp.onreadystatechange=function(){if (xmlhttp.readyState==4 && xmlhttp.status==200)document.getElementById(\"summary\").innerHTML=xmlhttp.responseText;};}</script>");
  client.println("</head><body><div align=\"center\"><h1>Arduino Web Server</h1><div id=\"info\">");
  client.println(lastTime);
  client.println("</div><div id=\"summary\">");
  client.println(summary);
  client.println("</div><button id=\"update\" type=\"button\" onclick=\"sendUpdate()\">update</button><button id=\"summary\" type=\"button\" onclick=\"getSummary()\">summary</button></div></body></html>");
}
