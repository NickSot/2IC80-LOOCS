// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <Psapi.h>

DWORD thread_id;

BYTE * JumpAddress;
BYTE * PlayerInAir;

#pragma comment(lib, "psapi.lib")

VOID WINAPI Play(LPVOID Module) {
	if (* PlayerInAir) {
		*JumpAddress = 0;
	}
	else {
		*JumpAddress = 5;
	}

	// CALL THIS FUNCTION HERE ALWAYS!!!
	FreeLibraryAndExitThread((HMODULE)Module, 0);
}
 
void LoopThread() {
	HANDLE LoopThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Play, NULL, NULL, &thread_id);

	if (LoopThread != NULL) {
		CloseHandle(LoopThread);
	}
}

void Detatch() {
	HANDLE LoopThread = OpenThread(THREAD_TERMINATE, FALSE, thread_id);

	if (LoopThread != NULL) {
		CloseHandle(LoopThread);
	}
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	BYTE * client = NULL;
	BYTE * engine = NULL;

	// get the base addresses of client.dll and engine.dll
	while (client == NULL && engine == NULL) {
		client = (BYTE *) GetModuleHandle(L"client.dll");
		engine = (BYTE *) GetModuleHandle(L"engine.dll");
	}

	// jump offset
	JumpAddress = client + 0x4F5D24;
	// player in air state offset
	PlayerInAir = client + 0x4A4078;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		LoopThread();
		break;

	case DLL_THREAD_DETACH:
		Detatch();
		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}
