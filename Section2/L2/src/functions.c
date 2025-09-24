#include "functions.h"

volatile sig_atomic_t actor_ready = 0;
volatile sig_atomic_t director_ready = 0;

void Handle_sigusr1(int sig) {
  (void)sig;
  actor_ready = 1;
}

void Handle_sigusr2(int sig) {
  (void)sig;
  director_ready = 1;
}