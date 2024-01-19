#include <windows.h>
#include <stdio.h>

void errorExit(const char* message) {
    fprintf(stderr, "%s. Error: %lu\n", message, GetLastError());
    ExitProcess(1);
}

int main() {
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi;

    if (!CreateProcessA(
        "C:\\Windows\\System32\\notepad.exe",
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    )) {
        errorExit("Failed to create process");
    }

    printf("[Process started: (%lu)]\n", pi.dwProcessId);
    printf("[Handle to process: 0x%p]\n", pi.hProcess);

    WaitForSingleObject(pi.hProcess, INFINITE);
    printf("Execution finished. Exiting...\n");

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 0;
}
