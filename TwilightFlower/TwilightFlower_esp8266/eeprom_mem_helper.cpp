#include "eeprom/eeprom_mem_helper.h"

uint8_t* eeprom_read_primary_key() {
	uint8_t* key = new uint8_t[32];
	for (int i = 0; i < EEPROM_PRIMARY_KEY_SIZE; i++) __EEREAD(EEPROM_PRIMARY_KEY_ADDRESS + i, key + i);
	return key;
}

uint8_t* eeprom_read_secondary_key() {
	uint8_t* key = new uint8_t[32];
	for (int i = 0; i < EEPROM_SECONDARY_KEY_SIZE; i++) __EEREAD(EEPROM_SECONDARY_KEY_ADDRESS + i, key + i);
	return key;
}

uint32_t eeprom_read_user_key_checksum() {
	uint32_t checksum = 0;
	for (int i = 0; i < 4; i++) __EEREAD(EEPROM_USER_KEY_CHECKSUM_ADDRESS + i, ((uint8_t*)&checksum) + i);
	return checksum;
}

void eeprom_write_user_key_checksum(uint32_t checksum) {
	for (int i = 0; i < 4; i++) __EEWRITE(EEPROM_USER_KEY_CHECKSUM_ADDRESS + i, checksum >> (i * 8));
}

void eeprom_write_primary_key(uint8_t* key) {
	for (int i = 0; i < EEPROM_PRIMARY_KEY_SIZE; i++) __EEWRITE(EEPROM_PRIMARY_KEY_ADDRESS + i, key[i]);
}

void eeprom_write_secondary_key(uint8_t* key) {
	for (int i = 0; i < EEPROM_SECONDARY_KEY_SIZE; i++) __EEWRITE(EEPROM_SECONDARY_KEY_ADDRESS + i, key[i]);
}
