#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#define MAX_LINE_LENGTH 256
#define MAX_COMMANDS 100

typedef struct {
    char worker_name[16];
    char signal_name[16];
    int signal_number;
    pid_t worker_pid;
} Command;

int Parse_signal(const char *signal_name) {
    if (strcmp(signal_name, "SIGUSR1") == 0) {
        return SIGUSR1;
    } else if (strcmp(signal_name, "SIGUSR2") == 0) {
        return SIGUSR2;
    } else {
        return -1;
    }
}

void Send_signal_to_worker(pid_t worker_pid,
    int signal_num, const char *signal_name) {
    if (kill(worker_pid, signal_num) == -1) {
        fprintf(stderr, "Error sending %s to worker %d: %s\n",
                signal_name, worker_pid, strerror(errno));
        return;
    }
    fprintf(stderr, "Sent %s to worker %d\n", signal_name, worker_pid);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <worker1_pid> <worker2_pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    pid_t worker1_pid = atoi(argv[1]);
    pid_t worker2_pid = atoi(argv[2]);
    if (worker1_pid <= 0 || worker2_pid <= 0) {
        fprintf(stderr, "Error: Invalid worker PIDs\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Manager started with worker1 PID: %d, worker2 PID: %d\n",
            worker1_pid, worker2_pid);
    FILE *commands_file = fopen("commands.txt", "r");
    if (commands_file == NULL) {
        perror("Error opening commands.txt");
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];
    int command_count = 0;
    while (fgets(line, sizeof(line), commands_file) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) {
            continue;
        }
        char worker_name[16], signal_name[16];
        if (sscanf(line, "%15s %15s", worker_name, signal_name) != 2) {
            fprintf(stderr, "Error parsing line: %s\n", line);
            continue;
        }
        pid_t target_pid;
        if (strcmp(worker_name, "worker1") == 0) {
            target_pid = worker1_pid;
        } else if (strcmp(worker_name, "worker2") == 0) {
            target_pid = worker2_pid;
        } else {
            fprintf(stderr, "Error: Unknown worker '%s'\n", worker_name);
            continue;
        }
        int signal_num = Parse_signal(signal_name);
        if (signal_num == -1) {
            fprintf(stderr, "Error: Unknown signal '%s'\n", signal_name);
            continue;
        }
        sleep(1);
        Send_signal_to_worker(target_pid, signal_num, signal_name);
        command_count++;
    }
    fclose(commands_file);
    fprintf(stderr, "Manager processed %d commands and is shutting down\n",
        command_count);
    fprintf(stderr, "Terminating workers...\n");
    kill(worker1_pid, SIGTERM);
    kill(worker2_pid, SIGTERM);
    return EXIT_SUCCESS;
}
