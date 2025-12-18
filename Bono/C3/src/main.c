#include <sys/types.h>
#include <curses.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PIDS 128
#define LINE_BUF 256
#define SAMPLE_US 500000  // 500 ms

static pid_t pids[MAX_PIDS];
static int n_pids;

static bool read_vruntime(pid_t pid, double *out) {
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/sched", pid);
    FILE *f = fopen(path, "r");
    if (!f) return false;

    char line[LINE_BUF];
    int line_no = 0;
    bool ok = false;
    while (fgets(line, sizeof(line), f)) {
        line_no++;
        if (line_no == 4) {  // 4th line holds vruntime
            // Expected like: "vruntime                             : 12345678.901234567"
            char *colon = strchr(line, ':');
            if (colon) {
                double v = 0.0;
                if (sscanf(colon + 1, "%lf", &v) == 1) {
                    *out = v;
                    ok = true;
                }
            }
            break;
        }
    }
    fclose(f);
    return ok;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pid1> [pid2 ...]\n", argv[0]);
        return 1;
    }
    if (argc - 1 > MAX_PIDS) {
        fprintf(stderr, "Too many PIDs (max %d)\n", MAX_PIDS);
        return 1;
    }
    n_pids = argc - 1;
    for (int i = 0; i < n_pids; i++) {
        pids[i] = (pid_t)strtol(argv[i + 1], NULL, 10);
    }

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    bool running = true;
    double values[MAX_PIDS];
    while (running) {
        // Handle input
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            running = false;
        }

        clear();
        mvprintw(0, 0, "Resource monitor (vruntime). Press q to quit.");
        for (int i = 0; i < n_pids; i++) {
            double v = 0.0;
            bool ok = read_vruntime(pids[i], &v);
            values[i] = ok ? v : -1.0;
            if (ok) {
                mvprintw(2 + i, 0, "PID %d: vruntime = %.6f", pids[i], v);
            } else {
                mvprintw(2 + i, 0, "PID %d: <unreadable: %s>", pids[i], strerror(errno));
            }
        }
        refresh();
        usleep(SAMPLE_US);
    }

    endwin();
    return 0;
}