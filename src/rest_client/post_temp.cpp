#include "post_temp.hpp"
#include <ArduinoJson.h>
#include "ESP8266WiFi.h"
// #include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "rest_settings.hpp"

bool post_temp(float value, String sensorId, String iotToken, bool retry) {
    // TODO: ignore if 85. This is wrong val

    if(WiFi.status() == WL_CONNECTED) {
      Serial.println("Sending post req");
      // HTTPClient c;
      WiFiClientSecure c;
      c.setInsecure();

      int connected = c.connect("iot.skoczo.pl", 443);
      if(connected && retry) {
        c.print("POST ");
        c.print("/iot/temperature");
        c.println(" HTTP/1.1");
        c.print("Host: ");
        c.println(MY_HOST);
        c.println("User-Agent: raleik.pl/homeiot");
        c.print("Authorization: Bearer ");
        c.println(iotToken);
        c.println("Connection: close");
        c.println("Content-Type: application/json");
        c.print("Content-Length: ");

        StaticJsonBuffer<300> JSONbuffer;
        JsonObject& JSONencoder = JSONbuffer.createObject();
        JSONencoder["value"] = value;
        JSONencoder["sensorId"] = sensorId;
        JSONencoder["deviceId"] = WiFi.macAddress();
        String output;
        JSONencoder.printTo(output);

        c.println(output.length());
        c.println("");
        c.println(output);

        Serial.println("request sent");
        String line = c.readStringUntil('\r');
        Serial.println(line);
        c.stop();

        return true;
      } else {
        Serial.println("connect failed");
        if(retry) {
          return post_temp(value, sensorId, iotToken, false);
        }
        return false;
      }
    } else {
        Serial.println("Wifi not connected");
        return false;
    }
}