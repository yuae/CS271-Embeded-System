#include <SPI.h>
#include <Ethernet.h>
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,177);
EthernetServer server(80);
EthernetClient client;
String readString=""; 
int Light=2;
int Sensor=A0;
 
void setup() {
  Serial.begin(9600);
  //初始化Ethernet通信(initialize ethernet connection)
  Ethernet.begin(mac, ip);
  server.begin();
  pinMode(Light,OUTPUT);
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
}
 
void loop() {
  // 监听连入的客户端(listening connecting client)
  client = server.available();
  if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = false;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        readString += c;
        if (c == '\n') {
          Serial.println(readString);
          //检查收到的信息中是否有”on”，有则开灯(if received "?on", turn on the led)
          if(readString.indexOf("?on") >0) {
            digitalWrite(Light, HIGH);
            Serial.println("Led On");
            break;
          }
          //检查收到的信息中是否有”off”，有则关灯(if "?off", turn off the led)
          if(readString.indexOf("?off") >0) {
            digitalWrite(Light, LOW);
            Serial.println("Led Off");
            break;
          }
          //检查收到的信息中是否有”getBrightness”，有则读取光敏模拟值，并返回给浏览器
          //checking if "getBrightness" received, if received read Cadmium-Sulfide Photoresistor (CdS)
          //value and send it back to browser
          if(readString.indexOf("?getBrightness") >0) {
            client.println(analogRead(Sensor));
            break;
          }
          //发送HTML文本(sent HTML text)
          SendHTML();
          break;
        }        
      }
    }
    delay(1);
    client.stop();
    Serial.println("client disonnected");
    readString=""; 
  }
}
 
// 用于输出HTML文本的函数(function for sending HTML text)
void SendHTML()
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html><head><meta charset=\"UTF-8\"><title>OpenJumper!Arduino Web Server</title><script type=\"text/javascript\">");
  client.println("function send2arduino(){var xmlhttp;if (window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();else xmlhttp=new ActiveXObject(\"Microsoft.XMLHTTP\");element=document.getElementById(\"light\");if (element.innerHTML.match(\"Turn on\")){element.innerHTML=\"Turn off\"; xmlhttp.open(\"GET\",\"?on\",true);}else{ element.innerHTML=\"Turn on\";xmlhttp.open(\"GET\",\"?off\",true); }xmlhttp.send();}");
  client.println("function getBrightness(){var xmlhttp;if (window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();else xmlhttp=new ActiveXObject(\"Microsoft.XMLHTTP\");xmlhttp.onreadystatechange=function(){if (xmlhttp.readyState==4 && xmlhttp.status==200)document.getElementById(\"brightness\").innerHTML=xmlhttp.responseText;};xmlhttp.open(\"GET\",\"?getBrightness\",true); xmlhttp.send();}window.setInterval(getBrightness,1000);</script>");
  client.println("</head><body><div align=\"center\"><h1>Arduino Web Server</h1><div>brightness:</div><div id=\"brightness\">");  
  client.println(analogRead(Sensor));
  client.println("</div><button id=\"light\" type=\"button\" onclick=\"send2arduino()\">Turn on</button><button type=\"button\" onclick=\"alert('OpenJumper Web Server')\">About</button></div></body></html>");
}
