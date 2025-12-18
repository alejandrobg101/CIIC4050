#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdint.h>
#include <sys/mman.h>
#include <signal.h>

#define SHM_NAME "/mem_block_exam_1"
#define SHM_KEY 1234

struct memory {
    double item1;
    double item2;
    double item3;
    double item4;
    double item5;
    double item6;
    double item7;
    double item8;
    double item9;
    double item10;
};

void Sigusr2_handler(int signum) {
  printf("Data Processor: new data available\n");
}

int main() {
  int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  ftruncate(fd, 1024);
  char *shm_ptr = mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shm_ptr == MAP_FAILED) {
    perror("mmap failed");
    exit(1);
  }

  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = 40;
  attr.mq_curmsgs = 0;

  mqd_t queue_os;
  queue_os = mq_open("/mq_exam_1", O_CREAT | O_WRONLY, 0666, &attr);
  if (queue_os == -1) {
    perror("error in open");
    exit(EXIT_FAILURE);
  }

  double arr_pid = getpid();

  pid_t pid1, pid2;
  int status;
  int signal_count = 0;
  pid1 = fork();

  struct sigaction sa;
  sa.sa_handler = Sigusr2_handler;
  sigaction(SIGUSR2, &sa, NULL);
  while (1 || signal_count<=5) {
    if (pid1 == 0) {
      printf("Process 1 working: %d\n", pid1);
      char buffer[14];
      snprintf(buffer, sizeof(buffer), "%f", arr_pid);
      execlp("../test/data_creator", "data_creator", "/mem_block_exam_1",
         buffer, NULL);
      perror("execl worker1");
      exit(EXIT_FAILURE);
    } else if (pid1 < 0) {
      perror("fork worker1");
      exit(EXIT_FAILURE);
    }
    signal_count++;
    sleep(1);
  }

  pid2 = fork();
  if (pid2 == 0) {
    printf("Process 1 working: %d\n", pid2);
    execlp("../test/data_logger", "data_logger", "/mq_exam_1", NULL);
    perror("execl worker2");
    exit(EXIT_FAILURE);
  } else if (pid2 < 0) {
    perror("fork worker2");
    exit(EXIT_FAILURE);
  }
  waitpid(pid1, &status, 0);
  waitpid(pid2, &status, 0);
  double average = 1;
  char buffer[80];
  snprintf(buffer, sizeof(buffer), "%f", average);
  munmap(shm_ptr, 1024);
  shm_unlink(SHM_NAME);
  unlink("/mq_exam_1");
  return 0;
}
