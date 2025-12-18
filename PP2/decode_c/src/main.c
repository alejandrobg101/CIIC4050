#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define WEIGHT 100
#define HEIGHT 100
#define NUM_THREADS 4
#define TOTAL_PIXELS (WEIGHT * HEIGHT)

int tests[] = {88, 222, 5, 934};
int progress[NUM_THREADS] = {0};
pthread_t threads[NUM_THREADS];
pthread_mutex_t progress_mutex = PTHREAD_MUTEX_INITIALIZER;

void TryTest(int signum) {
    pthread_mutex_lock(&progress_mutex);
    printf("Timer tick - Progress:\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        double percentage = (progress[i] * 100.0) / TOTAL_PIXELS;
        printf("  Thread %d (key %d): %d pixels decoded (%.2f%%)\n",
               i, tests[i], progress[i], percentage);
    }
    printf("\n");
    pthread_mutex_unlock(&progress_mutex);
}

void* Decode(void* arg) {
    int try_number = *(int*)arg;
    int key = tests[try_number];

    int fd = open("encrypt.bin", O_RDONLY);
    if (fd < 0) {
        perror("Error opening encrypt.bin");
        pthread_exit(NULL);
    }

    mkdir("outputs", 0755);

    char ppm_filename[256];
    char txt_filename[256];
    snprintf(ppm_filename, sizeof(ppm_filename), "outputs/output_%d.ppm", key);
    snprintf(txt_filename, sizeof(txt_filename), "outputs/output_%d.txt", key);

    int ppm_fd = open(ppm_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int txt_fd = open(txt_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (ppm_fd < 0 || txt_fd < 0) {
        perror("Error creating output files");
        close(fd);
        if (ppm_fd >= 0) close(ppm_fd);
        if (txt_fd >= 0) close(txt_fd);
        pthread_exit(NULL);
    }

    char ppm_header[256];
    int header_len = snprintf(ppm_header, sizeof(ppm_header),
     "P6\n%d %d\n255\n", WEIGHT, HEIGHT);
    write(ppm_fd, ppm_header, header_len);

    unsigned char buffer[3];
    char txt_line[32];

    for (int i = 0; i < TOTAL_PIXELS; i++) {
        ssize_t bytes_read = read(fd, buffer, 3);
        if (bytes_read != 3) {
            break;
        }

        unsigned char component_r_decrypted = buffer[0] ^ key;
        unsigned char component_g_decrypted = buffer[1] ^ key;
        unsigned char component_b_decrypted = buffer[2] ^ key;

        unsigned char rgb[3] = {component_r_decrypted,
             component_g_decrypted, component_b_decrypted};
        write(ppm_fd, rgb, 3);

        int txt_len = snprintf(txt_line, sizeof(txt_line), "%d %d %d\n",
                              component_r_decrypted,
                               component_g_decrypted, component_b_decrypted);
        write(txt_fd, txt_line, txt_len);
        usleep(200);

        pthread_mutex_lock(&progress_mutex);
        progress[try_number]++;
        pthread_mutex_unlock(&progress_mutex);
    }

    close(fd);
    close(ppm_fd);
    close(txt_fd);

    pthread_exit(NULL);
}

int main() {
    signal(SIGALRM, TryTest);

    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 100000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 200000;

    if (setitimer(ITIMER_REAL, &timer, NULL) < 0) {
        perror("Error setting timer");
        return 1;
    }

    mkdir("outputs", 0755);

    int thread_args[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        if (pthread_create(&threads[i], NULL, Decode, &thread_args[i]) != 0) {
            perror("Error creating thread");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);

    return 0;
}
