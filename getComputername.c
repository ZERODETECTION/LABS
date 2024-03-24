// x86_64-w64-mingw32-gcc getComputername.c -o cname.exe

#include <stdio.h>
#include <windows.h>

int main() {
    char hostname[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(hostname);

    // Ruft den Computernamen ab
    if (GetComputerName(hostname, &size)) {
        printf("Der Computernamen ist: %s\n", hostname);
    } else {
        perror("Fehler beim Abrufen des Computernamens");
        return 1;
    }

    return 0;
}
