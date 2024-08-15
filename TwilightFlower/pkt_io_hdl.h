#pragma once

#include "packets.h"
#include "types.h"
#include <vector>

#define HELLO_PKT_SZ       sizeof(packets::hello)
#define REQ_PKT_SZ         sizeof(packets::req)
#define RESP_KEY_PKT_SZ    sizeof(packets::resp_key)
#define RESP_STATUS_PKT_SZ sizeof(packets::resp_s)
#define RESP_HEAD_PKT_SZ   sizeof(packets::resp_head)
#define RESP_DATA_PKT_SZ   sizeof(packets::resp)
#define PKT_REQ_PKT_SZ     sizeof(packets::pkt_req)
#define CMD_PKT_SZ         sizeof(packets::command)
#define RTS_PKT_SZ         sizeof(packets::rts)
#define CTS_PKT_SZ         sizeof(packets::cts)
#define UT_PKT_SZ          sizeof(packets::ut_pkt)

#define TRS_SIG 0xdc00ffdd
#define REC_SIG 0xaaff

#define EC_VER 1000
#define PROTO_VER 1

packets::ut_pkt* pkt_parser(uint8_t* pkt, size_t pkt_sz);
std::vector<std::vector<size_t>> thread_pkt_split(void* thread, size_t thread_sz, uint32_t rec_sig);