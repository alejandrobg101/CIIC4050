#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

#include "../include/functions.h"
// --- Main Program ---

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <nice_value> <cpu_number>\n", argv[0]);
    fprintf(stderr, "Example: %s 10 1\n", argv[0]);
    return 1;
  }

  int nice_val = atoi(argv[1]);
  int cpu_num = atoi(argv[2]);
  pthread_t thread;

  // Log the process start for the test script
  printf("TEST_START|PID=%d|NICE=%d|CPU=%d\n", getpid(), nice_val, cpu_num);

  // Set process nice value (may fail if not permitted)
  if (SetNiceValue(nice_val) != 0) {
    // continue even if it fails; test harness may expect program to run
  }

  // Set CPU affinity for the main thread (and later threads will inherit or
  // can set their own affinity). We'll pin the main thread so the test is
  // deterministic.
  if (SetCpuAffinity(cpu_num) != 0) {
    // continue even if it fails
  }
  sleep(1);
  // Create the thread, passing the nice value as argument
  // NOTE: Remember to send pointers to a thread
  if (pthread_create(&thread, NULL, HeavyCpuTask, &nice_val) != 0) {
    fprintf(stderr, "ERROR: pthread_create failed: %s\n", strerror(errno));
    return 1;
  }

  // Wait for the thread to finish
  if (pthread_join(thread, NULL) != 0) {
    fprintf(stderr, "ERROR: pthread_join failed: %s\n", strerror(errno));
    return 1;
  }

  printf("TEST_END\n");

  return 0;
}
