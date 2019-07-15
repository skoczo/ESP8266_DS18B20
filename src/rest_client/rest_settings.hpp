#ifndef REST_SETTINGS_SENT
#define REST_SETTINGS_SENT

#include <Arduino.h>

#define DEBUG_HTTPCLIENT

#define TLS true

String iotToken = "f4246aa4-76ce-466d-81f3-1f8f6877c35f";

#ifdef TLS
    String MY_FINGERPRINT = "A5:3C:27:EA:02:6F:DD:9D:85:20:F2:BF:6D:BC:68:EB:02:82:60:44";
    String MY_HOST = "iot.skoczo.pl";
    uint16_t PORT = 443;
#else
    String MY_HOST = "https://iot.skoczo.pl";
    uint16_t PORT = 443;
#endif

#endif