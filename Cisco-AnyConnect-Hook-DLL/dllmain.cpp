// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "pch.h"
#include <windows.h>
#include <stringapiset.h>
#include <detours/detours.h>
#include <fstream>
#include <atlstr.h>
#include <string>
#include <WinBase.h>
#define SECURITY_WIN32

std::wofstream myfile;


// Target pointer for the uninstrumented lstrlenW API.
static int(WINAPI* OriginallstrlenW)(LPCWSTR lpString) = lstrlenW;

// Target pointer for the instrumented lstrlenW API.
int WINAPI WriteStringsToFile(LPCWSTR lpString)
{
  
    myfile << lpString;
    
    return OriginallstrlenW(lpString);
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    if (DetourIsHelperProcess()) {
        return TRUE;
    }

    if (dwReason == DLL_PROCESS_ATTACH) {
        myfile.open("C:\\Users\\<user>\\AppData\\Local\\Temp\\VPN-DLL.txt");
        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)OriginallstrlenW, WriteStringsToFile);
        DetourTransactionCommit();
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        myfile.close();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)OriginallstrlenW, WriteStringsToFile);
        DetourTransactionCommit();
    }
    return TRUE;
}

