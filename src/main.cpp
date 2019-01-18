#include <list>
#include <algorithm>
#include <Arduino.h>

#include "FS.h"

#include "ESP8266WiFi.h"
#include "wifi_server.hpp"

#include "OneWire.h"
#include "DallasTemperature.h"

#include "EEPROM.h"

#include "temperature.hpp"


#define ONE_WIRE_BUS 5

#define WIFI_AP_NAME "ESP_WIFI"
#define WIFI_AP_PASS "password"

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Temperature *temp;

void setup()
{
  Serial.begin(9600);
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(WIFI_AP_NAME, WIFI_AP_PASS) ? "Ready" : "Failed!");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

  delay(100);

  temp = new Temperature(&sensors);
  delay(1000);
  initWebServer(temp);

  Serial.println("Setup done");
}

void loop()
{
  handleClient();
  
  // Serial.println("Before NON-blocking/async requestForConversion");
  // auto start = millis();       
  // sensors.setWaitForConversion(false);  // makes it async
  // sensors.requestTemperatures();
  // sensors.setWaitForConversion(true);
  // auto stop = millis();
  // Serial.println("After NON-blocking/async requestForConversion");
  // Serial.print("Time used: ");
  // Serial.println(stop - start); 

  // for (auto index = 0; index < temp->getDeviceCount(); index++)
  // {
  //   // get temperature
  //   Serial.print("Temperature: ");
  //   Serial.println(temp->getTemp(index));  
  //   Serial.println("\n");
  // }

  // delay(1000);
  
}



void init_fs() {
   if(!SPIFFS.begin()) {
    Serial.println("ERROR: SPIFFS.begin fail");
  }
  Serial.println("After begin");

  Serial.println("List files");

  String str = "";
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
      str += dir.fileName();
      str += " / ";
      str += dir.fileSize();
      str += "\r\n";
  }
  Serial.print(str);

  File f = SPIFFS.open("/index.html", "r");
  if (!f) {
      Serial.println("file open failed");
  } else {
    int c;
    while((c = f.read()) != -1) {
      Serial.print((char)c);
    }
    f.close();
  }
}