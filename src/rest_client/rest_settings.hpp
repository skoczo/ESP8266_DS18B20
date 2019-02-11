#ifndef REST_SETTINGS_SENT
#define REST_SETTINGS_SENT

#include <Arduino.h>

#define TLS true

#ifdef TLS
    String MY_FINGERPRINT = "97:8F:83:88:42:0F:D3:CF:C5:14:F5:01:36:5E:34:91:89:7C:3C:A4";
    String MY_HOST = "iot.skoczo.pl";
    uint16_t PORT = 443;
#else
    String MY_HOST = "swoj";
    uint16_t PORT = 8443;
#endif

#endif