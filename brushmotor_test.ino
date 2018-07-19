/* Create a WiFi access point and provide a web server on it to receive command to control motor. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

/* Set these to your desired credentials. */
const char *ssid = "my-rc";
const char *password = "makhang123";

IPAddress ip(192, 168, 0, 1); // Part1 of the RoboRemo connection
IPAddress netmask(255, 255, 255, 0);
const int port = 1234; // Part2 of the RoboRemo connection => part 1+2 together => 192.168.0.1:1234 

ESP8266WebServer server(port);

int aa=D5,bb=D6,cc=D7,dd=D8;

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
  server.send(200, "text/plain", "hello from Robot!");
}

void motor_forward(){
    digitalWrite(aa, 1);
    digitalWrite(bb, 0);
    digitalWrite(cc, 0);
    digitalWrite(dd, 0);
  }
void motor_stop(){
    digitalWrite(aa, 0);
    digitalWrite(bb, 0);
  }
void motor_halt(){
    digitalWrite(aa, 0);
    digitalWrite(bb, 0);
    digitalWrite(cc, 0);
    digitalWrite(dd, 0);
  }
void motor_align(){
    digitalWrite(cc, 0);
    digitalWrite(dd, 0);
  }
void motor_back(){
    digitalWrite(aa, 0);
    digitalWrite(bb, 1);
    digitalWrite(cc, 0);
    digitalWrite(dd, 0);
  }

void motor_left(){
    digitalWrite(aa, 0);
    digitalWrite(bb, 0);
    digitalWrite(cc, 1);
    digitalWrite(dd, 0);
  }
void motor_right(){
    digitalWrite(aa, 0);
    digitalWrite(bb, 0);
    digitalWrite(cc, 0);
    digitalWrite(dd, 1);
  }


void setup() {
  // prepare Motor Output Pins
  pinMode(aa, OUTPUT);
  digitalWrite(aa, 0);
  
  // prepare GPIO5 relay 1
  pinMode(bb, OUTPUT);
  digitalWrite(bb, 0);
  
  pinMode(cc, OUTPUT);
  digitalWrite(cc, 0);
  
  pinMode(dd, OUTPUT);
  digitalWrite(dd, 0);

  
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

 
  server.on("/", handleRoot);
 
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/fw", []() {
    motor_forward();
    server.send(200, "text/plain", "Forward");
  });
  server.on("/bk", []() {
    motor_back();
    server.send(200, "text/plain", "Back");
  });

  server.on("/st", []() {
    motor_stop();
    server.send(200, "text/plain", "Stop");
  });
  
  server.on("/ht", []() {
    motor_halt();
    server.send(200, "text/plain", "Halt");
  });
  server.on("/lt", []() {
    motor_left();
    server.send(200, "text/plain", "Left");
  });
  server.on("/rt", []() {
    motor_right();
    server.send(200, "text/plain", "Right");
  });

  server.on("/al", []() {
    motor_align();
    server.send(200, "text/plain", "Align");
  });
  server.begin();
  Serial.println("HTTP server started");

  motor_stop();
  
}

void loop() {
  server.handleClient();
 //
}


