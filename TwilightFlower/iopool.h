/*
Как это должно работать:
Со старта запущен поток который читает указаный порт
При попытке достать пакет (recv_pkt(size_t* ret_sz)) он выдает самый первый полученный пакет и удаляет его из opool
ВАЖНО! если пакет поврежден т.е. thread_pkt_split() выдает пустой пул то функция будет ждать БЕСКОНЕЧНО пока не появится пакет
При попытке записать что-либо через send_pkt() поток чтения блокируется и пока не будет записан пакет поток будет ждать
ВАЖНО! send_pkt() игнорирует правильность того что туда пишут и если esp проигнорирует это то при попытке прочитать программа залочит себя
*/


#pragma once
#include "pkt_io_hdl.h"
#include <Windows.h>
#include <atlstr.h>
#include <time.h>
#include <thread>

class IOPool_esp {
private:
	std::thread read_t;
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
	void* recv_pkt(size_t ret_sz);
	void send_pkt(void* pkt, size_t pkt_sz);
	~IOPool_esp();
};