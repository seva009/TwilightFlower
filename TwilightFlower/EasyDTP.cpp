#include "EasyDTP.h"
#include "types.h"
#include "packets.h"
#include "io.h"
#include "crc32.h"
#include <cstdio>
#include "iopool.h"

#define pksf(x) (x >= 10)?2:1
IOPool_esp iopool("\\\\.\\COM6");

bool EasyDTP::SendPacket(void* pkt, size_t pkt_sz) {
	iopool.send_pkt(pkt, pkt_sz);
	return true;
}

bool EasyDTP::RecvPacket(void* pkt, size_t pkt_sz) {
	void* tmp_pkt = iopool.recv_pkt(pkt_sz);
	memcpy(pkt, tmp_pkt, pkt_sz);
	return true;
}

EasyDTP::EasyDTP(int port) {

	/*memset(PrimaryKey, 0, 32);
	char* raw_com_port = (char*)calloc(10 + pksf(port), sizeof(char));
	sprintf(raw_com_port, "\\\\.\\COM%s", port);
	COM_port = CString(raw_com_port);
	free(raw_com_port);*/

}

EasyDTP::~EasyDTP() {
	memset(PrimaryKey, 0, 32);
	packets::command* hlt = (packets::command*)malloc(CMD_PKT_SZ);
	hlt->start_sig = TRS_SIG;
	hlt->proto_ver = PROTO_VER;
	hlt->type = (uint16_t)Commands::HALT;
	hlt->end_sig = TRS_SIG;
	SendPacket(hlt, CMD_PKT_SZ);
	free(hlt);
}

bool EasyDTP::TransferKey(uint8_t key[32]) {

	/*Step 1 - checking a connection*/

	//Allocating memory for RTS and CTS
	packets::rts* rts = (packets::rts*)malloc(RTS_PKT_SZ);
	void* cts = malloc(CTS_PKT_SZ);

	//Setting RTS
	rts->start_sig = TRS_SIG;
	rts->proto_ver = PROTO_VER;
	rts->type = (uint16_t)pkt_types::RTS;
	rts->end_sig = TRS_SIG;
	rts->rts_timeout = 0x51;

	printf("\nRTS: ");
	for (int i = 0; i < RTS_PKT_SZ; i++) {
		printf("%02x ", ((uint8_t*)rts)[i]);
	}

	//Sending RTS
	printf("\nSending RTS...\n");
	
	if (!SendPacket(rts, RTS_PKT_SZ)) {
		printf("Failed to send RTS...\n");
		return false;
	}

	//Receiving CTS
	printf("Receiving CTS...\n");
	
	if (!RecvPacket(cts, CTS_PKT_SZ)) {
		printf("Failed to receive CTS...\n");
		return false;
	}
	printf("Received CTS...\n");

	//Parsing CTS
	packets::ut_pkt* ut = pkt_parser((uint8_t*)cts, CTS_PKT_SZ);

	printf("CTS: ");
	for (int i = 0; i < CTS_PKT_SZ; i++) {
		printf("%02x ", ((uint8_t*)cts)[i]);
	}
	printf("\n");


	//Checking CTS
	if (ut == nullptr) {
		printf("Failed to parse CTS...\n");
		return false;
	}
	if (ut->type != (int)pkt_types::CTS) {
		printf("Invalid CTS packet...\n");
		free(ut);
		free(rts);
		free(cts);
		return false;
	}

	//Deallocating RTS and CTS
	free(rts);
	free(cts);
	free(ut);


	/*Step 2 - sending HELLO*/

	//Allocating memory for HELLO packet
	uint8_t* buf_hello = (uint8_t*)malloc(HELLO_PKT_SZ);
	packets::hello* hello_pkt = (packets::hello*)malloc(HELLO_PKT_SZ);

	//Setting HELLO packet
	hello_pkt->sig = TRS_SIG;
	hello_pkt->proto_ver = PROTO_VER;
	hello_pkt->type = (uint16_t)pkt_types::HELLO;
	hello_pkt->ec_ver = EC_VER;

	printf("\nSending HELLO...\n");
	//Sending HELLO packet
	if (!SendPacket(hello_pkt, HELLO_PKT_SZ)) {
		printf("Failed to send HELLO...\n");
		return false;
	}


	//Receiving HELLO packet
	if (!RecvPacket(buf_hello, HELLO_PKT_SZ)) {
		printf("Failed to receive HELLO...\n");
		return false;
	}


	//Parsing HELLO packet
	packets::ut_pkt* hello_ut = pkt_parser(buf_hello, HELLO_PKT_SZ);

	//Checking HELLO packet
	if (hello_ut == nullptr) {
		printf("Failed to parse HELLO...\n");
		return false;
	}
	if (hello_ut->type != (int)pkt_types::HELLO) {
		printf("Invalid HELLO packet...\n");
		free(hello_ut);
		return false;
	}

	printf("Received HELLO...\n");
	//Deallocating HELLO packet
	free(hello_ut);
	free(hello_pkt);
	free(buf_hello);

	/*Step 3 - sending KEY*/

	//Allocating memory for KEY
	uint8_t* buf_key_req = (uint8_t*)malloc(REQ_PKT_SZ);
	packets::resp_key* key_pkt = (packets::resp_key*)malloc(REQ_PKT_SZ);

	if (!RecvPacket(buf_key_req, REQ_PKT_SZ)) {
		printf("Failed to receive KEY request...\n");
		return false;
	}
	printf("Received KEY request...\n");

	//Parsing KEY request
	packets::ut_pkt* key_ut = pkt_parser(buf_key_req, REQ_PKT_SZ);

	//Checking KEY request
	if (key_ut == nullptr) {
		printf("Failed to parse KEY request...\n");
		return false;
	}
	if (key_ut->type != (int)pkt_types::REQ_KEY) {
		printf("Invalid KEY request packet...\n");
		free(key_ut);
		return false;
	}

	//Deallocating KEY request
	free(key_ut);
	free(buf_key_req);

	//Setting KEY packet
	key_pkt->start_sig = TRS_SIG;
	key_pkt->proto_ver = PROTO_VER;
	key_pkt->type = (uint16_t)pkt_types::RESP_KEY;
	key_pkt->end_sig = TRS_SIG;

	memcpy(key_pkt->key, key, 32);
	key_pkt->key_checksum = crc32(key, 32);

	//Sending KEY
	if (!SendPacket(key_pkt, REQ_PKT_SZ)) {
		printf("Failed to send KEY...\n");
		return false;
	}

	printf("Sent KEY...\n");

	//Deallocating KEY packet
	//free(key_pkt);

	uint8_t* resp = (uint8_t*)calloc(RESP_STATUS_PKT_SZ, 1);

	//Receiving KEY response
	if (!RecvPacket(resp, RESP_STATUS_PKT_SZ)) {
		printf("Failed to receive KEY response...\n");
		return false;
	}

	printf("Received KEY response...\n");

	//Parsing KEY response
	packets::ut_pkt* resp_ut = pkt_parser(resp, RESP_STATUS_PKT_SZ);

	//Checking KEY response
	if (resp_ut == nullptr) {
		printf("Failed to parse KEY response...\n");
		return false;
	}
	if (resp_ut->type != (int)pkt_types::RESP_STATUS) {
		printf("Invalid KEY response packet...\n");
		free(resp_ut);
		return false;
	}
	printf("KEY response status: %d\n", resp_ut->status);
	if (resp_ut->status != (int)resp_status::OK) {
		printf("Invalid KEY response status...\n");
		free(resp_ut);
		return false;
	}

	//Deallocating KEY response
	//free(resp_ut);
	//free(resp); 
	return true;
}