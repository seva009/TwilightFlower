#pragma once

enum class pkt_types
{
	UNKNOWN = 0,

	HELLO,

	REQ_KEY,
	RESP_KEY,

	REQ,
	RESP_HEAD,
	RESP,
	RESP_STATUS,

	LOST_PKT_REQ,

	COMMAND,

	RTS,
	CTS
};

enum class resp_status
{
	UNKNOWN = 0,
	OK,
	FAIL
};

