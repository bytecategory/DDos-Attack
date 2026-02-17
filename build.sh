gcc -pthread -o ddos_attack_multithread ddos_attack_multithread.c
gcc monitor_pps.c -o monitor_pps -lpcap -lpthread
gcc -o ddos-attack ddos-attack.c
