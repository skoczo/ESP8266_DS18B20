#include "post_temp.hpp"
#include <ArduinoJson.h>
#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>
#include "rest_settings.hpp"

bool post_temp(float value, String sensorId) {
    if(WiFi.status() == WL_CONNECTED) {
      Serial.println("Sending post req");
      HTTPClient c;

      #ifdef TLS
        if(c.begin(MY_HOST, PORT, "/temperature", MY_FINGERPRINT)) {
      #else
        if(c.begin(MY_HOST, PORT, "/temperature")) {
      #endif
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