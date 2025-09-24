#ifndef INCLUDE_FUNCTIONS_H_
#define INCLUDE_FUNCTIONS_H_

#include <signal.h>

extern volatile sig_atomic_t actor_ready;
extern volatile sig_atomic_t director_ready;

void Handle_sigusr1(int sig);
void Handle_sigusr2(int sig);

#endif  // INCLUDE_FUNCTIONS_H_