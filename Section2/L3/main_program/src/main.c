#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdint.h>

int main() {
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 5;
  attr.mq_msgsize = 40;
  attr.mq_curmsgs = 0;

  printf("Main process started with PID: %d\n", getpid());

  mqd_t queue_os;
  queue_os = mq_open("/prime_queue", O_CREAT | O_WRONLY, 0666, &attr);
  if (queue_os == -1) {
    perror("error in open");
    exit(EXIT_FAILURE);
  }

  pid_t pid1, pid2;
  const char *worker_path = "../worker/build/worker";
  pid1 = fork();
  if (pid1 == 0) {
    execl(worker_path, "worker", "/prime_queue", NULL);
    perror("execl worker1");
    exit(EXIT_FAILURE);
  } else if (pid1 < 0) {
    perror("fork worker1");
    exit(EXIT_FAILURE);
  }

  pid2 = fork();
  if (pid2 == 0) {
    execl(worker_path, "worker", "/prime_queue", NULL);
    perror("execl worker2");
    exit(EXIT_FAILURE);
  } else if (pid2 < 0) {
    perror("fork worker2");
    exit(EXIT_FAILURE);
  }

  printf("workers created\n");
  sleep(1);


  FILE *file = fopen("../test/numbers.txt", "r");
  if (!file) {
    perror("fopen");
    mq_close(queue_os);
    mq_unlink("/prime_queue");
    exit(EXIT_FAILURE);
  }

  long long number;
  char msg[40];
  while (fscanf(file, "%lld", &number) == 1) {
    int written = snprintf(msg, sizeof(msg), "%lld", number);
    if (written >= sizeof(msg)) {
      fprintf(stderr, "Error: Number too large for message buffer\n");
      fclose(file);
      mq_close(queue_os);
      mq_unlink("/prime_queue");
      exit(EXIT_FAILURE);
    }
    if (mq_send(queue_os, msg, strlen(msg) + 1, 0) == -1) {
      perror("mq_send");
    }
  printf("number %lld sent\n", number);
  }
  fclose(file);

  snprintf(msg, sizeof(msg), "%lld", -1LL);
  for (int i = 0; i < 2; i++) {
    sleep(1);
    if (mq_send(queue_os, msg, strlen(msg) + 1, 0) == -1) {
      perror("mq_send termination");
      mq_close(queue_os);
      mq_unlink("/prime_queue");
      exit(EXIT_FAILURE);
    }
  }

  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);

  mq_close(queue_os);
  mq_unlink("/prime_queue");
  printf("Main process finished. Message queue unlinked\n");

  return EXIT_SUCCESS;
}