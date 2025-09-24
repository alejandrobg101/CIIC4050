#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>

int is_prime(long long number) {
    if (number <= 1) return 0;
    long long i;
    for (i=2; i*i <= number; i++) {
        if (number % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s /prime_queue\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  mqd_t mq = mq_open(argv[1], O_RDONLY);
  if (mq == (mqd_t)-1) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  printf("Worker process %d is waiting for messages.\n", getpid());

  char msg[40];
  long long number;
  while (1) {
    ssize_t bytes = mq_receive(mq, msg, 40, NULL);
    if (bytes == -1) {
      perror("mq_receive");
      mq_close(mq);
      exit(EXIT_FAILURE);
    }
    msg[bytes] = '\0';
    number = atoll(msg);

  printf("Worker %d received number: %lld\n", getpid(), number);

    if (number == -1) {
      break;
    }

    if (is_prime(number)) {
      fprintf(stderr, "PRIME FOUND %lld\n", number);
    }
  }

  mq_close(mq);
  return EXIT_SUCCESS;
}