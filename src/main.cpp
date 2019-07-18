#include <Arduino.h>
#include <ArduinoJson.h>

#include "ESP8266WiFi.h"
#include "ESP8266WiFiType.h"
#include "wifi_server.hpp"

#include "Timer.h"

#include "temperature.hpp"
#include "eeprom/eeprom_data.hpp"
#include "rest_client/post_temp.hpp"

#define ONE_WIRE_BUS 5

#define WIFI_AP_NAME "ESP_WIFI"
#define WIFI_AP_PASS "password"

bool connectToWifi();
void checkWifi();
void postTemp();
void checkIfDevicesConected();

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Temperature *temp;
EepromData *eeprom;
Timer t;
int8_t ledTimer = -1;
String iotToken;

int LED_PIN = 2;

void setAP() {
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(WIFI_AP_NAME, WIFI_AP_PASS) ? "Ready" : "Failed!");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
}

void setup()
{
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  delay(1000);

  eeprom = new EepromData();

  // wifi data stored
  if(eeprom->getSize() != -1) {
    connectToWifi();

    if(WiFi.isConnected()) {
      ledTimer = t.oscillate(LED_BUILTIN, 2000, HIGH);
      Serial.print("Connected, IP address: ");
      Serial.println(WiFi.localIP());
    } else {
      ledTimer = t.oscillate(LED_BUILTIN, 100, HIGH);
      WiFi.disconnect();
      Serial.println("");
      Serial.println("Connecting failed. Switching to AP mode");
      setAP();
    }
    Serial.println();
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    setAP();
  }
  
  temp = new Temperature(&sensors);
  delay(1000);
  initWebServer(temp);  

  t.every(100, handleClient);
  t.every(1000, checkWifi);
  t.every(1000, checkIfDevicesConected);
  t.every(1000 * 60 * 5, postTemp);
  // t.every(1000, postTemp);

  Serial.println("Setup done");
}

void checkWifi() {
  if(WiFi.getMode() != WIFI_AP && !WiFi.isConnected()) {
    if(connectToWifi()) {
      Serial.println("wifi connected");
      t.stop(ledTimer);
      ledTimer = t.oscillate(LED_BUILTIN, 2000, HIGH);
    } else {
      Serial.println("can't connect to wifi");
      t.stop(ledTimer);
      ledTimer = t.oscillate(LED_BUILTIN, 100, HIGH);
      delay(1000);
    }
  }
}

void postTemp() {
  sensors.setWaitForConversion(false);  // makes it async
  sensors.requestTemperatures();
  sensors.setWaitForConversion(true);

  for (auto index = 0; index < temp->getDeviceCount(); index++)
  {
    // get temperature
    Serial.print("Temperature: ");
    Serial.println(temp->getTemp(index));  
    Serial.println("\n");
    if(temp->getTemp(index) == -127.0 || temp->getTemp(index) == 85.0) {
      Serial.println("Bad temp. Restart temp measuring");
      index--;
    } else {
      post_temp(temp->getTemp(index), temp->getSensorAddress(index), iotToken, true);
    }
  }
}

bool connectToWifi() {
  // wifi data stored
    if(eeprom->getSize() != -1) {
      auto ssid = eeprom->getData()[0];
      auto password = eeprom->getData()[1];
      iotToken = eeprom->getData()[2];
      WiFi.softAPdisconnect(true);
      WiFi.disconnect();
      WiFi.begin(ssid.c_str(), password.c_str());

      Serial.print("wifi connecting");
      int counter = 0;
      while (!WiFi.isConnected())
      {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
        Serial.print(".");
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);

        if(counter > 20) {
          break;
        }

        counter++;
      }
      Serial.println("");
      return WiFi.isConnected();
    }

    return false;
}

void checkIfDevicesConected()
{
  if(WiFi.getMode() == WIFI_STA) {
    if(temp->getDeviceCount() == 0 ) {
      ESP.restart();
    }
  }
}

void loop()
{
  t.update();
}
