#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>

int main() {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        _tprintf(_T("something happaned while trying to create process snapshot. Error %lu\n"), GetLastError());
        return 1;
    }

    atexit((void(*)())CloseHandle);

    const TCHAR* targetProcess = _T("notepad.exe");
    DWORD targetProcessId = 0;

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hProcessSnap, &pe32)) {
        do {
            TCHAR processName[MAX_PATH];
            _tcscpy_s(processName, MAX_PATH, pe32.szExeFile);
            std::wstring processNameStr(processName);

            if (_tcscmp(targetProcess, processName) == 0) {
                targetProcessId = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    _tprintf(_T("[process id identified!]\n\\___[process name: %s]\n"), targetProcess);
    _tprintf(_T("\t\\___[process id: %lu]\n"), targetProcessId);

    return 0;
}
