// EasyCrypt.h : включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта.

#pragma once

#include "commands.h"
#include "pkt_io_hdl.h"
#include <atlstr.h>

class EasyDTP { //Data Trancfer Protocol
private:
	uint8_t PrimaryKey[32];
	CString COM_port = "\\\\.\\COM6";
public:
	bool SendPacket(void* pkt, size_t pkt_sz);
	bool RecvPacket(void* pkt, size_t pkt_sz);

	EasyDTP(int COM_number);
	bool TransferKey(uint8_t key[32]);
	bool SendCommand(Commands command);
	bool GetPrimaryKey();
	~EasyDTP();
};