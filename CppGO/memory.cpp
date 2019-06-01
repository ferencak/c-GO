#include "pch.h"
#include "memory.h"
#include <windows.h>
#include <Tlhelp32.h> 
#include <stdio.h>

memory::memory()
{
}


memory::~memory()
{
}

int memory::getProcID(const std::wstring& pName)
{
	PROCESSENTRY32 pInfo;
	pInfo.dwSize = sizeof(pInfo);

	HANDLE pSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (pSnapshot == INVALID_HANDLE_VALUE) {
		return 0;
	}

	Process32First(pSnapshot, &pInfo);
	if (!pName.compare(pInfo.szExeFile))
	{
		CloseHandle(pSnapshot);
		return pInfo.th32ProcessID;
	}

	while (Process32Next(pSnapshot, &pInfo))
	{
		if (!pName.compare(pInfo.szExeFile))
		{
			CloseHandle(pSnapshot);
			return pInfo.th32ProcessID;
		}
	}

	CloseHandle(pSnapshot);
	return 0;
}

DWORD memory::read(HANDLE handle, LPCVOID address) {
	DWORD value;
	ReadProcessMemory(handle, address, &value, sizeof(value), NULL);
	return value;
}

void memory::writeb(HANDLE handle, LPVOID address, BOOL value) {
	WriteProcessMemory(handle, address, &value, sizeof(value), NULL);
}

void memory::writef(HANDLE handle, LPVOID address, float value) {
	WriteProcessMemory(handle, address, &value, sizeof(value), NULL);
}