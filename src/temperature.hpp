#ifndef TEMPERATURE_HPP
#define TEMPERATURE_HPP

#include <Arduino.h>
#include "DallasTemperature.h"

class Temperature {
    public:
        Temperature(DallasTemperature *sensors);
        float getTemp(int index);
        float getTemp(uint8_t address);
        uint8_t getDeviceCount();
        DallasTemperature& getSensors();
        void printAddress(const DeviceAddress deviceAddress);

    private:
        DallasTemperature *sensors;
        uint8_t** sensorAddresses; 
        uint8_t deviceCount;
        void init();

};

#endif