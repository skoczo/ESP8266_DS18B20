#include "temperature.hpp"

Temperature::Temperature(DallasTemperature *sensors) {
    this->sensors = sensors;
    sensors->begin();
    init();
}

void Temperature::init()
{
    this->deviceCount = sensors->getDeviceCount();
    sensorAddresses = new uint8_t*[deviceCount];
    Serial.print("Found ");
    Serial.print(deviceCount, DEC);
    Serial.println(" devices.");

    for (auto index = 0; index < deviceCount; index++)
    {
        DeviceAddress t;
        sensors->getAddress(t, index);
        // sensorAddresses.push_back(std::move(t);
        sensorAddresses[index] = new uint8_t[8];
        std::copy(std::begin(t), std::end(t), sensorAddresses[index]);
    }

    for (auto index = 0; index < deviceCount; index++)
    {
        Serial.printf("%d: ", index);
        printAddress(sensorAddresses[index]);
        Serial.println();
    }
}

uint8_t Temperature::getDeviceCount() 
{
    return this->deviceCount;
}

float Temperature::getTemp(int index)
{
    sensors->setWaitForConversion(false);  // makes it async
    sensors->requestTemperatures();
    sensors->setWaitForConversion(true);
    return sensors->getTempCByIndex(index);
}

float Temperature::getTemp(uint8_t address)
{

}

DallasTemperature& Temperature::getSensors() 
{
    return *sensors;
}

void Temperature::printAddress(const DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}