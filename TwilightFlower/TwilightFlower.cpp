// EasyCrypt.cpp: определяет точку входа для приложения.
//
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "io.h"
#include "crc32.h"
#include "EasyDTP.h"
#include "pkt_io_hdl.h"
#include "iopool.h"

const char* hw = "Hello, world!";
int main()
{
	//unsigned short q = 'a';
	//for (int i = 0; i < 14; i++) {
	//	if (WriteComPort("\\\\.\\COM6", hw[i]) == 0) {
	//		printf("Can't open com6 port");
	//	}
	//	//printf("%c", hw[i]);
	//	q = ReadByte("\\\\.\\COM6");
	//	printf("%c", q);
	//}

	/*packets::hello* pkt = (packets::hello*)malloc(HELLO_PKT_SZ);
	pkt->sig = TRS_SIG;
	pkt->proto_ver = PROTO_VER;
	pkt->type = (uint16_t)pkt_types::HELLO;
	pkt->ec_ver = EC_VER;

	packets::ut_pkt* ut = pkt_parser((uint8_t*)pkt, HELLO_PKT_SZ);

	printf(" sig: %hxx\n protocol version:%u\n type: %u\n EC version: %u\n", ut->start_sig, ut->proto_ver, ut->type, ut->ec_ver);
	free(ut);
	free(pkt);
	packets::req* pkt1 = (packets::req*)malloc(REQ_PKT_SZ);

	pkt1->start_sig = TRS_SIG;
	pkt1->proto_ver = PROTO_VER;
	pkt1->type = (uint16_t)pkt_types::REQ;
	pkt1->end_sig = TRS_SIG;

	packets::ut_pkt* ut1 = pkt_parser((uint8_t*)pkt1, REQ_PKT_SZ);

	printf("Testing request parser...\n");

	if (ut1 == nullptr) {
		printf("error");
	}

	else printf(" sig: %hxx\n protocol version:%u\n type: %u\n end sig: %hxx\n", ut1->start_sig, ut1->proto_ver, ut1->type, ut1->end_sig);

	packets::resp_key* pkt2 = (packets::resp_key*)malloc(RESP_KEY_PKT_SZ);

	pkt2->start_sig = TRS_SIG;
	pkt2->proto_ver = PROTO_VER;
	pkt2->type = (uint16_t)pkt_types::RESP_KEY;

	uint8_t key[32] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32 };
	memcpy(pkt2->key, key, 32);
	pkt2->key_checksum = crc32(key, 32);
	pkt2->end_sig = TRS_SIG;

	packets::ut_pkt* ut2 = pkt_parser((uint8_t*)pkt2, RESP_KEY_PKT_SZ);

	printf("Testing RESP_KEY parser...\n\n");

	printf(" sig: %hxx\n protocol version:%u\n type: %u\nkey checksum: %u\n end sig: %hxx\n", ut2->start_sig, ut2->proto_ver, ut2->type,ut2->key_checksum, ut2->end_sig);
	for (int i = 0; i < 32; i++) printf("%u,", ut2->key[i]);

	printf("\nTestting splitter\n");

	void* th = calloc(REQ_PKT_SZ + RESP_KEY_PKT_SZ + 1, 1);
	memcpy(th, pkt1, REQ_PKT_SZ);
	memcpy((uint8_t*)th + REQ_PKT_SZ + 1, pkt2, RESP_KEY_PKT_SZ);

	std::vector<std::vector<size_t>> cl = thread_pkt_split(th, RESP_KEY_PKT_SZ + REQ_PKT_SZ + 1, TRS_SIG);

	for (int i = 0; i < cl.size(); i++) {
		printf("%zu,%zu\n", cl[i][0], cl[i][1]);
	}

	packets::rts pkt3;

	pkt3.start_sig = TRS_SIG;
	pkt3.proto_ver = PROTO_VER;
	pkt3.type = (uint16_t)pkt_types::RTS;
	pkt3.end_sig = TRS_SIG;
	pkt3.rts_timeout = 50;

	printf("Testing RTS parser...\n\n");


	packets::ut_pkt* ut3 = pkt_parser((uint8_t*)&pkt3, RTS_PKT_SZ);
	printf(" sig: %hxx\n protocol version:%u\n type: %u\n rts timeout: %u\n end sig: %hxx\n", ut3->start_sig, ut3->proto_ver, ut3->type, ut3->rts_timeout, ut3->end_sig);

	packets::cts pkt4;

	pkt4.start_sig = TRS_SIG;
	pkt4.proto_ver = PROTO_VER;
	pkt4.type = (uint16_t)pkt_types::CTS;
	pkt4.end_sig = TRS_SIG;
	pkt4.cts_timeout = 50;

	void* test = calloc(CTS_PKT_SZ, 1);

	printf("Testing CTS parser...\n\n");

	packets::ut_pkt* ut4 = pkt_parser((uint8_t*)&pkt4, CTS_PKT_SZ);
	printf(" sig: %hxx\n protocol version:%u\n type: %u\n cts timeout: %u\n end sig: %hxx\n", ut4->start_sig, ut4->proto_ver, ut4->type, ut4->cts_timeout, ut4->end_sig);

	uint8_t* test_buf = (uint8_t*)calloc(256, 1);
	uint8_t* test_buf2 = (uint8_t*)calloc(256, 1);*/
	//for (int i = 0; i < 256; i++) {
	//	test_buf[i] = 0x19;
	//}
	//for (int i = 0; i < 256; i++) {
	//	printf("0x%hx,", test_buf[i]);
	//}
	/*printf("\n\n");
	for (int i = 0; i < 256; i++) {
		printf("0x%hx,", i);
		WriteComPort("\\\\.\\COM6", CString((char*)&i, 1), 1);
		ReadComPort("\\\\.\\COM6", test_buf2, 1);
		printf("0x%hx ", test_buf2[0]);
	}*/

	//for (int i = 0; i < 256; i++) printf("0x%hx,", ((uint8_t*)test_buf2)[i]);
	uint8_t key[32];
	EasyDTP dtp(6);
	dtp.TransferKey(key);
	return 0;
}
