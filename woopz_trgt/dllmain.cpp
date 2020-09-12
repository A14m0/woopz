// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <memoryapi.h>
#include <heapapi.h>

void do_chad_crash() {
    HANDLE hHeap;
    HANDLE hProc;

    // gets address of process heap
    if ((hHeap = GetProcessHeap()) == NULL) {
        printf("Failed to get heap handle :/ (%d)\n", GetLastError());
        return;
    }
    
    // gets a process handle
    hProc = GetCurrentProcess();

    // gets current page protections
    MEMORY_BASIC_INFORMATION old_info;
    if (!VirtualQueryEx(hProc, hHeap, &old_info, sizeof(old_info))) {
        printf("Failed to query current heap info: %d\n", GetLastError());
        return;
    }

    // set the page protections of the regions
    if (!VirtualProtect(hHeap, 0x1000, PAGE_NOACCESS, &(old_info.AllocationProtect))) {
        printf("Failed to set page protections: %d (address %p)\n", GetLastError(), hHeap);
        return;
    }

    // profit :)
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    // want to allocate a console just in case they havent already
    AllocConsole();

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        printf("(proc_attach)\n");
        do_chad_crash();
        break;
    case DLL_THREAD_ATTACH:
        printf("(thread_attach)\n");
        //do_chad_crash();
        break;
    case DLL_THREAD_DETACH:
        printf("(thread_detach)\n");
        break;
    case DLL_PROCESS_DETACH:
        printf("(proc_detach)\n");
        break;
    }
    return TRUE;
}

