/*
��� ��� ������ ��������:
�� ������ ������� ����� ������� ������ �������� ����
��� ������� ������� ����� (recv_pkt(size_t* ret_sz)) �� ������ ����� ������ ���������� ����� � ������� ��� �� opool
�����! ���� ����� ��������� �.�. thread_pkt_split() ������ ������ ��� �� ������� ����� ����� ���������� ���� �� �������� �����
��� ������� �������� ���-���� ����� send_pkt() ����� ������ ����������� � ���� �� ����� ������� ����� ����� ����� �����
�����! send_pkt() ���������� ������������ ���� ��� ���� ����� � ���� esp ������������� ��� �� ��� ������� ��������� ��������� ������� ����
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
	HANDLE hPortW;
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
	void read_bytes();
	void lock_i();
	void unlock_i();
	void lock_o();
	void unlock_o();
public:
	IOPool_esp(const wchar_t* port);
	void* recv_pkt(size_t* ret_sz);
	void send_pkt(void* pkt, size_t pkt_sz);
	~IOPool_esp();
};