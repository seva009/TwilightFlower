#include "packets.h"
#include "types.h"
#include "pkt_io_hdl.h"
#include <string.h>
#include <malloc.h>
#include <vector>
#include "eeprom/eeprom_mem_helper.h"
#ifdef ESP8266
#include <Arduino.h>
#endif


/**
 * Splits a thread into packets based on a given record signature.
 *
 * @param thread Pointer to the thread to be split.
 * @param thread_sz Size of the thread.
 * @param rec_sig Signature used to split the thread.
 *
 * @return A vector of vectors containing the start and end points of each packet.
 *
 * @throws None.
 */
std::vector<std::vector<size_t>> thread_pkt_split(void* thread, size_t thread_sz, uint32_t rec_sig) {
	std::vector<std::vector<size_t>> pool;
	const char* sig = reinterpret_cast<const char*>(&rec_sig);
	char* str = static_cast<char*>(calloc(4, sizeof(char)));

	std::vector<size_t> pkt_ps = { 0, 0 }; // packet points
	bool isStartPoint = true;
	size_t thread_ptr = 0;

	// if (!str || !sig || !thread) {
	// 	free(str);
	// 	return pool;
	// }

	while (thread_sz - thread_ptr > 0) {
		char* memcpy_result = reinterpret_cast<char*>(memcpy(str, reinterpret_cast<char*>(thread) + thread_ptr, 4));
		if (!memcpy_result) {
			free(str);
			return pool;
		}

		int memcmp_result = memcmp(sig, str, 4);
		if (memcmp_result == 0) {
			pkt_ps[(int)isStartPoint] = thread_ptr; // somnitel'no no okey
			if (!isStartPoint) {
				pool.push_back(pkt_ps);
				pkt_ps = { 0, 0 };
			}
			isStartPoint = !isStartPoint;
		}

		thread_ptr++;
	}

	free(str);
	return pool;
}

packets::ut_pkt* pkt_parser(uint8_t* pkt, size_t pkt_sz) {

	packets::hello* h = nullptr;
	packets::req* r = nullptr;
	packets::resp_key* rk = nullptr;
	packets::resp_head* rh = nullptr;
	packets::resp_data* rd = nullptr;
	packets::pkt_req* pr = nullptr;
	packets::command* cmd = nullptr;
	packets::rts* rts = nullptr;
	packets::cts* cts = nullptr;

	packets::ut_pkt* ut = (packets::ut_pkt*)calloc(UT_PKT_SZ,1);

	if (!ut) {
#ifdef ESP8266
    delay(500);
    for (int i = 0; i < RTS_PKT_SZ; i++) Serial.write(0x01);
#endif
		return nullptr;
	}

	packets::pkt_header header;
	memcpy(&header, pkt, sizeof(packets::pkt_header));

	uint32_t start_sig = header.start_sig;
	uint8_t protocol_ver = header.proto_ver;
	uint16_t type = header.type;
#ifdef ESP8266
  delay(500);
	for (int i = 0; i < RTS_PKT_SZ; i++) Serial.write(pkt[i]);
#endif // ESP8266

 

	switch (type) {
		case HELLO:
			if (pkt_sz != HELLO_PKT_SZ) return nullptr;
			h = (packets::hello*)pkt;

			ut->start_sig = h->sig;
			ut->proto_ver = h->proto_ver;
			ut->type = h->type;
			ut->ec_ver = h->ec_ver;

			return ut;

		case REQ:
			if (pkt_sz != REQ_PKT_SZ) return nullptr;
			r = (packets::req*)pkt;

			ut->type = r->type;
			ut->start_sig = r->start_sig;
			ut->proto_ver = r->proto_ver;
			ut->end_sig = r->end_sig;

			return ut;

		case RESP_KEY: 
			if (pkt_sz != RESP_KEY_PKT_SZ) return nullptr;
			rk = (packets::resp_key*)pkt;

			ut->type = rk->type;
			ut->start_sig = rk->start_sig;
			ut->proto_ver = rk->proto_ver;
			ut->end_sig = rk->end_sig;
			memcpy(ut->key, rk->key, 32);
			ut->key_checksum = rk->key_checksum;

			return ut;

		case RESP_HEAD:
			if (pkt_sz != RESP_HEAD_PKT_SZ) return nullptr;
			rh = (packets::resp_head*)pkt;

			ut->type = rh->type;
			ut->start_sig = rh->start_sig;
			ut->proto_ver = rh->proto_ver;
			ut->end_sig = rh->end_sig;
			ut->resp_prts = rh->resp_prts;
			ut->resp_size = rh->resp_size;

			return ut;

		case RESP_DATA:
			if (pkt_sz != RESP_DATA_PKT_SZ) return nullptr;
			rd = (packets::resp_data*)pkt;

			ut->type = rd->type;
			ut->start_sig = rd->start_sig;
			ut->proto_ver = rd->proto_ver;
			ut->end_sig = rd->end_sig;
			ut->resp_seq_num = rd->resp_seq_num;
			ut->responce = rd->responce;
			memcpy(ut->data, rd->data, packets::data_payload_sz);

			return ut;

		case PKT_REQ:
			if (pkt_sz != PKT_REQ_PKT_SZ) return nullptr;
			pr = (packets::pkt_req*)pkt;

			ut->type = pr->type;
			ut->start_sig = pr->start_sig;
			ut->proto_ver = pr->proto_ver;
			ut->end_sig = pr->end_sig;
			ut->req_seq_num = pr->req_seq_num;

			return ut;

		case COMMAND:
			if (pkt_sz != CMD_PKT_SZ) return nullptr;
			cmd = (packets::command*)pkt;

			ut->type = cmd->type;
			ut->start_sig = cmd->start_sig;
			ut->proto_ver = cmd->proto_ver;
			ut->end_sig = cmd->end_sig;
			ut->command = cmd->command;

			return ut;

		case RTS:
			if (pkt_sz != RTS_PKT_SZ) return nullptr;
			rts = (packets::rts*)pkt;

			ut->type = rts->type;
			ut->start_sig = rts->start_sig;
			ut->proto_ver = rts->proto_ver;
			ut->end_sig = rts->end_sig;
			ut->rts_timeout = rts->rts_timeout;

			return ut;

		case CTS:
			if (pkt_sz != CTS_PKT_SZ) return nullptr;
			cts = (packets::cts*)pkt;

			ut->type        = cts->type;
			ut->start_sig   = cts->start_sig;
			ut->proto_ver   = cts->proto_ver;
			ut->end_sig     = cts->end_sig;
			ut->cts_timeout = cts->cts_timeout;

			return ut;
	}

	return nullptr;
}