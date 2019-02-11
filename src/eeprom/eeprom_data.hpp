#ifndef EEPROM_DATA
#define EEPROM_DATA

#include <Arduino.h>

class EepromData {
    public:
        EepromData();
        void write(int size, String data[]);        

        int getSize();
        String* getData();

    private:
        String *data;
        int size = -1;

        String readString(int index);
        void writeString(int address, const char* data);
};

#endif

