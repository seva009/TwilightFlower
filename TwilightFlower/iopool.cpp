#include <malloc.h>
#include <thread>
#include "iopool.h"
#include "io.h"
#include <stdio.h>
#include <stdexcept>

#pragma warning(disable : 26495)
IOPool_esp::IOPool_esp(CString port) {
    
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
    opool = calloc(1,1);
	ipool = calloc(1,1);
    locked_i = false;
	locked_o = false;
    ipool_sz = 0;
	opool_sz = 0;
    tmp_buf_sz = 0;
    read_t = std::thread(&IOPool_esp::inf_read_bytes, this);
    read_t.detach();
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

void IOPool_esp::merge_buf(void** buf, void* adn_buf, size_t* buf_sz, size_t adn_buf_sz) {
    // Проверяем, что переданные параметры валидны
    if (!buf || !*buf || !adn_buf || !buf_sz) return;

    // Выделяем новую память для объединенного буфера
    void* new_buf = realloc(*buf, *buf_sz + adn_buf_sz);
    if (!new_buf) {
        // Ошибка выделения памяти, возвращаем ничего
        return;
    }

    // Копируем содержимое дополнительного буфера в новое место
    memcpy((uint8_t*)new_buf + *buf_sz, adn_buf, adn_buf_sz);

    // Обновляем размер буфера
    *buf_sz += adn_buf_sz;  // Исправлено: использована правильная переменная

    // Обновляем указатель на буфер
    *buf = new_buf;
}

void IOPool_esp::write_ipool() {
    lock_o();
    if (locked_i)
    {
        while (locked_i)
        {
            Sleep(1);
        }
    }
    
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
        printf("locked_o\n");
		while (locked_o)
		{
			Sleep(1);
		}
	}
	lock_i();
    opool = realloc(opool, opool_sz + 1);
	ReadFile(hPort, (uint8_t*)opool + opool_sz, 1, NULL, NULL);
	opool_sz += 1;
    /*if (tmp_buf_sz - 1 == sizeof(tmp_buf) || last_byte_read - time(NULL) >= 1) {
        printf("tmp_buf_sz %d\n", tmp_buf_sz);
        merge_buf(&opool, tmp_buf, &opool_sz, sizeof(tmp_buf));
        opool_sz += tmp_buf_sz;
        tmp_buf_sz = 0;
    }*/
    last_byte_read = time(NULL);
	unlock_i();  
    //printf("read_byte end\n");
	return;
}

void IOPool_esp::inf_read_bytes() {
	while (true) {
		read_byte();
	}
}

void IOPool_esp::send_pkt(void* pkt, size_t pkt_sz) {
    lock_o();
    WriteFile(hPort, pkt, pkt_sz, NULL, NULL);
    unlock_o();
}

//well i dont want to fix it
//void* IOPool_esp::recv_pkt(size_t* ret_sz) {
//	if (ret_sz == nullptr || opool == nullptr) {
//		throw std::invalid_argument("Invalid arguments");
//	}
//
//	printf("recv_pkt\n");
//	printf("opool_sz: %zd\n", opool_sz);
//	for (int i = 0; i < opool_sz; i++) {
//		printf("%hx", ((uint8_t*)opool)[i]);
//	}
//
//	std::vector<std::vector<size_t>> pool;
//	do {
//		pool = thread_pkt_split(opool, opool_sz, TRS_SIG);
//		if (pool.size() == 0) {
//			printf("opool sz:%zd\n", opool_sz);
//			Sleep(1);
//		}
//	} while (pool.size() == 0);
//
//	printf("Pool size: %zd\n", pool.size());
//	*ret_sz = pool[0][0] - pool[0][1];
//
//	void* ret = calloc(pool[0][0] - pool[0][1], 1);
//	//if (ret == nullptr) {
//	//	throw std::bad_alloc();
//	//}
//
//	memcpy(ret, (uint8_t*)opool + pool[0][0], *ret_sz);
//
//	void* new_opool = calloc(opool_sz - *ret_sz, 1);
//	if (new_opool == nullptr) {
//		free(ret);
//		throw std::bad_alloc();
//	}
//
//	memcpy(new_opool, (uint8_t*)opool + pool[0][0], opool_sz - *ret_sz);
//	free(opool);
//	opool = new_opool;
//	opool_sz -= *ret_sz;
//
//	return ret;
//}

void* IOPool_esp::recv_pkt(size_t ret_sz) {
    void* result = malloc(ret_sz);
    while (opool_sz < ret_sz) {
        Sleep(10);
    }
    void* tmp_opool = malloc(opool_sz);
    memcpy(tmp_opool, opool, opool_sz);
    opool = realloc(opool, opool_sz - ret_sz);
    memcpy(opool, (uint8_t*)tmp_opool + ret_sz, opool_sz - ret_sz);
    memcpy(result, tmp_opool, ret_sz);
    free(tmp_opool);
    return result;
}
