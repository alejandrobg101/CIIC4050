#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SHM_NAME "/mem_block_exam_1"
#define MQ_NAME "/mq_exam_1"
#define NUM_ELEMENTS 10
#define MAX_SIGNALS 5

static volatile sig_atomic_t signal_count = 0;

void Signal_handler(int sig) {
  printf("Data Processor: new data available\n");
  signal_count++;
}

int main() {
  // Set up SIGUSR2 signal handler
  struct sigaction sa;
  sa.sa_handler = Signal_handler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGUSR2, &sa, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  // Create shared memory
  int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }

  if (ftruncate(shm_fd, NUM_ELEMENTS * sizeof(double)) == -1) {
    perror("ftruncate");
    shm_unlink(SHM_NAME);
    exit(EXIT_FAILURE);
  }

  double *shm_data = mmap(NULL, NUM_ELEMENTS * sizeof(double),
                          PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shm_data == MAP_FAILED) {
    perror("mmap");
    shm_unlink(SHM_NAME);
    exit(EXIT_FAILURE);
  }

  // Create message queue
  struct mq_attr attr = {0};
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = 80;

  mqd_t mq = mq_open(MQ_NAME, O_CREAT | O_WRONLY, 0666, &attr);
  if (mq == -1) {
    perror("mq_open");
    munmap(shm_data, NUM_ELEMENTS * sizeof(double));
    shm_unlink(SHM_NAME);
    exit(EXIT_FAILURE);
  }

  // Fork data_creator process
  pid_t creator_pid = fork();
  if (creator_pid == -1) {
    perror("fork creator");
    exit(EXIT_FAILURE);
  }

  if (creator_pid == 0) {
    // Child process: data_creator
    char pid_str[16];
    snprintf(pid_str, sizeof(pid_str), "%d", getppid());
    execl("./data_creator", "data_creator", SHM_NAME, pid_str, NULL);
    perror("execl data_creator");
    exit(EXIT_FAILURE);
  }

  // Fork data_logger process
  pid_t logger_pid = fork();
  if (logger_pid == -1) {
    perror("fork logger");
    kill(creator_pid, SIGTERM);
    waitpid(creator_pid, NULL, 0);
    exit(EXIT_FAILURE);
  }

  if (logger_pid == 0) {
    // Child process: data_logger
    execl("./data_logger", "data_logger", MQ_NAME, NULL);
    perror("execl data_logger");
    exit(EXIT_FAILURE);
  }

  // Main loop to process signals
  while (signal_count < MAX_SIGNALS) {
    pause();  // Wait for SIGUSR2

    if (signal_count <= MAX_SIGNALS) {
      // Calculate average
      double sum = 0.0;
      for (int i = 0; i < NUM_ELEMENTS; i++) {
        sum += shm_data[i];
      }
      double average = sum / NUM_ELEMENTS;

      // Convert to string and send via message queue
      char buffer[80];
      snprintf(buffer, sizeof(buffer), "%f", average);

      if (mq_send(mq, buffer, strlen(buffer) + 1, 0) == -1) {
        perror("mq_send");
        break;
      }
    }
  }

  // Cleanup
  waitpid(creator_pid, NULL, 0);
  waitpid(logger_pid, NULL, 0);

  // Close and unlink shared memory
  munmap(shm_data, NUM_ELEMENTS * sizeof(double));
  close(shm_fd);
  shm_unlink(SHM_NAME);

  // Close and unlink message queue
  mq_close(mq);
  mq_unlink(MQ_NAME);

  return 0;
}