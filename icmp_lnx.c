// gcc icmp.c -o icmp
// chmod 777 icmp
// ./icmp <ip>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

#define PACKET_SIZE 64
#define ICMP_ECHO_REQUEST 8

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
    struct icmp *icmp_header = (struct icmp *)packet;
    icmp_header->icmp_type = ICMP_ECHO_REQUEST;
    icmp_header->icmp_code = 0;
    icmp_header->icmp_id = getpid();
    icmp_header->icmp_seq = sequence;
    icmp_header->icmp_cksum = 0;
    icmp_header->icmp_cksum = checksum(icmp_header, PACKET_SIZE - sizeof(struct ip));
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Verwendung: %s <Ziel-IP-Adresse>\n", argv[0]);
        return 1;
    }

    char packet[PACKET_SIZE];
    struct sockaddr_in dest_addr;
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (sockfd < 0) {
        perror("Fehler beim Erstellen des Sockets");
        return 1;
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &(dest_addr.sin_addr));

    int sequence = 0;
    while (1) {
        // ICMP-Paket erstellen
        craft_icmp_packet(packet, sequence);

        // ICMP-Paket an das Ziel senden
        if (sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) <= 0) {
            perror("Fehler beim Senden des ICMP-Pakets");
            return 1;
        }

        printf("Ping gesendet an %s\n", argv[1]);
        sequence++;
        sleep(1); // Wartezeit zwischen den Ping-Paketen
    }

    return 0;
}
