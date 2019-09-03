#include "get_parent_id.h"

#ifdef _WIN32
#include    <windows.h>
#include    <tlhelp32.h>
#include    <stdio.h>

size_t get_parent_id()
{
    HANDLE snapshot_handle;
    PROCESSENTRY32 pe32;
    DWORD pid = GetCurrentProcessId();
    DWORD ppid = 0;

    snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    __try
    {
        if(snapshot_handle == INVALID_HANDLE_VALUE)
        {
            __leave;
        }

        ZeroMemory(&pe32, sizeof(pe32));

        pe32.dwSize = sizeof(pe32);

        if(!Process32First(snapshot_handle, &pe32))
        {
            __leave;
        }

        do
        {
            if(pe32.th32ProcessID == pid)
            {
                ppid = pe32.th32ParentProcessID;
                break;
            }
        }
        while(Process32Next(snapshot_handle, &pe32));

    }
    __finally
    {
        if(snapshot_handle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(snapshot_handle);
        }
    }

    return ppid;
}
#elif __linux__
#include <unistd.h>

size_t get_parent_id()
{
    return getppid();
}
#else
#error "Platform not supported"
#endif
