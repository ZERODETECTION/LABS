// x86_64-w64-mingw32-gcc getDomainname.c -o dname.exe

#include <stdio.h>
#include <windows.h>

#define MAX_DOMAIN_NAME_LENGTH 256

int main() {
    char domainName[MAX_DOMAIN_NAME_LENGTH + 1];
    DWORD size = MAX_DOMAIN_NAME_LENGTH + 1;

    // Ruft die DNS-Domäne des Computers ab
    if (GetComputerNameEx(ComputerNameDnsDomain, domainName, &size)) {
        printf("Die DNS-Domäne des Computers ist: %s\n", domainName);
    } else {
        perror("Fehler beim Abrufen der DNS-Domäne des Computers");
        return 1;
    }

    return 0;
}
