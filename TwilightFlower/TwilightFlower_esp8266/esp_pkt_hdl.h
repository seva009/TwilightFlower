#pragma once 

#include "linked_list.h"

#ifdef ESP8266
#include <Arduino.h>
#define delay(ms) delay(ms)
#define port_write(x) Serial.write(x)
#define port_read(__buffer__, __length__) Serial.read(__buffer__, __length__)
#define random() os_random()
#define is_available() Serial.available()
#else //chtobi VS ne rugalas'
#define port_write(x) ;
#define port_read(__buffer__, __length__) ;
#define delay(ms) ;
#define random() 0
#define is_available() true
#endif

#define STAGE_NONE 0
#define STAGE_AUTH 1

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

class esp_pkt_hdl
{
public:
	esp_pkt_hdl();
	~esp_pkt_hdl();
	void handle(uint8_t* thread, size_t thread_sz);

private:
	uint8_t stage;

	uint8_t user_key[32];
	uint8_t primary_key[32];
	uint8_t secondary_key[32];
	uint8_t shadow_key[32];

	uint32_t user_key_checksum_received;
	uint32_t user_key_checksum;

	linked_list<uint8_t> enc_data;

};