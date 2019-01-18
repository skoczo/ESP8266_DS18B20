#ifndef WIFI_SERVER_SKOCZO
#define WIFI_SERVER_SKOCZO

#include <Arduino.h>
#include "temperature.hpp"

void initWebServer(Temperature *t);
void handleClient();

#endif