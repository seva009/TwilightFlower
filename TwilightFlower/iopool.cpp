#include <malloc.h>
#include <thread>
#include "iopool.h"
#include "io.h"
#include <stdio.h>
#include <stdexcept>

OVERLAPPED overlappedrd, overlappedwr;
DWORD btr, temp, mask, signal; //btr - bytes to read � �� �� ��� �� �������(�) :)
bool write_status;

#pragma warning(disable : 26495)
IOPool_esp::IOPool_esp(const wchar_t* port) {
    overlappedrd.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
    hPort = CreateFile(
        CString("\\\\.\\COM6"),
        GENERIC_WRITE | GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
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
    SetCommMask(hPort, EV_RXCHAR);
    memset(tmp_buf, 0, tmp_buf_sz);
    opool = calloc(1,1);
	ipool = calloc(1,1);
    locked_i = false;
	locked_o = false;
    ipool_sz = 0;
	opool_sz = 0;
    tmp_buf_sz = 0;
    /*read_t = std::thread(&IOPool_esp::read_bytes, this);
    read_t.detach();*/
    Sleep(300);
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
    // ���������, ��� ���������� ��������� �������
    if (!buf || !*buf || !adn_buf || !buf_sz) return;

    // �������� ����� ������ ��� ������������� ������
    void* new_buf = realloc(*buf, *buf_sz + adn_buf_sz);
    if (!new_buf) {
        // ������ ��������� ������, ���������� ������
        return;
    }

    // �������� ���������� ��������������� ������ � ����� �����
    memcpy((uint8_t*)new_buf + *buf_sz, adn_buf, adn_buf_sz);

    // ��������� ������ ������
    *buf_sz += adn_buf_sz;  // ����������: ������������ ���������� ����������

    // ��������� ��������� �� �����
    *buf = new_buf;
}

void IOPool_esp::read_bytes() {
    printf("\nRead thread launched\n");
    COMSTAT comstat; //��������� �������� ��������� �����, � ������ ��������� ������������ ��� ��������������������� �������� � ���� ������
    DWORD btr, temp, mask, signal; //���������� temp ������������ � �������� ��������
    overlappedrd.hEvent = CreateEvent(NULL, true, true, NULL); //������� ���������� ������-������� �������������� ��������
    SetCommMask(hPort, EV_RXCHAR); //���������� ����� �� ������������ �� ������� ����� ����� � ����
    while (1) //���� ����� �� ����� �������, ��������� ����
    {

        if (!locked_o)
        {
            WaitCommEvent(hPort, &mask, &overlappedrd); //������� ������� ����� ����� (��� � ���� ������������� ��������)
            signal = WaitForSingleObject(overlappedrd.hEvent, INFINITE); //������������� ����� �� ������� �����
            if (signal == WAIT_OBJECT_0) //���� ������� ������� ����� ���������
            {
                if (GetOverlappedResult(hPort, &overlappedrd, &temp, true)) //���������, ������� �� ����������� ������������� �������� WaitCommEvent
                    if ((mask & EV_RXCHAR) != 0) //���� ��������� ������ ������� ������� �����
                    {
                        ClearCommError(hPort, &temp, &comstat); //����� ��������� ��������� COMSTAT
                        btr = comstat.cbInQue; //� �������� �� �� ���������� �������� ������
                        if (btr) //���� ������������� ���� ����� ��� ������
                        {
                            void* _tmp_buf = realloc(opool, opool_sz + btr);
                            ReadFile(hPort, (uint8_t*)_tmp_buf + opool_sz, btr, &temp, &overlappedrd); //��������� ����� �� ����� � ����� ���������
                            memcpy(_tmp_buf, opool, opool_sz);
                            opool_sz += btr;
                            opool = _tmp_buf;
                            for (int i = 0; i < btr; i++) {
                                printf("%hx, ", ((uint8_t*)opool)[i + opool_sz - btr]);
                            }   
                            printf("\n%zd\n", opool_sz);
                        }
                    }
            }
        }
        else {
            printf("\nLocked\n");
        }
    }
    CloseHandle(overlappedrd.hEvent); //����� ������� �� ������ ������� ������-�������
}

void IOPool_esp::write_ipool() {
    lock_o();
    WriteFile(hPortW, ipool, ipool_sz, NULL, NULL);
    unlock_o();
}

void xcut_buf(void** buf, size_t buf_sz, size_t p1, size_t p2, void** cut) {
	if (p1 >= p2 || p2 > buf_sz) {
		return;
	}

	*cut = malloc(p2 - p1);
	void* pre_cut_part = malloc(p1);
	void* post_cut_part = malloc(buf_sz - p2);

	if (*cut == nullptr || pre_cut_part == nullptr || post_cut_part == nullptr) {
		printf("Can't allocate memory");
		free(*cut);
		free(pre_cut_part);
		free(post_cut_part);
		return;
	}

	memcpy(pre_cut_part, *buf, p1);
	memcpy(post_cut_part, (uint8_t*)*buf + p2, buf_sz - p2);
	memcpy(*cut, (uint8_t*)*buf + p1, p2 - p1);

	void* tmp = malloc(p2 - p1);
	if (tmp == nullptr) {
		printf("Can't reallocate memory");
		free(*cut);
		free(pre_cut_part);
		free(post_cut_part);
		free(*buf);
		*buf = nullptr;
		return;
	}

	memcpy(tmp, pre_cut_part, p1);
	memcpy((uint8_t*)tmp + p1, post_cut_part, p2 - p1);
	free(*buf);
	*buf = tmp;
}


#pragma warning(disable : 6031)
void* IOPool_esp::recv_pkt(size_t* ret_sz) {
    COMSTAT comstat; //��������� �������� ��������� �����, � ������ ��������� ������������ ��� ��������������������� �������� � ���� ������
    DWORD btr, temp, mask, signal; //���������� temp ������������ � �������� ��������
    overlappedrd.hEvent = CreateEvent(NULL, true, true, NULL); //������� ���������� ������-������� �������������� ��������
    SetCommMask(hPort, EV_RXCHAR); //���������� ����� �� ������������ �� ������� ����� ����� � ����
    std::vector<std::vector<size_t>> pool;
    if (!opool) {
        printf("Error\n");
    }
    do {
        for (int i = 0; i < opool_sz; i++) printf("%hx, ", ((uint8_t*)opool)[i]);
        printf("\n");
        pool = thread_pkt_split(opool, opool_sz, TRS_SIG);
        //printf("%zd, ", opool_sz);
        if (!locked_o)
        {
            WaitCommEvent(hPort, &mask, &overlappedrd); //������� ������� ����� ����� (��� � ���� ������������� ��������)
            signal = WaitForSingleObject(overlappedrd.hEvent, INFINITE); //������������� ����� �� ������� �����
            if (signal == WAIT_OBJECT_0) //���� ������� ������� ����� ���������
            {
                if (GetOverlappedResult(hPort, &overlappedrd, &temp, true)) //���������, ������� �� ����������� ������������� �������� WaitCommEvent
                    if ((mask & EV_RXCHAR) != 0) //���� ��������� ������ ������� ������� �����
                    {
                        ClearCommError(hPort, &temp, &comstat); //����� ��������� ��������� COMSTAT
                        btr = comstat.cbInQue; //� �������� �� �� ���������� �������� ������
                        if (btr) //���� ������������� ���� ����� ��� ������
                        {
                            void* _tmp_buf = malloc(opool_sz + btr + 1);
                            if (!_tmp_buf) {
                                printf("error\n");
                                return nullptr;
                            }
                            memcpy(_tmp_buf, opool, opool_sz);
                            ReadFile(hPort, (uint8_t*)_tmp_buf + opool_sz, btr, &temp, &overlappedrd); //��������� ����� �� ����� � ����� ���������
                            memcpy(_tmp_buf, opool, opool_sz);
                            opool_sz += btr;
                            opool = _tmp_buf;
                        }
                    }
            }
        }
        else {
            printf("\nLocked\n");
        }
    } while (pool.size() < 1);
    *ret_sz = pool[0][0] - pool[0][1] + 4;

    void* pkt;
    xcut_buf(&opool, opool_sz, pool[0][1], pool[0][0] + 4, &pkt); //lol it useful
    return pkt;
    
}

void IOPool_esp::send_pkt(void* pkt, size_t pkt_sz) {
    printf("Writing...\n");
    DWORD temp, signal;
    overlappedwr.hEvent = CreateEvent(NULL, true, true, NULL); //������� �������
    WriteFile(hPort, pkt, pkt_sz, &temp, &overlappedwr); //�������� ����� � ���� (������������� ��������!)
    signal = WaitForSingleObject(overlappedwr.hEvent, INFINITE); //������������� �����, ���� �� ���������� ������������� �������� WriteFile
    if ((signal == WAIT_OBJECT_0) && (GetOverlappedResult(hPort, &overlappedwr, &temp, true))) write_status = true; //���� �������� ����������� �������, ���������� ��������������� ������
    else write_status = false;
}
