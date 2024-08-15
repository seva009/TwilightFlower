#include "commands.h"
#include "packets.h"
#include "types.h"
#include "pkt_io_hdl.h"
#include "esp_pkt_hdl.h"
#include "commands.h"
#include "eeprom/eeprom_mem_helper.h"
#include <string.h>
#include <malloc.h>
#include <vector>

using namespace std;

esp_pkt_hdl::esp_pkt_hdl() {
  return;
	stage = STAGE_NONE;
#ifdef ESP8266
	Serial.begin(256000);
#endif
	memset(user_key, 0, sizeof(user_key));
	memset(primary_key, 0, sizeof(primary_key));
	memset(secondary_key, 0, sizeof(secondary_key));
	memset(shadow_key, 0, sizeof(shadow_key));

	for (uint8_t i = 0; i < sizeof(shadow_key); i++) shadow_key[i] = random() % (uint8_t)-1;

	user_key_checksum = eeprom_read_user_key_checksum();
	uint8_t* readed_secondary_key = eeprom_read_secondary_key();

	if (readed_secondary_key != nullptr) {
		memcpy(secondary_key, readed_secondary_key, sizeof(secondary_key));
	}
	delete readed_secondary_key;

	for (uint8_t i = 0; i < sizeof(secondary_key); i++) secondary_key[i] ^= shadow_key[i];

	eeprom_write_secondary_key(secondary_key);
	return;
}

esp_pkt_hdl::~esp_pkt_hdl() {
	for (uint8_t i = 0; i < sizeof(secondary_key); i++) {
		secondary_key[i] ^= shadow_key[i];
		secondary_key[i] ^= user_key[i];
	}
	eeprom_write_secondary_key(secondary_key);

	memset(user_key, 0, sizeof(user_key));
	memset(primary_key, 0, sizeof(primary_key));
	memset(secondary_key, 0, sizeof(secondary_key));
	memset(shadow_key, 0, sizeof(shadow_key));

	delete user_key;
	delete primary_key;
	delete secondary_key;
	delete shadow_key;

	user_key_checksum = 0;
	user_key_checksum_received = 0;

	enc_data.~linked_list();
	return;
}

void esp_pkt_hdl::handle(uint8_t* thread, size_t thread_sz) {
	vector<vector<size_t>> packets = thread_pkt_split(thread, thread_sz, REV_TRS_SIG);
	//packets::ut_pkt pkt_buf;
	size_t pkt_seq_num = 0;
	size_t pkt_sz = 0;

  for (int i = 0; i < packets.size(); i++) {
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
  }

  packets::hello pkt_hello;
  packets::cts cts;

	while (pkt_seq_num < packets.size()) {
		pkt_sz = packets[pkt_seq_num][1] - packets[pkt_seq_num][0] + 4;
		uint8_t* raw_pkt_buf = (uint8_t*)calloc(pkt_sz, 1);

		if (!raw_pkt_buf) {
			return;
		}

		memcpy(raw_pkt_buf, (uint8_t*)&thread[packets[pkt_seq_num][0]], pkt_sz);

		packets::ut_pkt* pkt = pkt_parser(raw_pkt_buf, pkt_sz);

		switch (pkt->type)
		{
		case HELLO:
			if (pkt->start_sig != TRS_SIG || pkt->proto_ver != PROTO_VER || pkt->ec_ver != EC_VER) {
				free(raw_pkt_buf);
				free(pkt);
				//halt_brk();

				break;
			}
			for (int i = 0; i < sizeof(packets::hello); i++) port_write(*(uint8_t*)(&pkt_hello + i));
			break;

		case REQ_KEY:  //lol how?
			break;

		case RESP_KEY:
			if (pkt->start_sig != TRS_SIG || pkt->proto_ver != PROTO_VER || pkt->ec_ver != EC_VER) {
				//halt_brk();

				break;
			}
			user_key_checksum_received = pkt->key_checksum;
			if (user_key_checksum_received == user_key_checksum) {
				memcpy(user_key, pkt->key, sizeof(user_key));

				for (int i = 0; i < sizeof(secondary_key); i++) {
					secondary_key[i] ^= shadow_key[i];
					secondary_key[i] ^= user_key[i];
					primary_key[i]   ^= secondary_key[i];
				}

				stage = STAGE_AUTH;

				break;
			}
			break;
		case CTS:
			if (pkt->start_sig != TRS_SIG || pkt->proto_ver != PROTO_VER || pkt->ec_ver != EC_VER || pkt->end_sig != TRS_SIG) break;
			delay(pkt->cts_timeout);
			break;
		case RTS:
			if (pkt->start_sig != TRS_SIG || pkt->proto_ver != PROTO_VER || pkt->ec_ver != EC_VER || pkt->end_sig != TRS_SIG) break;
			cts.cts_timeout = pkt->rts_timeout;
			digitalWrite(LED_BUILTIN, HIGH);
			for (int i = 0; i < sizeof(packets::cts); i++) port_write(*(uint8_t*)(&cts + i));
			break;

		}

	}
  digitalWrite(LED_BUILTIN, LOW);
}

void halt_brk() {
	packets::command pkt;
	pkt.type = COMMAND;
	pkt.start_sig = TRS_SIG;
	pkt.proto_ver = PROTO_VER;
	pkt.command = (uint8_t)commands::HALT;
	pkt.end_sig = TRS_SIG;

	for (int i = 0; i < sizeof(packets::command); i++) port_write(*(uint8_t*)(&pkt + i));
}