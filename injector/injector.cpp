// injector.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <string>

// TODO:
// - get the PID of the game process x
// - spawn a thread in the game process
// - load the get_rekt.dll file into the game process from the thread
// - export the binary

DWORD PrintProcessNameAndID( DWORD processID, TCHAR * processName)
{
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    // Get a handle to the process.
    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, processID );

    // Get the process name.
    if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
             &cbNeeded) )
        {
            GetModuleBaseName(hProcess, hMod, szProcessName,
                               sizeof(szProcessName)/sizeof(TCHAR));
        }
    }

	if (wcscmp(szProcessName, processName) == 0) {
		CloseHandle(hProcess);
		return processID;
	}

    // Release the handle to the process.
	CloseHandle(hProcess);

	return -1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	 // Get the list of process identifiers.

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        return 1;
    }


    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.
	int pID = -1;

    for ( i = 0; i < cProcesses && pID == -1; i++ )
    {
        if( aProcesses[i] != 0 )
        {
			pID = PrintProcessNameAndID( aProcesses[i], TEXT("dummy_process.exe"));
        }
    }

	printf("%d\n", pID);

	// Open the target process
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS , FALSE, pID);

	if (hProcess == NULL) {
		printf("Error opening the target.");
		return -1;
	}

	// Get the address of the DLL which contains the LoadLibraryA function
	// This function is used to call our own dll inside of the target process
	LPVOID addr = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
	
	if(addr == NULL) {
		printf("Error getting the address of the LoadLibraryA process.");
	}

	// TODO: change this to point to the final location
	char* buffer = "./get_rekt.dll";
	
	// Allocate memory for the argument of the LoadLibraryA function in the heap of the target process
	LPVOID arg = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(buffer), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	if(arg == NULL) {
		printf("Error allocating the memory for the argument of the library injection");

		return -1;
	}

	// write the argument of the LoadLibraryA function in the heap of the target process
	int bytesWritten = WriteProcessMemory(hProcess, arg, buffer, strlen(buffer), NULL);
	
	if(bytesWritten == 0) {
		printf("Error writing bytes to the process’s address space");

		return -1;
	}

	// Initiate the thread, the beginning address of which is the LoadLibraryA function, in the target process
	HANDLE threadHandle = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE) addr, NULL, NULL, NULL);

	if (threadHandle == NULL) {
		printf("Error creating the remote thread.");

		return -1;
	}

	printf("Successfully injected the DLL.");

	char end;
	scanf("%c", &end);

	return 0;
}

