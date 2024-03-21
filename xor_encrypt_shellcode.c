// gcc xor_encrypt_shellcode.c -o xor_enc
// chmod 777 xor_enc
// msfvenom -p windows/x64/messagebox TEXT=zerodetection TITLE=zerodetection -f raw -o messagebox.raw
// ./xor_enc messagebox.raw

#include <stdio.h>
#include <stdlib.h>

#define MAX_BUF_SIZE 4096
#define XOR_KEY 0xAB // XOR-KEY

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error");
        return 1;
    }

    unsigned char buf[MAX_BUF_SIZE];
    size_t buf_size = 0;

    // READ SHELLCODE
    int c;
    while ((c = fgetc(file)) != EOF && buf_size < MAX_BUF_SIZE) {
        buf[buf_size++] = c;
    }
    fclose(file);

    // XOR-ENC
    for (size_t i = 0; i < buf_size; i++) {
        buf[i] ^= XOR_KEY;
    }

    // OUTPUT as C-Array
    printf("unsigned char buf[] = \n");
    printf("\"");
    for (size_t i = 0; i < buf_size; i++) {
        printf("\\x%02X", buf[i]);
    }
    printf("\";\n");

    return 0;
}
