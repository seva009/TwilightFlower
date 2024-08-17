#pragma once
#include "pkt_io_hdl.h"
#include <Windows.h>
#include <atlstr.h>
#include <time.h>

class IOPool_esp {
private:
	HANDLE hPort;
	bool locked_i;
	bool locked_o;
	void* ipool;
	void* opool;
	uint8_t tmp_buf[32];
	size_t tmp_buf_sz;
	size_t ipool_sz;
	size_t opool_sz;
	time_t last_byte_read;
	void merge_buf(void** buf, void* tmp_buf, size_t* buf_sz, size_t tmp_buf_sz);
	void write_ipool();
	void read_byte();
	void inf_read_bytes();
	void lock_i();
	void unlock_i();
	void lock_o();
	void unlock_o();
public:
	IOPool_esp(CString port);
	void* recv_pkt(size_t* ret_sz);
	void send_pkt(void* pkt, size_t pkt_sz);
	~IOPool_esp();
};