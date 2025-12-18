
#define _XOPEN_SOURCE 700

#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SHM_NAME "/mem_block_exam_1"
#define MQ_NAME "/mq_exam_1"

static volatile sig_atomic_t signal_count = 0;

void Sigusr2_handler(int signum) {
  printf("Data Processor: new data available\n");
  signal_count++;  // change
}

int main() {
  int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  ftruncate(fd, 10 * sizeof(double));  // change
  double *shm_ptr = mmap(0, 10 * sizeof(double), PROT_READ | PROT_WRITE,
                         MAP_SHARED, fd, 0);  // change
  if (shm_ptr == MAP_FAILED) {
    perror("mmap failed");
    shm_unlink(SHM_NAME);  // change
    exit(1);
  }

  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = 80;
  attr.mq_curmsgs = 0;

  mqd_t queue_os = mq_open(MQ_NAME, O_CREAT | O_WRONLY, 0666, &attr);
  if (queue_os == -1) {
    perror("error in open");
    munmap(shm_ptr, 10 * sizeof(double));  // change
    shm_unlink(SHM_NAME);                  // change
    exit(EXIT_FAILURE);
  }

  double arr_pid = getpid();
  int status;

  struct sigaction sa;
  sa.sa_handler = Sigusr2_handler;
  sa.sa_flags = 0;                            // change
  sigemptyset(&sa.sa_mask);                   // change
  if (sigaction(SIGUSR2, &sa, NULL) == -1) {  
    perror("sigaction");                      // change
    munmap(shm_ptr, 10 * sizeof(double));     // change
    shm_unlink(SHM_NAME);                     // change
    mq_close(queue_os);                       // change
    mq_unlink(MQ_NAME);                       // change
    exit(EXIT_FAILURE);                       // change
  }

  pid_t pid1 = fork();  // change
  if (pid1 == 0) {
    printf("Process 1 working: %d\n", pid1);
    char buffer[14];
    snprintf(buffer, sizeof(buffer), "%d", (int)arr_pid);  // change
    execl("../test/data_creator", "data_creator", SHM_NAME, buffer, NULL);
    perror("execl worker1");
    exit(EXIT_FAILURE);
  } else if (pid1 < 0) {
    perror("fork worker1");
    munmap(shm_ptr, 10 * sizeof(double));  // change
    shm_unlink(SHM_NAME);                  // change
    mq_close(queue_os);                    // change
    mq_unlink(MQ_NAME);                    // change
    exit(EXIT_FAILURE);
  }

  pid_t pid2 = fork();
  if (pid2 == 0) {
    printf("Process 2 working: %d\n", pid2);
    execl("../test/data_logger", "data_logger", MQ_NAME, NULL);
    perror("execl worker2");
    exit(EXIT_FAILURE);
  } else if (pid2 < 0) {
    perror("fork worker2");
    kill(pid1, SIGTERM);                   // change
    waitpid(pid1, NULL, 0);                // change
    munmap(shm_ptr, 10 * sizeof(double));  // change
    shm_unlink(SHM_NAME);                  // change
    mq_close(queue_os);                    // change
    mq_unlink(MQ_NAME);                    // change
    exit(EXIT_FAILURE);
  }

  while (signal_count < 5) {
    pause();                        // change
    double sum = 0.0;               // change
    for (int i = 0; i < 10; i++) {  // change
      sum += shm_ptr[i];            // change
    }
    double average = sum / 10;  // change
    char buffer[80];
    snprintf(buffer, sizeof(buffer), "%f", average);               // change
    if (mq_send(queue_os, buffer, strlen(buffer) + 1, 0) == -1) {  // change
      perror("mq_send");                                           // change
      break;                                                       // change
    }
  }

  waitpid(pid1, &status, 0);
  waitpid(pid2, &status, 0);
  munmap(shm_ptr, 10 * sizeof(double));
  close(fd);
  shm_unlink(SHM_NAME);
  mq_close(queue_os);  // change
  mq_unlink(MQ_NAME);  // change
  return 0;
}