// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <Psapi.h>

DWORD thread_id;

BYTE * JumpAddress;
BYTE * PlayerInAir;

WNDPROC OriginalWindowProc;

bool BHopToggle;
bool Disable;

#pragma comment(lib, "psapi.lib")
#pragma comment( lib, "user32.lib")

LRESULT CALLBACK TrackKeyPresses(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam) {
	printf("Key pressed: %d\n", wParam);
	printf("Disable cheat: %d\n", Disable);

	// the keys we're looking for are 'L' = 0x4c || 0x6c
	// or 'H' = 0x49 || 0x68

	if (Message == WM_KEYDOWN) {
		// on 'L', toggle the cheat
		if (wParam == 0x4c || wParam == 0x6c) {
			BHopToggle = !BHopToggle;
		}

		// on 'H', stop the cheat
		if (wParam == 0x48 || wParam == 0x68) {
			Disable = true;
		}
	}
	
	// logic for the BHop machine toggling
	if (BHopToggle) {
		if (* PlayerInAir) {
			*JumpAddress = 0;
		}
		else {
			*JumpAddress = 5;
		}
	}

	// return back to the context of the hooked procedure
	return CallWindowProcA(OriginalWindowProc, Window, Message, wParam, lParam);
}

VOID WINAPI Play(LPVOID Module) {
	BYTE * client = NULL;
	BYTE * engine = NULL;

	// get the base addresses of client.dll and engine.dll
	while (client == NULL && engine == NULL) {
		client = (BYTE *) GetModuleHandle(L"client.dll");
		engine = (BYTE *) GetModuleHandle(L"engine.dll");
	}

	BHopToggle = false;
	Disable = false;

	// jump offset
	JumpAddress = client + 0x4F5D24;
	// player in air state offset
	PlayerInAir = client + 0x4A4078;

	// get the main game window
	HWND GameWindow = FindWindowA("Valve001", 0);

	// set the hook for the main window
	OriginalWindowProc = (WNDPROC)SetWindowLongPtr(GameWindow, GWLP_WNDPROC, (LONG_PTR)TrackKeyPresses);

	while (!Disable) {
		Sleep(1000);
	}

	// uninject the DLL
	SetWindowLongPtr(GameWindow, GWLP_WNDPROC, (LONG_PTR)OriginalWindowProc);
	FreeLibraryAndExitThread((HMODULE)Module, 0);
}
 
void LoopThread() {
	// create a thread to do the heavy work
	HANDLE LoopThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Play, NULL, NULL, &thread_id);

	if (LoopThread != NULL) {
		CloseHandle(LoopThread);
	}
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		LoopThread();
	}

	return TRUE;
}
