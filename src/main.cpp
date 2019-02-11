#include <Arduino.h>
#include <ArduinoJson.h>

#include "ESP8266WiFi.h"
#include "wifi_server.hpp"

#include "temperature.hpp"
#include "eeprom/eeprom_data.hpp"
#include "rest_client/post_temp.hpp"

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
    Serial.println(ssid);

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
      Serial.println("Connecting failed. Switching to AP mode");
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

    post_temp(temp->getTemp(index), temp->getSensorAddress(index));
  }

  delay(1000);
}