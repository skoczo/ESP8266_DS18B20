#ifndef EEPROM_DATA
#define EEPROM_DATA

#include <Arduino.h>

class EepromData {
    public:
        EepromData();
        bool write(int size, String data[]);

        int getSize();
        String* getData();
        void clear();

    private:
        String *data;
        int size = -1;

        String readString(int index);
        bool writeString(int address, const char* data);
};

#endif

