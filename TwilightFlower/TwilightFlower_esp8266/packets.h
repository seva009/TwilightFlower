#pragma once

#include "types.h"


#ifdef __GNUC__
#define packed( __decl__ ) __decl__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define packed( __decl__ ) __pragma( pack(push, 1) ) __decl__ __pragma( pack(pop))
#endif

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

#define TRS_SIG 0xdc00ffdd
#define REC_SIG 0xaaff

#define EC_VER 1000
#define PROTO_VER 1

namespace packets {
	const uint16_t data_payload_sz = 256;
	//server and client hello

	typedef packed(struct {
		uint32_t sig = TRS_SIG;      //will be used by server or by client
		uint8_t  proto_ver = PROTO_VER;//protocol version
		uint16_t type = HELLO;     //type
		uint16_t ec_ver = EC_VER;   //EasyCrypt version
    uint32_t end_sig = TRS_SIG;
	}) hello;

	typedef packed(struct {
		uint32_t start_sig = TRS_SIG; //signature
		uint8_t  proto_ver = PROTO_VER; //protocol version
		uint16_t type;      //type
		uint32_t end_sig = TRS_SIG;   //signature
	}) req;

	//key response

	typedef packed(struct {
		uint32_t start_sig = TRS_SIG;     //signature
		uint8_t  proto_ver = PROTO_VER;     //protocol version
		uint16_t type      = RESP_KEY;          //type
		uint8_t  key[32];       //key 
		uint32_t key_checksum;  //key checksum
		uint32_t end_sig   = TRS_SIG;       //signature
	}) resp_key;

	typedef packed(struct {
		uint32_t start_sig = TRS_SIG;    //signature
		uint8_t  proto_ver = PROTO_VER;    //protocol version
		uint16_t type      = RESP_STATUS;         //type
		uint8_t  resp;         //response
		uint32_t end_sig   = TRS_SIG;      //signature
	}) resp_s;

	//response head

	typedef packed(struct {
		uint32_t start_sig = TRS_SIG;    //signature
		uint8_t  proto_ver = PROTO_VER;    //protocol version
		uint16_t type      = RESP_HEAD;         //type
		uint8_t  resp_prts;    //number of response parts
		uint32_t resp_size;    //size of each response part
		uint32_t end_sig   = TRS_SIG;      //signature
	}) resp_head;

	//response part

	typedef packed(struct {
		uint32_t start_sig = TRS_SIG;   //signature
		uint8_t  proto_ver = PROTO_VER;   //protocol version
		uint16_t type      = RESP_DATA;        //type
		uint8_t  resp_seq_num;//sequence number
		uint8_t  responce;    //response
		uint8_t  data[data_payload_sz];   //data
		uint32_t end_sig   = TRS_SIG;     //signature
	}) resp_data;

	//lost packet request

	typedef packed(struct {
		uint32_t start_sig = TRS_SIG;   //signature
		uint8_t  proto_ver = PROTO_VER;   //protocol version
		uint16_t type      = PKT_REQ;        //type
		uint8_t  req_seq_num; //sequence number
		uint32_t end_sig   = TRS_SIG;     //signature
	}) pkt_req;

	//command

	typedef packed(struct {
		uint32_t start_sig = TRS_SIG; //signature
		uint8_t  proto_ver = PROTO_VER; //protocol version
		uint16_t type      = COMMAND;      //type
		uint8_t  command;   //command
		uint32_t end_sig   = TRS_SIG;   //signature
	}) command;

	typedef packed(struct {
		uint32_t start_sig = TRS_SIG;  //signature
		uint8_t  proto_ver = PROTO_VER;  //protocol version
		uint16_t type      = RTS;       //type
		uint8_t  rts_timeout = 0x64;//request to send timeout
		uint32_t end_sig   = TRS_SIG;    //signature
	}) rts;

	typedef packed(struct {
		uint32_t start_sig = TRS_SIG;  //signature
		uint8_t  proto_ver = PROTO_VER;  //protocol version
		uint16_t type      = CTS;       //type
		uint8_t  cts_timeout = 0x64;//clear to send timeout
		uint32_t end_sig   = TRS_SIG;    //signature
	}) cts;

	typedef packed(struct {
		uint32_t start_sig;  //signature
		uint8_t  proto_ver;  //protocol version
		uint16_t type;       //type
		uint8_t  cts_timeout;//clear to send timeout
		uint8_t  rts_timeout;//request to send timeout
		uint8_t command;     //command
		uint8_t data[data_payload_sz];   //data
		uint8_t key[32];     //key
		uint32_t key_checksum;//key checksum
		uint8_t  responce;    //response
		uint8_t  resp_prts;   //number of response parts
		uint32_t resp_size;   //size of each response part
		uint8_t  seq_num;     //sequence number
		uint16_t  ec_ver;      //EasyCrypt version
		uint8_t  resp_seq_num;//sequence number
		uint8_t  req_seq_num; //sequence number
		uint8_t  resp;        //response
		uint32_t end_sig;    //signature
	}) ut_pkt; //universal tecnical packet

	typedef packed(struct {
		uint32_t start_sig;  //signature
		uint8_t  proto_ver;  //protocol version
		uint16_t type;       //type
	}) pkt_header;
}