#include "eeprom/eeprom_data.hpp"
#include "EEPROM.h"

EepromData::EepromData() {
    EEPROM.begin(200);

    String num = "";
    uint8_t c;
    int index;

    Serial.print("READING\n'");
    
    for(index=0; (c = EEPROM.read(index)) != '\0'; index++) {
        Serial.print((char)c);
        num+= (char)c;
    }

    Serial.println("'\nEND READING\n");
    Serial.println("Readed num: '" + num + "'.");

    data = new String[3];
    if(num.equals("4")) {
        index++;
        String ssid = readString(index);
        index = index + ssid.length() +1;

        String pass = readString(index);
        index = index + pass.length()+1;

        String iotToken = readString(index);
        index = index + iotToken.length()+1;

        Serial.println("Data in EEPROM");
        Serial.println("SSID: " + ssid);
        Serial.print("Password: ");
        Serial.println(pass != NULL && pass.length()>0 ? true : false);
        Serial.print("iotToken: ");
        Serial.println(iotToken != NULL && iotToken.length()>0 ? true : false);

        size = 3;
        data[0] = ssid;
        data[1] = pass;
        data[2] = iotToken;
    } else {
        size = -1;
        Serial.println("No data in EEPROM");
    }
}

bool EepromData::write(int size, String data[]) {
    int address = 0;
    for(int i=0; i<size; i++) {
        if(writeString(address, data[i].c_str())) {
            address+= data[i].length() + 1;
        } else {
            clear();
            return false;
        }
    }

    return true;
}

bool EepromData::writeString(int address, const char* data) {
    int numBytes = strlen(data) + 1;
    Serial.print("Writing string:");
    Serial.println(data);
    Serial.print("numBytes:");
    Serial.println(numBytes);

    for (int i = 0; i < numBytes; i++) {
        EEPROM.write(address + i, data[i]);
        if(!EEPROM.commit()) {
            Serial.println("Commit fail");
        }
    }

    return true;
}


boolean eeprom_write_bytes(int startAddr, const byte* array, int numBytes) {
    int i;
    for (i = 0; i < numBytes; i++) {
        EEPROM.write(startAddr + i, array[i]);
    }
    return true;
}


String EepromData::readString(int index) {
    int i;
    uint8_t c;
    String text;
    for(i=0; (c = EEPROM.read(index + i)) != '\0'; i++) {
        text+=(char)c;
    }

    return text;
}

void EepromData::clear() {
    Serial.println("Clearing eeprom data");
    for (unsigned int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
    }
}

int EepromData::getSize() {
    return size;
}

String* EepromData::getData() {
    return data;
}