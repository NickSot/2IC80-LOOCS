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

	char end;
	scanf("%c", &end);

	return 0;
}

