#include <windows.h>
#include <stdio.h>

struct PROC_THREAD_ATTRIBUTE_ENTRY {
    DWORD_PTR Attribute;
    SIZE_T cbSize;
    PVOID lpValue;
};

struct PROC_THREAD_ATTRIBUTE_LIST {
    DWORD dwFlags;
    ULONG Size;
    ULONG Count;
    ULONG Reserved;
    PULONG Unknown;
    PROC_THREAD_ATTRIBUTE_ENTRY Entries[10];
};

typedef PROC_THREAD_ATTRIBUTE_LIST* LPPROC_THREAD_ATTRIBUTE_LIST;

struct STARTUPINFOEXA {
    STARTUPINFOA StartupInfo;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
};

extern BOOL InitializeProcThreadAttributeList(
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
    DWORD dwAttributeCount,
    DWORD dwFlags,
    PSIZE_T lpSize
);

extern VOID DeleteProcThreadAttributeList(
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList
);

extern BOOL UpdateProcThreadAttribute(
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
    DWORD dwFlags,
    DWORD_PTR Attribute,
    PVOID lpValue,
    SIZE_T cbSize,
    PVOID lpPreviousValue,
    PSIZE_T lpReturnSize
);

#define PROC_THREAD_ATTRIBUTE_PARENT_PROCESS 0x00020000
#define EXTENDED_STARTUPINFO_PRESENT 0x00080000

void cleanup(LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList, HANDLE parentPhandle) {
    if (lpAttributeList != NULL) {
        DeleteProcThreadAttributeList(lpAttributeList);
        HeapFree(GetProcessHeap(), 0, lpAttributeList);
    }

    if (parentPhandle != NULL) {
        CloseHandle(parentPhandle);
    }
}

void spawnProcess() {
    STARTUPINFOEXA si;
    PROCESS_INFORMATION pi;
    SIZE_T attributeSize;

    INT pid = 1040; // change this to the PID you want to spoof.

    HANDLE parentPhandle = OpenProcess(MAXIMUM_ALLOWED, FALSE, pid);
    if (parentPhandle == NULL) {
        fprintf(stderr, "Failed to open the parent process. Error: %lu\n", GetLastError());
        return;
    }

    ZeroMemory(&si, sizeof(si));

    if (!InitializeProcThreadAttributeList(NULL, 1, 0, &attributeSize)) {
        fprintf(stderr, "Failed to initialize attribute list. Error: %lu\n", GetLastError());
        cleanup(NULL, parentPhandle);
        return;
    }

    si.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, attributeSize);
    if (si.lpAttributeList == NULL) {
        fprintf(stderr, "Failed to allocate memory for attribute list\n");
        cleanup(NULL, parentPhandle);
        return;
    }

    if (!InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &attributeSize)) {
        fprintf(stderr, "Failed to initialize attribute list. Error: %lu\n", GetLastError());
        cleanup(si.lpAttributeList, parentPhandle);
        return;
    }

    if (!UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &parentPhandle, sizeof(HANDLE), NULL, NULL)) {
        fprintf(stderr, "Failed to update attribute list. Error: %lu\n", GetLastError());
        cleanup(si.lpAttributeList, parentPhandle);
        return;
    }

    si.StartupInfo.cb = sizeof(si);

    if (!CreateProcessA("C:\\Windows\\System32\\notepad.exe", NULL, NULL, NULL, FALSE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, &si.StartupInfo, &pi)) {
        fprintf(stderr, "Failed to create process. Error: %lu\n", GetLastError());
        cleanup(si.lpAttributeList, parentPhandle);
        return;
    }

    cleanup(si.lpAttributeList, parentPhandle);
}

int main() {
    spawnProcess();
    return 0;
}
