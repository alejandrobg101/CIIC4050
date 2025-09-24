#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

volatile sig_atomic_t keep_running = 1;

void Sigusr1_handler(int sig) {
    pid_t pid = getpid();
    char filename[256];
    snprintf(filename, sizeof(filename), "worker_log_%d.txt", pid);
    FILE *log_file = fopen(filename, "a");
    if (log_file == NULL) {
        fprintf(stderr, "Error opening log file %s: %s\n",
            filename, strerror(errno));
        return;
    }
    fprintf(log_file, "SIGUSR1 received\n");
    fclose(log_file);
}

void Sigusr2_handler(int sig) {
    pid_t pid = getpid();
    char filename[256];
    snprintf(filename, sizeof(filename), "worker_log_%d.txt", pid);
    FILE *log_file = fopen(filename, "a");
    if (log_file == NULL) {
        fprintf(stderr, "Error opening log file %s: %s\n",
            filename, strerror(errno));
        return;
    }
    fprintf(log_file, "SIGUSR2 received\n");
    fclose(log_file);
}

void Sigterm_handler(int sig) {
    keep_running = 0;
}

int main() {
    struct sigaction sa_usr1, sa_usr2, sa_term;
    sa_usr1.sa_handler = Sigusr1_handler;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }
    sa_usr2.sa_handler = Sigusr2_handler;
    sigemptyset(&sa_usr2.sa_mask);
    sa_usr2.sa_flags = 0;
    if (sigaction(SIGUSR2, &sa_usr2, NULL) == -1) {
        perror("sigaction SIGUSR2");
        exit(EXIT_FAILURE);
    }
    sa_term.sa_handler = Sigterm_handler;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("sigaction SIGTERM");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Worker started with PID: %d\n", getpid());
    while (keep_running) {
        pause();
    }
    fprintf(stderr, "Worker %d shutting down\n", getpid());
    return EXIT_SUCCESS;
}
