#pragma once 

#ifdef ESP8266
#include <EEPROM.h>
#endif

#define EEPROM_PRIMARY_KEY_ADDRESS 1
#define EEPROM_PRIMARY_KEY_SIZE 32
#define EEPROM_SECONDARY_KEY_ADDRESS 33
#define EEPROM_SECONDARY_KEY_SIZE 32
#define EEPROM_USER_KEY_CHECKSUM_ADDRESS 65
#define EEPROM_USER_KEY_CHECKSUM_SIZE 4

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

void eeprom_write_primary_key(uint8_t* key);
void eeprom_write_secondary_key(uint8_t* key);
void eeprom_write_user_key_checksum(uint32_t checksum);
uint8_t* eeprom_read_primary_key();
uint8_t* eeprom_read_secondary_key();
uint32_t eeprom_read_user_key_checksum();

#ifdef ESP8266
#define __EEWRITE(addr, val) EEPROM.write(addr, val);EEPROM.commit()
#define __EEREAD(addr, val) *(val) = EEPROM.read(addr)
#else
#define __EEWRITE(addr, val) ;
#define __EEREAD(addr, val) ;
#endif
