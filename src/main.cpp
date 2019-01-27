#define DEBUG_ESP_HTTP_CLIENT 1
#define DEBUG_ESP_PORT Serial

#include <list>
#include <algorithm>
#include <Arduino.h>
#include <ArduinoJson.h>

#include "FS.h"

#include "ESP8266WiFi.h"
#include "wifi_server.hpp"

#include "OneWire.h"
#include "DallasTemperature.h"

#include "EEPROM.h"

#include "temperature.hpp"
#include "eeprom/eeprom_data.hpp"

#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#define ONE_WIRE_BUS 5

#define WIFI_AP_NAME "ESP_WIFI"
#define WIFI_AP_PASS "password"

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Temperature *temp;
EepromData *eeprom;

void setAP() {
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(WIFI_AP_NAME, WIFI_AP_PASS) ? "Ready" : "Failed!");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
}

void setup()
{
  Serial.begin(9600);

  delay(100);

  eeprom = new EepromData();

  // wifi data stored
  if(eeprom->getSize() != -1) {
    auto ssid = eeprom->getData()[0];
    auto password = eeprom->getData()[1];

    Serial.print("Wifi connection establishing. SSID: ");
    Serial.print(ssid);
    Serial.print(" password: ");
    Serial.println(password);

    WiFi.begin(ssid.c_str(), password.c_str());

    Serial.print("Connecting");
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");

      if(counter > 20) {
        break;
      }

      counter++;
    }

    if(WiFi.status() == WL_CONNECTED) {
      Serial.print("Connected, IP address: ");
      Serial.println(WiFi.localIP());
    } else {
      WiFi.disconnect();
      Serial.println("");
      Serial.println("Connecting failed. Going to AP mode");
      setAP();
    }
    Serial.println();
  } else {
    setAP();
  }
  
  temp = new Temperature(&sensors);
  delay(1000);
  initWebServer(temp);  

  Serial.println("Setup done");
}

String MY_FINGERPRINT = "97:8F:83:88:42:0F:D3:CF:C5:14:F5:01:36:5E:34:91:89:7C:3C:A4";
String MY_HOST = "iot.skoczo.pl";
uint16_t PORT = 443;

void loop()
{
  handleClient();

  // TODO: check wifi status
  
  auto start = millis();       
  sensors.setWaitForConversion(false);  // makes it async
  sensors.requestTemperatures();
  sensors.setWaitForConversion(true);
  auto stop = millis();
  Serial.print("Temp calc time: ");
  Serial.println(stop - start); 

  // cert_check();

  for (auto index = 0; index < temp->getDeviceCount(); index++)
  {
    // get temperature
    Serial.print("Temperature: ");
    Serial.println(temp->getTemp(index));  
    Serial.println("\n");

    if(WiFi.status() == WL_CONNECTED) {
      Serial.println("Sending post req");
      HTTPClient c;

      if(c.begin(MY_HOST, 443, "/temperature", MY_FINGERPRINT)) {
        c.addHeader("Content-Type", "application/json");

        StaticJsonBuffer<300> JSONbuffer;
        JsonObject& JSONencoder = JSONbuffer.createObject();
        JSONencoder["value"] = temp->getTemp(index);
        JSONencoder["sensorId"] = temp->getSensorAddress(index);
        JSONencoder["deviceId"] = WiFi.macAddress();
        String output;
        JSONencoder.printTo(output);

        int result = c.POST(output);

        if (result != HTTP_CODE_OK) {
          Serial.print("POST error code: ");
          Serial.println(result, DEC);
        }      

        c.end();
      } else {
        Serial.println("begin failed");
      }
    }
  }

  delay(1000);
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