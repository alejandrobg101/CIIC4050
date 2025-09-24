#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

int main() {
  pid_t pid1;
  pid_t pid2;
  pid_t pid3;
  int status;

  pid1 = fork();
  if (pid1 == 0) {
    sleep(1);
    execlp("../test/geo_calc", "geo_calc", "circle_perimeter", "10.0", NULL);
    perror("Child process 1 failed");
    exit(1);
  } else if (pid1 > 0) {
    fprintf(stderr, "Child 1 (PID: %d) is running 'circle_perimeter'\n", pid1);
  }
  pid2 = fork();
  if (pid2 == 0) {
    sleep(2);
    execlp("../test/geo_calc", "geo_calc", "circle_area", "5.0", NULL);
    perror("Child process 2 failed");
    exit(1);
  } else if (pid2 > 0) {
    fprintf(stderr, "Child 2 (PID: %d) is running 'circle_area'\n", pid1);
  }
  pid3 = fork();
  if (pid3 == 0) {
    sleep(3);
    execlp("../test/geo_calc", "geo_calc", "sphere_volume", "2.5", NULL);
    perror("Child process 3 failed");
    exit(1);
  } else if (pid3 > 0) {
    fprintf(stderr, "Child 3 (PID: %d) is running 'sphere_volume'\n", pid1);
  }

  waitpid(pid1, &status, 0);
  waitpid(pid2, &status, 0);
  waitpid(pid3, &status, 0);

  fprintf(stderr, "Parent process is finishing.\n");
  return 0;
}