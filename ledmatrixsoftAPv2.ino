// Use the Parola library to scroll text on the display
//
// Demonstrates the use of the scrolling function to display text received
// from the serial interface
//
// User can enter text on the serial monitor and this will display as a
// scrolling message on the display.
// Speed for the display is controlled by a pot on SPEED_IN analog in.
// Scrolling direction is controlled by a switch on DIRECTION_SET digital in.
// Invert ON/OFF is set by a switch on INVERT_SET digital in.
//
// UISwitch library can be found at https://github.com/MajicDesigns/MD_UISwitch
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WebServer.h>
#include "FS.h" 
#include <DNSServer.h>

// set to 1 if we are implementing the user interface pot, switch, etc
#define USE_UI_CONTROL 0

#if USE_UI_CONTROL
#include <MD_UISwitch.h>
#endif

// Turn on debug statements to the serial output
#define DEBUG 0

#if DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(x) Serial.print(F(x))
#define PRINTX(x) Serial.println(x, HEX)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8
#define CLK_PIN   D5 // or SCK
#define DATA_PIN  D7 // or MOSI
#define CS_PIN    D2 // or SS

// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Scrolling parameters
#if USE_UI_CONTROL
const uint8_t SPEED_IN = A5;
const uint8_t DIRECTION_SET = 8;  // change the effect
const uint8_t INVERT_SET = 9;     // change the invert

const uint8_t SPEED_DEADBAND = 5;
#endif // USE_UI_CONTROL

uint8_t scrollSpeed = 25;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 1000; // in milliseconds

// Global message buffers shared by Serial and Scrolling functions
#define	BUF_SIZE	75
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "Hello! Enter new message?" };
bool newMessageAvailable = false;


const char* ssid = "led-";
const char* password = "password1234";
const byte DNS_PORT = 53;
IPAddress ip(192, 168, 0, 1); // Part1 of the RoboRemo connection
IPAddress netmask(255, 255, 255, 0);
const int port = 1234; // Part2 of the RoboRemo connection => part 1+2 together => 192.168.0.1:1234 
DNSServer dnsServer;

//const char*  ssid = "kucingmucing";
//const char*  password = "74D4WVa2";

ESP8266WebServer server(80);

File f;

char WebPage[] =
"<!DOCTYPE html>" \
"<html>" \
"<head>" \
"<title>MajicDesigns Test Page</title>" \

"<script>" \
"strLine = \"\";" \

"function SendData()" \
"{" \
"  nocache = \"/&nocache=\" + Math.random() * 1000000;" \
"  var request = new XMLHttpRequest();" \
"  strLine = \"msg?MSG=\" + document.getElementById(\"data_form\").Message.value;" \
"  strLine = strLine + \"/&SD=\" + document.getElementById(\"data_form\").ScrollType.value;" \
"  strLine = strLine + \"/&I=\" + document.getElementById(\"data_form\").Invert.value;" \
"  strLine = strLine + \"/&SP=\" + document.getElementById(\"data_form\").Speed.value;" \
"  request.open(\"GET\", strLine + nocache, false);" \
"  request.send(null);return false;" \
"}" \
"</script>" \
"</head>" \

"<body>" \
"<p><b>MD_Parola set message</b></p>" \

"<form id=\"data_form\" name=\"frmText\">" \
"<label>Message:<br><input type=\"text\" name=\"Message\" maxlength=\"255\"></label>" \
"<br><br>" \
"<input type = \"radio\" name = \"Invert\" value = \"0\" checked> Normal" \
"<input type = \"radio\" name = \"Invert\" value = \"1\"> Inverse" \
"<br>" \
"<input type = \"radio\" name = \"ScrollType\" value = \"L\" checked> Left Scroll" \
"<input type = \"radio\" name = \"ScrollType\" value = \"R\"> Right Scroll" \
"<br><br>" \
"<label>Speed:<br>Fast<input type=\"range\" name=\"Speed\"min=\"10\" max=\"200\">Slow"\
"<br>" \
"</form>" \
"<br>" \
"<input type=\"submit\" value=\"Send Data\" onclick=\"return SendData()\">" \
"</body>" \
"</html>";




void readMsg(void)
{
  server.send(200, "text/plain", "hello from esp8266!");

  if(!server.hasArg("MSG"))
    return;

  if(server.hasArg("SP")){
    scrollSpeed = server.arg("SP").toInt();
    P.setSpeed(scrollSpeed);
  }
  
  if(server.hasArg("I")){
    bool invert = server.arg("I").toInt();
    P.setInvert(invert);
  }
  
  if(server.hasArg("SD")){
    char sd = server.arg("SD").charAt(0);
    textEffect_t scrollEffect = (sd == 'R' ? PA_SCROLL_RIGHT : PA_SCROLL_LEFT);
    P.setTextEffect(scrollEffect, scrollEffect);
    P.displayReset();
  }
  
  String msg = server.arg("MSG");
  newMessageAvailable = true;
  strcpy(newMessage,msg.c_str());
  
  f = SPIFFS.open("/f.txt", "w");
  if (!f) {
      Serial.println("file create failed");
  }
  Serial.println("====== Writing to SPIFFS file =========");
  // write 10 strings to file
  f.println(String(newMessage));
  f.println(String(scrollSpeed));
  Serial.println("file updated");
  f.close();
}



void setup()
{
  Serial.begin(57600);
  Serial.print("\n[Parola Scrolling Display]\n");


  SPIFFS.begin();
//SPIFFS.format();


  // open file for reading
  f = SPIFFS.open("/f.txt", "r");
  if (!f) {
      Serial.println("file open failed");
        // open file for writing
      f = SPIFFS.open("/f.txt", "w");
      if (!f) {
          Serial.println("file create failed");
      }
      Serial.println("====== Writing to SPIFFS file =========");
      // write 10 strings to file
      f.println(String(curMessage));
      f.println(String(scrollSpeed));
      f.close();

      
  }  Serial.println("====== Reading from SPIFFS file =======");



  while(f.available()){
    String line=f.readStringUntil('\n');
    line.trim();
    Serial.println("load setting:");
    Serial.println(line);
    strcpy(curMessage,line.c_str());
    scrollSpeed = f.readStringUntil('\n').toInt();
  }
  




  

  
//  WiFi.mode(WIFI_STA);
//  WiFi.begin(ssid, password);
 
  WiFi.softAPConfig(ip, ip, netmask); // configure ip address for softAP 
  WiFi.softAP((ssid + WiFi.macAddress()).c_str(), password);

  dnsServer.start(DNS_PORT, "*", ip);

  // Wait for connection
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }
  Serial.println("");
  Serial.print("Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/msg", readMsg);
  server.on ( "/", []() {
    server.send ( 200, "text/html", WebPage );
  } );

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  
  P.begin();
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
}

void loop()
{
  dnsServer.processNextRequest();
  server.handleClient();
  if (P.displayAnimate())
  {
    if (newMessageAvailable)
    {
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;
    }
    P.displayReset();
  }
  
}



void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
