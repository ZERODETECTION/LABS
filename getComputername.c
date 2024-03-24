// x86_64-w64-mingw32-gcc getComputername.c -o cname.exe

#include <stdio.h>
#include <windows.h>

int main() {
    char hostname[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(hostname);

    // GET COMPUTERNAME
    if (GetComputerName(hostname, &size)) {
        printf("ComputerName: %s\n", hostname);
    } else {
        perror("Error");
        return 1;
    }

    return 0;
}
