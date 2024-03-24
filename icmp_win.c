// x86_64-w64-mingw32-gcc icmp_win.c -o icmp_win.exe -lws2_32

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PACKET_SIZE 64
#define REPLY_BUFFER_SIZE 1024
#define ICMP_ECHO_REQUEST 8
#define ICMP_ECHO_REPLY 0

#pragma comment(lib, "ws2_32.lib")

// IP-Header-Struktur
typedef struct iphdr {
    unsigned char  ip_hl:4; // Headerlänge in 32-Bit-Wörtern
    unsigned char  ip_v:4;  // IP-Version
    unsigned char  ip_tos;  // Type of Service
    unsigned short ip_len;  // Gesamtlänge
    unsigned short ip_id;   // Identifikation
    unsigned short ip_off;  // Fragmentversatz
    unsigned char  ip_ttl;  // Time To Live
    unsigned char  ip_p;    // Protokoll
    unsigned short ip_sum;  // Headerprüfsumme
    unsigned int   ip_src;  // Quell-IP-Adresse
    unsigned int   ip_dst;  // Ziel-IP-Adresse
} IP_HEADER;

// ICMP-Header-Struktur
typedef struct icmphdr {
    unsigned char  type;     // Nachrichtentyp
    unsigned char  code;     // Nachrichtencode
    unsigned short checksum; // Prüfsumme
    unsigned short id;       // Identifikation
    unsigned short seq;      // Sequenznummer
} ICMP_HEADER;

// IP-Header-Checksumme berechnen
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

// ICMP-Paket erstellen
void craft_icmp_packet(char *packet, int sequence) {
    ICMP_HEADER *icmp_header = (ICMP_HEADER *)packet;
    icmp_header->type = ICMP_ECHO_REQUEST;
    icmp_header->code = 0;
    icmp_header->checksum = 0;
    icmp_header->id = (unsigned short)GetCurrentProcessId();
    icmp_header->seq = sequence;
    icmp_header->checksum = checksum(icmp_header, PACKET_SIZE);
}

// ICMP-Antwort verarbeiten
void process_icmp_reply(char *reply, int size) {
    ICMP_HEADER *icmp_header = (ICMP_HEADER *)reply;
    if (icmp_header->type == ICMP_ECHO_REPLY) {
        printf("Ping-Antwort empfangen:\n");
        printf("  Typ: %d\n", icmp_header->type);
        printf("  Code: %d\n", icmp_header->code);
        printf("  ID: %d\n", icmp_header->id);
        printf("  Sequenz: %d\n", icmp_header->seq);
        // Weitere Felder können hier je nach Bedarf ausgegeben werden
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Verwendung: %s <Ziel-IP-Adresse>\n", argv[0]);
        return 1;
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Fehler beim Initialisieren von Winsock\n");
        return 1;
    }

    char packet[PACKET_SIZE];
    char reply[REPLY_BUFFER_SIZE]; // ICMP-Antwort-Puffer
    struct sockaddr_in dest_addr;
    SOCKET sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (sockfd == INVALID_SOCKET) {
        printf("Fehler beim Erstellen des Sockets: %d\n", WSAGetLastError());
        return 1;
    }

    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &(dest_addr.sin_addr));

    int sequence = 0;
    while (1) {
        // ICMP-Paket erstellen
        craft_icmp_packet(packet, sequence);

        // ICMP-Paket an das Ziel senden
        if (sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == SOCKET_ERROR) {
            printf("Fehler beim Senden des ICMP-Pakets: %d\n", WSAGetLastError());
            return 1;
        }

        printf("Ping gesendet an %s\n", argv[1]);

        // ICMP-Antwort empfangen
        int addr_len = sizeof(dest_addr);
        int bytes_received = recvfrom(sockfd, reply, REPLY_BUFFER_SIZE, 0, (struct sockaddr *)&dest_addr, &addr_len);
        if (bytes_received == SOCKET_ERROR) {
            printf("Fehler beim Empfangen der ICMP-Antwort: %d\n", WSAGetLastError());
            return 1;
        }
        else if (bytes_received > 0) {
            printf("Empfangene ICMP-Antwort (%d Bytes):\n", bytes_received);
            process_icmp_reply(reply, bytes_received);
        }

        sequence++;
        Sleep(1000); // Wartezeit zwischen den Ping-Paketen in Millisekunden
    }

    closesocket(sockfd);
    WSACleanup();

    return 0;
}
