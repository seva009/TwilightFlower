#include <Windows.h>
#include <atlstr.h>
#include "io.h"
#include <fstream>

bool WriteComPort(CString PortSpecifier, CString data, size_t length)
{
    DCB dcb;
    DWORD byteswritten;
    HANDLE hPort = CreateFileA(
        PortSpecifier,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    if (!GetCommState(hPort, &dcb))
        return false;
    dcb.BaudRate = CBR_115200; //9600 Baud
    dcb.ByteSize = 8; //8 data bits
    dcb.Parity = NOPARITY; //no parity
    dcb.StopBits = ONESTOPBIT; //1 stop
    if (!SetCommState(hPort, &dcb))
        return false;
    bool retVal = WriteFile(hPort, data, length, &byteswritten, NULL);
    /*std::fstream fs("\\\\.\\COM6");
    fs << data;*/
    CloseHandle(hPort); //close the handle
    return true;
}

bool ReadComPort(CString PortSpecifier, void* data, size_t length)
{
    DCB dcb;
    memset(&dcb, 0, sizeof(DCB));
    int retVal;
    BYTE Byte;
    DWORD dwBytesTransferred;
    DWORD dwCommModemStatus;
    HANDLE hPort = CreateFile(
        PortSpecifier,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    if (!GetCommState(hPort, &dcb))
        return false;
    dcb.BaudRate = CBR_115200; //9600 Baud
    dcb.ByteSize = 8; //8 data bits
    dcb.Parity = NOPARITY; //no parity
    dcb.StopBits = ONESTOPBIT; //1 stop
    if (!SetCommState(hPort, &dcb))
        return false;
    SetCommMask(hPort, EV_RXCHAR | EV_ERR); //receive character event
    WaitCommEvent(hPort, &dwCommModemStatus, 0); //wait for character
    if (dwCommModemStatus & EV_RXCHAR)
        ReadFile(hPort, data, length, &dwBytesTransferred, 0); //read 1
    else if (dwCommModemStatus & EV_ERR)
        retVal = 0x101;
    CloseHandle(hPort);
    return true;
}