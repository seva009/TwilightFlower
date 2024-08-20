#include "packets.h"
#include "types.h"
#include "pkt_io_hdl.h"
#include <string.h>
#include <malloc.h>
#include <vector>

std::vector<std::vector<size_t>> thread_pkt_split(void* thread, size_t thread_sz, uint32_t rec_sig) {
	std::vector<std::vector<size_t>> pool;
	const char* sig = reinterpret_cast<const char*>(&rec_sig);
	char* str = static_cast<char*>(calloc(4, sizeof(char)));

	std::vector<size_t> pkt_ps;
	pkt_ps.resize(2, 0);
	bool isStartPoint = true;
	size_t thread_ptr = 0;

	if (!str || !sig || !thread) {
		//free(str);
		return pool;
	}

	while (thread_sz - thread_ptr > 0) {
		char* memcpy_result = reinterpret_cast<char*>(memcpy(str, reinterpret_cast<char*>(thread) + thread_ptr, 4));
		if (!memcpy_result) {
			//free(str);
			return pool;
		}

		int memcmp_result = memcmp(sig, str, 4);
		if (memcmp_result == 0) {
			pkt_ps[isStartPoint] = thread_ptr; // somnitel'no no okey
			if (!isStartPoint) {
				pool.push_back(pkt_ps);
				pkt_ps = { 0, 0 };
			}
			isStartPoint = !isStartPoint;
		}

		thread_ptr++;
	}

	//free(str);
	return pool;
}

packets::ut_pkt* pkt_parser(uint8_t* pkt, size_t pkt_sz) {

	packets::ut_pkt* ut = (packets::ut_pkt*)malloc(UT_PKT_SZ);

	if (!ut) {
		return nullptr;
	}

	memset(ut, 0, UT_PKT_SZ);

	uint32_t start_sig = *(uint32_t*)pkt;
	uint8_t protocol_ver = *(uint8_t*)(pkt + 4);
	uint16_t type = *(uint16_t*)(pkt + 5);

	if (start_sig != TRS_SIG || protocol_ver != PROTO_VER) {
		return nullptr;
	}

	switch (type) {
		case (int)pkt_types::HELLO:
			if (pkt_sz != HELLO_PKT_SZ) return nullptr;
			ut->type = type;
			ut->start_sig = start_sig;
			ut->proto_ver = protocol_ver;
			memcpy(&(ut->ec_ver), pkt + pkt_sz - 2, 2);
			return ut;

		case (int)pkt_types::REQ_KEY:
			if (pkt_sz != REQ_PKT_SZ) return nullptr;
			ut->type = type;
			ut->start_sig = start_sig;
			ut->proto_ver = protocol_ver;
			ut->end_sig = *(uint32_t*)(pkt + pkt_sz - 4);
			return ut;

		case (int)pkt_types::REQ:
			if (pkt_sz != REQ_PKT_SZ) return nullptr; //done
			ut->type = type;
			ut->start_sig = start_sig;
			ut->proto_ver = protocol_ver;
			ut->end_sig = *(uint32_t*)(pkt + pkt_sz - 4);
			return ut;

		case (int)pkt_types::RESP_KEY:                             //done
			if (pkt_sz != RESP_KEY_PKT_SZ) return nullptr;
			ut->type = type;
			ut->start_sig = start_sig;
			ut->proto_ver = protocol_ver;
			ut->end_sig = *(uint32_t*)(pkt + pkt_sz - 4);
			memcpy(ut->key, pkt + 7, 32);
			memcpy(&(ut->key_checksum), pkt + 7 + 32, sizeof(uint32_t));
			return ut;

		case (int)pkt_types::RESP_HEAD:
			if (pkt_sz != RESP_HEAD_PKT_SZ) return nullptr;
			ut->type = type;
			ut->start_sig = start_sig;
			ut->proto_ver = protocol_ver;
			ut->end_sig = *(uint32_t*)(pkt + pkt_sz - 4);
			ut->resp_prts = *(uint8_t*)(pkt + 7);
			ut->resp_size = *(uint32_t*)(pkt + 8);
			return ut;

		case (int)pkt_types::RESP:
			if (pkt_sz != RESP_DATA_PKT_SZ) return nullptr;
			ut->type = type;
			ut->start_sig = start_sig;
			ut->proto_ver = protocol_ver;
			ut->end_sig = *(uint32_t*)(pkt + pkt_sz - 4);

			ut->resp_seq_num = *(uint8_t*)(pkt + 7);
			ut->responce = *(uint32_t*)(pkt + 8);
			memcpy(ut->data, pkt + 12, packets::data_payload_sz);
			return ut;

		case (int)pkt_types::LOST_PKT_REQ:
			if (pkt_sz != PKT_REQ_PKT_SZ) return nullptr;
			ut->type = type;
			ut->start_sig = start_sig;
			ut->proto_ver = protocol_ver;
			ut->end_sig = *(uint32_t*)(pkt + pkt_sz - 4);

			ut->req_seq_num = *(uint8_t*)(pkt + 7);
			return ut;

		case (int)pkt_types::COMMAND:
			if (pkt_sz != CMD_PKT_SZ) return nullptr;
			ut->type = type;
			ut->start_sig = start_sig;
			ut->proto_ver = protocol_ver;
			ut->end_sig = *(uint32_t*)(pkt + pkt_sz - 4);
			ut->command = *(uint8_t*)(pkt + 7);
			return ut;

		case (int)pkt_types::RTS:
			if (pkt_sz != RTS_PKT_SZ) return nullptr;
			ut->type = type;
			ut->start_sig = start_sig;
			ut->proto_ver = protocol_ver;
			ut->end_sig = *(uint32_t*)(pkt + pkt_sz - 4);
			ut->rts_timeout = *(uint32_t*)(pkt + 7);

			return ut;

		case (int)pkt_types::CTS:
			if (pkt_sz != CTS_PKT_SZ) return nullptr;
			ut->type = type;
			ut->start_sig = start_sig;
			ut->proto_ver = protocol_ver;
			ut->end_sig = *(uint32_t*)(pkt + pkt_sz - 4);
			ut->cts_timeout = *(uint32_t*)(pkt + 7);
			return ut;
		case (int)pkt_types::RESP_STATUS:
			if (pkt_sz != RESP_STATUS_PKT_SZ) return nullptr;
			ut->type = type;
			ut->start_sig = start_sig;
			ut->proto_ver = protocol_ver;
			ut->end_sig = *(uint32_t*)(pkt + pkt_sz - 4);
			ut->status = *(uint8_t*)(pkt + 7);
			return ut;

		default:
			return nullptr;
	}

	return nullptr;
}