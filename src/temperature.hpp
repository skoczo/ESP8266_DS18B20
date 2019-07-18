#ifndef TEMPERATURE_HPP
#define TEMPERATURE_HPP

#include <Arduino.h>
#include "DallasTemperature.h"

class Temperature {
    public:
        Temperature(DallasTemperature *sensors);
        float getTemp(int index);
        uint8_t getDeviceCount();
        DallasTemperature& getSensors();
        String printAddress(const DeviceAddress deviceAddress);
        String getSensorAddress(int index) ;

    private:
        DallasTemperature *sensors;
        String* sensorAddresses; 
        void init();

};

#endif