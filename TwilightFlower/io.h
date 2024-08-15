#pragma once

#include <Windows.h>
#include <atlstr.h>


bool WriteComPort(CString PortSpecifier, CString data, size_t lenght);
bool ReadComPort(CString PortSpecifier, void* data, size_t lenght);
