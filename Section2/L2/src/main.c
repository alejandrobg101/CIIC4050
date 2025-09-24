#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#include "functions.h"




int main() {
  pid_t pid;
  struct sigaction sa_usr1, sa_usr2;

  pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    printf("Actor (PID: %d): I'm ready\n", getpid());

    memset(&sa_usr1, 0, sizeof(sa_usr1));
    sa_usr1.sa_handler = Handle_sigusr1;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    pause();

    if (actor_ready) {
      printf("Actor (PID: %d): Received SIGUSR1 from the director\n", getpid());
      sleep(1);
      printf("Actor (PID: %d): To be, or not to be, that is the question: "
        "Whether 'tis nobler in the mind to suffer the slings and "
        "arrows of outrageous fortune, or to take arms against a sea of "
        "troubles, and by opposing end them.\n", getpid());
      printf("Actor (PID: %d): I'm finished sending SIGURS2 from the actor\n",
        getpid());
      kill(getppid(), SIGUSR2);
    }
    exit(EXIT_SUCCESS);
    } else {
      printf("Director (PID:%d): The play is about to begin\n", getpid());
      sleep(1);

      memset(&sa_usr2, 0, sizeof(sa_usr2));
      sa_usr2.sa_handler = Handle_sigusr2;
      sigaction(SIGUSR2, &sa_usr2, NULL);

      printf("Director (PID:%d): I'll send SIGUSR1 to "
        "the actor to start\n", getpid());
      kill(pid, SIGUSR1);

      pause();

      if (director_ready) {
        printf("Director (PID:%d): Received SIGUSR2 "
            "from the actor\n", getpid());
        sleep(1);
        printf("Director (PID:%d): OKay, We're ending the play\n", getpid());
      }
      kill(pid, SIGKILL);
      waitpid(pid, NULL, 0);
    }
  return 0;
}