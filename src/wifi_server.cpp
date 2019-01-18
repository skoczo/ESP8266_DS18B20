#include <stdio.h>
#include "wifi_server.hpp"
#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "eeprom/eeprom_data.hpp"
// #include "ESP8266WebServerSecure.h"

// The certificate is stored in PMEM
static const uint8_t x509[] PROGMEM = {
  0x30, 0x82, 0x01, 0x51, 0x30, 0x81, 0xfc, 0x02, 0x09, 0x00, 0xa6, 0x59,
  0xaa, 0xc1, 0x62, 0x1a, 0xd9, 0xbc, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86,
  0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x30, 0x31,
  0x17, 0x30, 0x15, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x0e, 0x79, 0x6f,
  0x75, 0x72, 0x2d, 0x6e, 0x61, 0x6d, 0x65, 0x2d, 0x68, 0x65, 0x72, 0x65,
  0x31, 0x15, 0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x0c, 0x65,
  0x73, 0x70, 0x38, 0x32, 0x36, 0x36, 0x2d, 0x64, 0x65, 0x6d, 0x6f, 0x30,
  0x1e, 0x17, 0x0d, 0x31, 0x38, 0x30, 0x33, 0x31, 0x39, 0x31, 0x36, 0x35,
  0x33, 0x30, 0x30, 0x5a, 0x17, 0x0d, 0x33, 0x31, 0x31, 0x31, 0x32, 0x36,
  0x31, 0x36, 0x35, 0x33, 0x30, 0x30, 0x5a, 0x30, 0x30, 0x31, 0x17, 0x30,
  0x15, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x0e, 0x79, 0x6f, 0x75, 0x72,
  0x2d, 0x6e, 0x61, 0x6d, 0x65, 0x2d, 0x68, 0x65, 0x72, 0x65, 0x31, 0x15,
  0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x0c, 0x65, 0x73, 0x70,
  0x38, 0x32, 0x36, 0x36, 0x2d, 0x64, 0x65, 0x6d, 0x6f, 0x30, 0x5c, 0x30,
  0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01,
  0x05, 0x00, 0x03, 0x4b, 0x00, 0x30, 0x48, 0x02, 0x41, 0x00, 0xc5, 0x5f,
  0x7a, 0xa4, 0xa5, 0x94, 0x45, 0xde, 0x1b, 0x7e, 0x28, 0x07, 0x4f, 0x76,
  0x60, 0xb5, 0xe5, 0xa8, 0xba, 0x98, 0xb7, 0xb8, 0x41, 0x69, 0x44, 0x14,
  0xdf, 0x5b, 0x92, 0xdd, 0x84, 0xac, 0x3d, 0x20, 0x82, 0x51, 0x19, 0x9a,
  0x68, 0xed, 0xca, 0xee, 0x45, 0x95, 0xe5, 0x15, 0xbb, 0x8f, 0xc2, 0xa1,
  0x02, 0xb9, 0x85, 0xb0, 0xfe, 0xfd, 0x9c, 0x55, 0x88, 0x7d, 0x02, 0x50,
  0x8d, 0xdf, 0x02, 0x03, 0x01, 0x00, 0x01, 0x30, 0x0d, 0x06, 0x09, 0x2a,
  0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x03, 0x41,
  0x00, 0x03, 0x04, 0x71, 0x08, 0x16, 0x9e, 0x6e, 0x84, 0x9f, 0x1f, 0xed,
  0x8d, 0xa2, 0x94, 0xae, 0x11, 0xa6, 0x64, 0x9d, 0x4c, 0xb1, 0x8a, 0xc5,
  0x17, 0x9c, 0xa9, 0xae, 0x30, 0xa3, 0xa1, 0xb8, 0xa6, 0x49, 0xda, 0xab,
  0xa3, 0x11, 0x0b, 0xbd, 0x2d, 0xfb, 0xd9, 0x34, 0x75, 0x50, 0xf8, 0xa9,
  0x53, 0x9f, 0x12, 0x3d, 0xf7, 0x90, 0x9d, 0xea, 0x7b, 0x96, 0x16, 0x59,
  0x5b, 0xc5, 0x6e, 0xdf, 0x19
};

