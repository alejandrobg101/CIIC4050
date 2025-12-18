#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <stdlib.h>

void Convert_Time(double seconds, int *days, int *hours, int *mins, int *secs) {
    *secs  = (int)seconds % 60;
    seconds /= 60;
    *mins  = (int)seconds % 60;
    seconds /= 60;
    *hours = (int)seconds % 24;
    seconds /= 24;
    *days  = (int)seconds;
}

static char *parent_of_cwd(void) {
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return NULL;
    }

    char *cwd_copy = strdup(cwd);
    if (!cwd_copy) {
        perror("strdup");
        return NULL;
    }

    char *parent = dirname(cwd_copy);
    char *result = strdup(parent);
    free(cwd_copy);
    return result;
}

int main(void) {
    FILE *up_fp = fopen("/proc/uptime", "r");
    if (!up_fp) {
        perror("Failed to open /proc/uptime");
        return 1;
    }

    double uptime = 0.0, idle_time = 0.0;
    if (fscanf(up_fp, "%lf %lf", &uptime, &idle_time) != 2) {
        fprintf(stderr, "Failed to parse /proc/uptime\n");
        fclose(up_fp);
        return 1;
    }
    fclose(up_fp);

    FILE *cpu_fp = fopen("/proc/cpuinfo", "r");
    if (!cpu_fp) {
        perror("Failed to open /proc/cpuinfo");
        return 1;
    }

    int num_cores = 0;
    char line[256];
    while (fgets(line, sizeof(line), cpu_fp)) {
        if (strncmp(line, "processor", 9) == 0)
            ++num_cores;
    }
    fclose(cpu_fp);

    if (num_cores == 0) {
        fprintf(stderr, "No processors detected\n");
        return 1;
    }

    double utilisation = (1.0 - (idle_time / (uptime * num_cores))) * 100.0;
    if (utilisation < 0.0)  utilisation = 0.0;
    if (utilisation > 100.0) utilisation = 100.0;

    int up_d = 0, up_h = 0, up_m = 0, up_s = 0;
    Convert_Time(uptime, &up_d, &up_h, &up_m, &up_s);

    int idle_d = 0, idle_h = 0, idle_m = 0, idle_s = 0;
    Convert_Time(idle_time, &idle_d, &idle_h, &idle_m, &idle_s);

    char *project_root = parent_of_cwd();
    if (!project_root) {
        return 1;
    }

    char report_path[PATH_MAX];
    int rc = snprintf(report_path, sizeof(report_path),
                      "%s/report.txt", project_root);
    if ((size_t)rc >= sizeof(report_path)) {
        fprintf(stderr, "Report path too long\n");
        free(project_root);
        return 1;
    }

    FILE *report_fp = fopen(report_path, "w");
    if (!report_fp) {
        perror("Failed to create report.txt in project root");
        free(project_root);
        return 1;
    }

    fprintf(report_fp, "--- System Activity Analysis ---\n");
    fprintf(report_fp, "Detected CPU Cores: %d\n", num_cores);
    fprintf(report_fp,
            "Total Uptime: %d days, %d hours, %d minutes, %d seconds\n",
            up_d, up_h, up_m, up_s);
    fprintf(report_fp,
            "Idle Time (Sum of Cores): %f seconds\n", idle_time);
    fprintf(report_fp, "Global CPU Utilization Percentage: %.2f%%\n",
            utilisation);
    fprintf(report_fp, "----------------------------------------");

    fclose(report_fp);

    printf("[SUCCESS]: The system analysis has been saved to"
           " the file '%s/report.txt'.\n",
           project_root);

    free(project_root);
    return 0;
}