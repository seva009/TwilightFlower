#include <malloc.h>
#include <thread>
#include "iopool.h"
#include "io.h"
#include <stdio.h>

#pragma warning(disable : 26495)
IOPool_esp::IOPool_esp(CString port) {
    //std::thread t(&IOPool_esp::inf_read_bytes, this);
    hPort = CreateFile(
        port,
        GENERIC_WRITE | GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    DCB dcbSerialParams;
    if (!GetCommState(hPort, &dcbSerialParams))
        return;
    dcbSerialParams.BaudRate = CBR_115200; //115200 Baud
    dcbSerialParams.ByteSize = 8; //8 data bits
    dcbSerialParams.Parity = NOPARITY; //no parity
    dcbSerialParams.StopBits = ONESTOPBIT; //1 stop
    if (!SetCommState(hPort, &dcbSerialParams))
        return;
    memset(tmp_buf, 0, tmp_buf_sz);
    locked_i = false;
	locked_o = false;
    ipool_sz = 0;
	opool_sz = 0;
    tmp_buf_sz = 0;
	return;
}

IOPool_esp::~IOPool_esp() {
	CloseHandle(hPort);
}

void IOPool_esp::lock_i() {
	locked_i = true;
}

void IOPool_esp::lock_o() {
	locked_o = true;
}

void IOPool_esp::unlock_i() {
	locked_i = false;
}

void IOPool_esp::unlock_o() {
	locked_o = false;
}

void IOPool_esp::merge_buf(void** buf, void* tmp_buf, size_t* buf_sz, size_t tmp_buf_sz) {
    *buf = realloc(*buf, *buf_sz + tmp_buf_sz);
	memcpy((uint8_t*)(*buf) + *buf_sz, tmp_buf, tmp_buf_sz);
	*buf_sz += tmp_buf_sz;
}

void IOPool_esp::write_ipool() {
    if (locked_i)
    {
        while (locked_i)
        {
            Sleep(1);
        }
    }
    lock_o();
    //merge_buf(&ipool, tmp_buf, &ipool_sz, tmp_buf_sz);
    if (!WriteFile(hPort, ipool, ipool_sz, NULL, NULL)) printf("Can't write\n");
	ipool_sz = 0;
    free(ipool);
	unlock_o();
	return;
}

void IOPool_esp::read_byte() {
	if (locked_o)
	{
		while (locked_o)
		{
			Sleep(1);
		}
	}
	lock_i();
    if (tmp_buf_sz - 1 == sizeof(tmp_buf)) {
        merge_buf(&opool, tmp_buf, &opool_sz, sizeof(tmp_buf));
		tmp_buf_sz = 0;
    }
	ReadFile(hPort, tmp_buf + tmp_buf_sz, 1, NULL, NULL);
	unlock_i();
	return;
}

void IOPool_esp::inf_read_bytes() {
	while (true) {
		read_byte();
	}
}

void IOPool_esp::send_pkt(void* pkt, size_t pkt_sz) {
    merge_buf(&ipool, pkt, &ipool_sz, pkt_sz);
    printf("%d\n", ipool_sz);
	write_ipool();
}

void* IOPool_esp::recv_pkt(size_t* ret_sz) {
	std::vector<std::vector<size_t>> pool = thread_pkt_split(opool, opool_sz, TRS_SIG);
	while (pool.size() == 0) {
        read_byte();
		pool = thread_pkt_split(opool, opool_sz, TRS_SIG);
	}
    *ret_sz = pool[0][1] - pool[0][0];
    void* ret = calloc(*ret_sz, 1);
    memcpy(ret, (uint8_t*)opool + pool[0][0], *ret_sz);
	void* new_opool = calloc(opool_sz - *ret_sz, 1);
	memcpy(new_opool, (uint8_t*)opool + pool[0][1], opool_sz - *ret_sz);
	free(opool);
	opool = new_opool;
	opool_sz -= *ret_sz;
	return ret;
}