// And so is the key.  These could also be in DRAM
static const uint8_t rsakey[] PROGMEM = {
  0x30, 0x82, 0x01, 0x3b, 0x02, 0x01, 0x00, 0x02, 0x41, 0x00, 0xc5, 0x5f,
  0x7a, 0xa4, 0xa5, 0x94, 0x45, 0xde, 0x1b, 0x7e, 0x28, 0x07, 0x4f, 0x76,
  0x60, 0xb5, 0xe5, 0xa8, 0xba, 0x98, 0xb7, 0xb8, 0x41, 0x69, 0x44, 0x14,
  0xdf, 0x5b, 0x92, 0xdd, 0x84, 0xac, 0x3d, 0x20, 0x82, 0x51, 0x19, 0x9a,
  0x68, 0xed, 0xca, 0xee, 0x45, 0x95, 0xe5, 0x15, 0xbb, 0x8f, 0xc2, 0xa1,
  0x02, 0xb9, 0x85, 0xb0, 0xfe, 0xfd, 0x9c, 0x55, 0x88, 0x7d, 0x02, 0x50,
  0x8d, 0xdf, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x40, 0x5c, 0x9d, 0x10,
  0xcf, 0x71, 0x62, 0xc1, 0xe6, 0x16, 0xc0, 0x7b, 0xc7, 0xf2, 0x61, 0x79,
  0xbf, 0xe0, 0xa3, 0xeb, 0xea, 0xfd, 0x4a, 0x58, 0x67, 0x2c, 0xab, 0x1e,
  0xa3, 0xb9, 0xa6, 0x78, 0x3a, 0x06, 0x61, 0xf8, 0xef, 0x2f, 0x97, 0xd5,
  0x3e, 0x24, 0x86, 0x55, 0xcb, 0x5e, 0x21, 0x8d, 0xa3, 0xd6, 0x87, 0xfd,
  0x53, 0xa7, 0xd3, 0xca, 0xe1, 0xba, 0x9d, 0x8c, 0x30, 0x00, 0xf7, 0x25,
  0x01, 0x02, 0x21, 0x00, 0xf2, 0xb0, 0xc0, 0x7a, 0xbe, 0xdc, 0x8b, 0xf0,
  0x2b, 0x7c, 0xfc, 0x1d, 0x1c, 0xd5, 0x9e, 0x3b, 0x0c, 0xda, 0xfd, 0x53,
  0x31, 0x73, 0x89, 0x4f, 0x33, 0x85, 0xc1, 0x52, 0x2b, 0xea, 0x7f, 0xe3,
  0x02, 0x21, 0x00, 0xd0, 0x32, 0x7e, 0x4c, 0x4b, 0x3b, 0x8f, 0x05, 0xaa,
  0x97, 0xf5, 0xf1, 0xd2, 0xcc, 0x67, 0x57, 0x9b, 0x09, 0x40, 0x93, 0x0a,
  0x1f, 0x33, 0xb4, 0x39, 0x66, 0x7f, 0x65, 0x55, 0x68, 0x22, 0xd5, 0x02,
  0x21, 0x00, 0x8e, 0x17, 0x84, 0xaa, 0x99, 0x43, 0x01, 0xbf, 0xdd, 0x86,
  0x71, 0x0a, 0x0e, 0x8e, 0xd7, 0xf4, 0xd4, 0xe3, 0x06, 0xbd, 0x05, 0xd0,
  0x12, 0x8d, 0xc7, 0xa9, 0xc1, 0x75, 0x7d, 0xf6, 0xef, 0x67, 0x02, 0x21,
  0x00, 0xc7, 0x04, 0xfd, 0xa6, 0x80, 0xe4, 0x56, 0x3b, 0xdc, 0x6f, 0x97,
  0x33, 0xab, 0x86, 0xa9, 0xe3, 0x1c, 0xd9, 0x23, 0x59, 0x6b, 0xfb, 0x97,
  0xb9, 0x58, 0x85, 0x9a, 0x92, 0x8b, 0xaa, 0x18, 0x61, 0x02, 0x20, 0x79,
  0xfc, 0x79, 0x86, 0xd1, 0x2f, 0x3a, 0x18, 0x56, 0xb7, 0x16, 0x8a, 0x57,
  0x2f, 0x59, 0x9a, 0x3e, 0x5f, 0x6c, 0x1c, 0x0d, 0x14, 0x28, 0xf7, 0xa6,
  0x84, 0xea, 0x18, 0xee, 0x0d, 0x64, 0x63
};

ESP8266WebServer webServerInstance;
Temperature *tempInstance;
extern EepromData *eeprom;

String wifiConfigPage = "<html>"
"<body>"
"<form action=\"/set_wifi\" method=\"post\">"
"SSID: <input type=\"text\" name=\"ssid\"><br>"
"Password: <input type=\"password\" name=\"pass\"><br>"
"<input type=\"submit\">"
"</form>"
"</body>"
"</html>";

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
  // webServerInstance.setServerKeyAndCert_P(rsakey, sizeof(rsakey), x509, sizeof(x509));

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
