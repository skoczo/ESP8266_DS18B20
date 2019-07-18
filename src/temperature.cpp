#include "temperature.hpp"

Temperature::Temperature(DallasTemperature *sensors) {
    this->sensors = sensors;
    sensors->begin();
    init();
}

void Temperature::init()
{
    uint8_t deviceCount = sensors->getDeviceCount();
    sensorAddresses = new String[deviceCount];
    Serial.print("Found ");
    Serial.print(deviceCount, DEC);
    Serial.println(" devices.");

    for (auto index = 0; index < deviceCount; index++)
    {
        DeviceAddress t;
        sensors->getAddress(t, index);
        sensorAddresses[index] = printAddress(t);
    }

    for (auto index = 0; index < deviceCount; index++)
    {
        Serial.printf("%d: ", index);
        Serial.println(sensorAddresses[index]);
        Serial.println();
    }
}

uint8_t Temperature::getDeviceCount() 
{
    return sensors->getDeviceCount();
}

String Temperature::getSensorAddress(int index) {
    return sensorAddresses[index];
}

float Temperature::getTemp(int index)
{
    return sensors->getTempCByIndex(index);
}

DallasTemperature& Temperature::getSensors() 
{
    return *sensors;
}

String Temperature::printAddress(const DeviceAddress deviceAddress)
{
    String address_hex;
    for (uint8_t i = 0; i < 8; i++)
    {
        // zero pad the address if necessary
        if (deviceAddress[i] < 16) address_hex+='0';

        address_hex+=String(deviceAddress[i],HEX);
    }

    return address_hex;
}