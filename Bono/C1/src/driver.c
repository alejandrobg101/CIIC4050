// src/alarm_driver.c
#define _DEFAULT_SOURCE
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <interval_ms>\n", argv[0]);
        return 1;
    }

    char *end = NULL;
    long interval_ms = strtol(argv[1], &end, 10);
    if (!argv[1][0] || *end || interval_ms <= 0) {
        fprintf(stderr, "Invalid interval: %s\n", argv[1]);
        return 1;
    }

    pid_t child = fork();
    if (child < 0) {
        perror("fork");
        return 1;
    }

    if (child == 0) {
        // Launch the provided timer plotter (the existing executable built from main.c)
        execlp("./time_plotter", "time_plotter", (char *)NULL);
        perror("execlp");
        _exit(127);
    }

    useconds_t interval_us = (useconds_t)(interval_ms * 1000);
    for (;;) {
        usleep(interval_us);
        if (kill(child, SIGALRM) != 0) {
            perror("kill");
            break;
        }
    }
    return 0;
}