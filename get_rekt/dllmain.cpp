// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>

// TODO:
// - create the hook
// - locate the physics component of the engine
// - call the hook on each engine tick
// - export the binary

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	HANDLE hFile = CreateFile(L"./f.txt",
                       GENERIC_WRITE,
                       0,
                       NULL,
                       CREATE_NEW,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

	if (hFile == INVALID_HANDLE_VALUE) 
    { 
        printf("%d", GetLastError());
        return FALSE;
    }


	char * DataBuffer = "works!";
	DWORD bytesWritten = 0;

	BOOL bErrorFlag = WriteFile(hFile, DataBuffer, strlen(DataBuffer), &bytesWritten, NULL);

	if (FALSE == bErrorFlag)
    {
        printf("Error writing to the file.\n");

		return FALSE;
    }

	CloseHandle(hFile);
	
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

