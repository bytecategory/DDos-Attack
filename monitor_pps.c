#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
typedef unsigned char u_char;
volatile long pps_counter = 0;
void* print_pps(void* arg) {
    char time_str[64];
    time_t now;
    struct tm *tm_info;
    while (1) {
        sleep(10);
        time(&now);
        tm_info = localtime(&now);
        strftime(time_str, sizeof(time_str), "%a %b %d %I:%M:%S %p %Z %Y", tm_info);

        fprintf(stdout, "[%s] %.1f PPS\n", time_str, pps_counter / 10.0);
        pps_counter = 0;
    }
    return NULL;
}
void packet_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    pps_counter++;
}
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <DEV> <BPF>\n", argv[0]);
        return 1;
    }
    char *dev = argv[1]; 
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    struct bpf_program fp;
    char *filter_exp = argv[2]; 
    handle = pcap_open_live(dev, 65535, 1, 1, errbuf); 
    if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        fprintf(stderr, "Unable to resolve filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return 2;
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Unable to install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return 2;
    }


    pthread_t thread_id;
    pthread_create(&thread_id, NULL, print_pps, NULL);
    pthread_detach(thread_id);


    printf("Listen on %s using %s\n", dev, filter_exp);
    pcap_loop(handle, 0, packet_handler, NULL);

    pcap_close(handle);
    return 0;
}


