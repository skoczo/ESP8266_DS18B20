#include "post_temp.hpp"
#include <ArduinoJson.h>
#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>

String MY_FINGERPRINT = "97:8F:83:88:42:0F:D3:CF:C5:14:F5:01:36:5E:34:91:89:7C:3C:A4";
String MY_HOST = "iot.skoczo.pl";
uint16_t PORT = 443;

bool post_temp(float value, String sensorId) {
    if(WiFi.status() == WL_CONNECTED) {
      Serial.println("Sending post req");
      HTTPClient c;

      if(c.begin(MY_HOST, 443, "/temperature", MY_FINGERPRINT)) {
        c.addHeader("Content-Type", "application/json");

        StaticJsonBuffer<300> JSONbuffer;
        JsonObject& JSONencoder = JSONbuffer.createObject();
        JSONencoder["value"] = value;
        JSONencoder["sensorId"] = sensorId;
        JSONencoder["deviceId"] = WiFi.macAddress();
        String output;
        JSONencoder.printTo(output);

        int result = c.POST(output);

        if (result != HTTP_CODE_OK) {
          Serial.print("POST error code: ");
          Serial.println(result, DEC);
        }      

        c.end();

        return true;
      } else {
        Serial.println("begin failed");
        return false;
      }
    } else {
        Serial.println("Wifi not connected");
        return false;
    }
}