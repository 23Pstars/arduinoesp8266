#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Servo.h>
//
//const char* ssid = "InnoSpace";
//const char* password = "74D4WVa2";

const char* ssid = "kucingmucing@unifi";
const char* password = "makhang123";

IPAddress ip(192, 168, 0, 123);       
IPAddress gateway(192,168,0,254);   
IPAddress subnet(255,255,255,0);   

Servo myservo,esc;

ESP8266WebServer server(80);
const int throttleChangeDelay = 50;
const int led = 13;

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
  
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {

  Serial.begin(115200);

  myservo.attach(D4);
  esc.attach(D3, 1000, 2000); 
  esc.write(0);
  
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);

  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/command",[]() { server.send(200, "text/html", command()); });
  
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}

String command() {
  String msg=input("cmd");

  if(msg.startsWith("servoA")){
    int val=server.arg("val").toInt();
  
    myservo.write(val);
    Serial.println(val);
    delay (1000);
    msg = "turn servo A to:"+String(val);
  }
  if(msg.startsWith("servoB")){
    int val=server.arg("val").toInt();
    msg = "turn servo B to:"+String(val);
  }
  
  if(msg.startsWith("motor")){
    int val=server.arg("val").toInt();
    msg = "turn motor to:"+String(val);

    int throttle = normalizeThrottle(val);
    
    // Print it out
    Serial.print("Setting throttle to: ");
    Serial.println(throttle);
    
    // Change throttle to the new value
    changeThrottle(throttle);
  }
  
  
  return "successfully recieve command:" + msg;
}

String input(String argName) {
  String a=server.arg(argName);
  a.replace("<","&lt;");a.replace(">","&gt;");
  a.substring(0,200); return a; 
}


void changeThrottle(int throttle) {
  
  // Read the current throttle value
  int currentThrottle = readThrottle();
  
  // Are we going up or down?
  int step = 1;
  if( throttle < currentThrottle )
    step = -1;
  
  // Slowly move to the new throttle value 
  while( currentThrottle != throttle ) {
    esc.write(currentThrottle + step);
    currentThrottle = readThrottle();
    delay(throttleChangeDelay);
  }
  
}

int readThrottle() {
  int throttle = esc.read();
  
  Serial.print("Current throttle is: ");
  Serial.println(throttle);
  
  return throttle;
}

// Ensure the throttle value is between 0 - 180
int normalizeThrottle(int value) {
  if( value < 45 )
    return 45;
  if( value > 140 )
    return 140;

  return value;
}
