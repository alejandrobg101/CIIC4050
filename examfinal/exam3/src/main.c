#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#define BUFFER_SIZE 256
static int target_pid;

/**
 * @brief Extracts the first floating-point number found in a text string.
 * @param text The input string (e.g., "vruntime: 12345.678").
 * @return The extracted floating-point number.
 */
double GetNumberFromText(const char* text) {
  double number = 0.0;
  // Searches for the first occurrence of a number pattern (signed, decimal,
  // optional exponent)
  if (sscanf(text, "%*[^0-9-]%lf", &number) == 1) {
    return number;
  }
  return 0.0;
}

/* SIGALRM handler */
void alarm_handler(int sig) {
    (void)sig;  // unused

    char path[BUFFER_SIZE];
    char line[BUFFER_SIZE];
    FILE *sched_file = NULL;
    FILE *out_file = NULL;
    double vruntime = 0.0;

    /* Build /proc/<PID>/sched path */
    snprintf(path, BUFFER_SIZE, "/proc/%d/sched", target_pid);

    sched_file = fopen(path, "r");
    if (!sched_file) {
        perror("Failed to open sched file");
        _exit(1);
    }

    /* Read until the 4th line */
    for (int i = 1; i <= 4; i++) {
        if (!fgets(line, BUFFER_SIZE, sched_file)) {
            perror("Failed to read sched file");
            fclose(sched_file);
            _exit(1);
        }
    }

    fclose(sched_file);

    /* Extract vruntime value */
    vruntime = GetNumberFromText(line);

    /* Write vruntime in binary format */
    out_file = fopen("vruntime_log.bin", "wb");
    if (!out_file) {
        perror("Failed to open output file");
        _exit(1);
    }

    fwrite(&vruntime, sizeof(double), 1, out_file);
    fclose(out_file);

    /* Exit immediately after logging */
    _exit(0);
}

int main(int argc, char* argv[]) {
    struct sigaction sa;
    struct itimerval timer;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PID>\n", argv[0]);
        return 1;
    }

    target_pid = atoi(argv[1]);
    if (target_pid <= 0) {
        fprintf(stderr, "Invalid PID\n");
        return 1;
    }

    /* Register SIGALRM handler */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = alarm_handler;
    sigaction(SIGALRM, &sa, NULL);

    /* Configure single-shot timer (500 ms) */
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 500000;  // 500 ms
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);

    /* Wait for signal */
    while (1) {
        pause();
    }

    return 0;
}
    