#pragma once
#include <iostream>
#include <Windows.h>
class memory
{
public:
	memory();
	~memory();
	int getProcID(const std::wstring& pName);
	DWORD read(HANDLE handle, LPCVOID address);
	void writeb(HANDLE handle, LPVOID address, BOOL value);
	void writef(HANDLE handle, LPVOID address, float value);
};

