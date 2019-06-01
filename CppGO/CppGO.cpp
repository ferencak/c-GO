#include "pch.h"
#include "memory.h"
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <tlhelp32.h>

memory mem;
DWORD pId;

#define pName L"csgo.exe"
#define cpDll L"client_panorama.dll"
#define cDll L"client.dll"
#define sDll L"server.dll"

struct Offsets {
	DWORD dwLocalPlayer = 0xCF1A4C;
	DWORD dwEntityList = 0x4D03AA4;
	DWORD dwGlowObjectManager = 0x5243FD0;
	DWORD m_bDormant = 0xED;
	DWORD m_iGlowIndex = 0xA40C;
} o;

struct Modules {
	DWORD cpModule;
	DWORD cModule;
	DWORD sModule;
} h;

struct Player {
	DWORD dwlocal;
	DWORD dwglow;
	int dwteam;
	int dweteam;
	int dwGlowIndex;
	DWORD dwentry;
	DWORD dwentity;
	DWORD dwplayer;
	BOOL dwdormat;
} p;

struct Game {
	DWORD dwplayerSize;
} g;

uintptr_t pGetModuleAddr(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hs = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hs != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 mE;
		mE.dwSize = sizeof(mE);
		if (Module32First(hs, &mE))
		{
			do
			{
				if (!_wcsicmp(mE.szModule, modName))
				{
					modBaseAddr = (uintptr_t)mE.modBaseAddr;
					break;
				}
			} while (Module32Next(hs, &mE));
		}
	}
	CloseHandle(hs);
	return modBaseAddr;
}


int main()
{

	float GlowTerroristRed = 1.f;
	float GlowTerroristGreen = 0.f;
	float GlowTerroristBlue = 0.f;
	float GlowTerroristAlpha = 0.6f;

	float GlowCounterTerroristRed = 0.f;
	float GlowCounterTerroristGreen = 1.f;
	float GlowCounterTerroristBlue = 1.f;
	float GlowCounterTerroristAlpha = 0.6f;

	while (mem.getProcID(pName) == 0) {
		std::cout << "Waiting for csgo.exe..." << std::endl;
		Sleep(1000);
	}

	std::cout << "Process was founded!" << std::endl;

	pId = mem.getProcID(L"csgo.exe");
	HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pId);
	std::cout << "Process was handled!" << std::endl;
	h.cpModule = pGetModuleAddr(pId, cpDll);

	if (h.cpModule == NULL) {
		do {
			std::cout << "Cannot get client_panorama module..." << std::endl;
			h.cpModule = pGetModuleAddr(pId, cpDll);
			Sleep(1);
		} while (h.cpModule == NULL);
	}

	for(; ;) {
		p.dwlocal = mem.read(pHandle, (LPCVOID)(h.cpModule + o.dwLocalPlayer));
		p.dwglow = mem.read(pHandle, (LPCVOID)(h.cpModule + o.dwGlowObjectManager));
		p.dwteam = mem.read(pHandle, (LPCVOID)(p.dwlocal + 0xF4));
		for (int i = 0; i < 32; i++) {
			p.dwplayer = mem.read(pHandle, (LPCVOID)(h.cpModule + o.dwEntityList + i * 0x10));
			p.dweteam = mem.read(pHandle, (LPCVOID)(p.dwplayer + 0xF4));			
			p.dwdormat = mem.read(pHandle, (LPCVOID)(p.dwplayer + o.m_bDormant));
			p.dwGlowIndex = mem.read(pHandle, (LPCVOID)(p.dwplayer + o.m_iGlowIndex));
			if(p.dwdormat != 1) {
				if (p.dwteam != p.dweteam) {
					switch (p.dweteam) {
						case 2: 
							mem.writef(pHandle, (LPVOID)((p.dwglow + ((p.dwGlowIndex * 0x38) + 0x4))), GlowTerroristRed);
							mem.writef(pHandle, (LPVOID)((p.dwglow + ((p.dwGlowIndex * 0x38) + 0x8))), GlowTerroristGreen);
							mem.writef(pHandle, (LPVOID)((p.dwglow + ((p.dwGlowIndex * 0x38) + 0xC))), GlowTerroristBlue);
							mem.writef(pHandle, (LPVOID)((p.dwglow + ((p.dwGlowIndex * 0x38) + 0x10))), GlowTerroristAlpha);
							mem.writeb(pHandle, (LPVOID)((p.dwglow + ((p.dwGlowIndex * 0x38) + 0x24))), true);
							mem.writeb(pHandle, (LPVOID)((p.dwglow + ((p.dwGlowIndex * 0x38) + 0x25))), false);
							break;
						case 3: 
							mem.writef(pHandle, (LPVOID)((p.dwglow + ((p.dwGlowIndex * 0x38) + 0x4))), GlowCounterTerroristRed);
							mem.writef(pHandle, (LPVOID)((p.dwglow + ((p.dwGlowIndex * 0x38) + 0x8))), GlowCounterTerroristGreen);
							mem.writef(pHandle, (LPVOID)((p.dwglow + ((p.dwGlowIndex * 0x38) + 0xC))), GlowCounterTerroristBlue);
							mem.writef(pHandle, (LPVOID)((p.dwglow + ((p.dwGlowIndex * 0x38) + 0x10))), GlowCounterTerroristAlpha);
							mem.writeb(pHandle, (LPVOID)((p.dwglow + ((p.dwGlowIndex * 0x38) + 0x24))), true);
							mem.writeb(pHandle, (LPVOID)((p.dwglow + ((p.dwGlowIndex * 0x38) + 0x25))), false);
							break;
					}
				}
			}
		}
	}
	
	return 0;
}