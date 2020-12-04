// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include <ESP8266WiFi.h>
RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int pinD = D7;

void setup () {
  Serial.begin(57600);

  pinMode(0, INPUT_PULLUP);
  pinMode(pinD,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(pinD,HIGH);
  digitalWrite(LED_BUILTIN,HIGH);
  WiFi.forceSleepBegin(); 
  Serial.println("forceSleepBegin");
    

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if ( !rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
//     rtc.adjust(DateTime(2020, 12, 4, 19, 19, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}

void loop () {

    DateTime now = rtc.now();



    // jalan pkl 7 malam dan 7 pagi
    if((now.hour()==7||now.hour()==19) && now.minute()==0 && now.second()>=0 && now.second()<=20) 
    {
      Serial.println("Running alarm");
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);
      
    
      //turn on relay
      digitalWrite(pinD, LOW);
      //wait 20 second
      delay(20000);
      //turn off relay
      digitalWrite(pinD, HIGH);
      //set alarm done for today
      
    }
    
    //butang test, tekan flash button untuk test relay
    if(digitalRead(0)==LOW)
    {
      Serial.println("Running alarm");
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);
      
    
      //turn on relay
      digitalWrite(pinD, LOW);
      //wait 20 second
      delay(2000);
      //turn off relay
      digitalWrite(pinD, HIGH);
    }
    
    
    ////////////display date saja/////////////
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");

    // calculate a date which is 7 days, 12 hours, 30 minutes, and 6 seconds into the future
    DateTime future (now + TimeSpan(7,12,30,6));

    Serial.print(" now + 7d + 12h + 30m + 6s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();
    Serial.println();
    ////////////display date saja/////////////
    
    
    delay(2000);
}
