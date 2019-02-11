#include <stdio.h>
#include "wifi_server.hpp"
#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "eeprom/eeprom_data.hpp"

ESP8266WebServer webServerInstance;
Temperature *tempInstance;
extern EepromData *eeprom;

String wifiConfigPage = "<html>"
"<body>"
"<form action=\"/set_wifi\" method=\"post\">"
"SSID: <input type=\"text\" name=\"ssid\"><br>"
"Password: <input type=\"password\" name=\"pass\"><br>"
"<input type=\"submit\" value=\"save\">"
"</form></body></html>";

void handleTemp() {
  Serial.println("uri: " + webServerInstance.uri());

  auto position = webServerInstance.uri().lastIndexOf('/');
  String sensorIndex = webServerInstance.uri().substring(position+1);
  Serial.println("sensor index: " + sensorIndex);

  if(sensorIndex.length() > 0) {
    auto temp = tempInstance->getTemp((int)sensorIndex.toInt());
    webServerInstance.send(200, "text/plain", String(temp, DEC));
  } else {
    webServerInstance.send(400, "text/plain", "Wrong index");
  }
}

void setWifi() {
  Serial.println("setWifi uri: " + webServerInstance.uri());

  auto ssid = webServerInstance.arg("ssid");
  auto pass = webServerInstance.arg("pass");

  Serial.println("ssid: " + ssid);
  Serial.println("pass: " + pass);

  String array[] = {"3", ssid, pass};
  eeprom->write(3, array);

  if(ssid.length() == 0 || pass.length() == 0) {
    webServerInstance.send(400, "text/plain", "SSID or password empty");
  } else {
    webServerInstance.send(200, "text/plain", "Data saved. Rebooting....");
    delay(100);
    ESP.restart();
  }
}

void initWebServer(Temperature *t) {
  tempInstance = t;
  Serial.println("initWebServer");

  int count = (int)tempInstance->getDeviceCount();
  Serial.print("Count: ");
  Serial.println(count, DEC);

  webServerInstance.on("/", []() {
    Serial.println("index handle");
    webServerInstance.send(200, "text/html", wifiConfigPage);
  });

  for(int i=0; i < count; i++) {
    String url = "/temp/" + String(i, DEC);
    Serial.println("Initializing url: " + url);
    webServerInstance.on(url, handleTemp);
  }

  webServerInstance.on("/set_wifi", HTTP_POST, setWifi);

  webServerInstance.begin();
}

void handleClient() {
  webServerInstance.handleClient();
}
