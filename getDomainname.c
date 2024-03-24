// x86_64-w64-mingw32-gcc getDomainname.c -o dname.exe

#include <stdio.h>
#include <windows.h>

#define MAX_DOMAIN_NAME_LENGTH 256

int main() {
    char domainName[MAX_DOMAIN_NAME_LENGTH + 1];
    DWORD size = MAX_DOMAIN_NAME_LENGTH + 1;

    // GET DOMAIN
    if (GetComputerNameEx(ComputerNameDnsDomain, domainName, &size)) {
        printf("Domain: %s\n", domainName);
    } else {
        perror("Error");
        return 1;
    }

    return 0;
}
