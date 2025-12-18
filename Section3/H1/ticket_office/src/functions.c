#include "functions.h"

#include <stdio.h>

int CreateSharedMemory() {
  key_t key = ftok("ticket_office", 65);
  int shmid = shmget(key, sizeof(struct memory), IPC_CREAT | 0666);

  if (shmid == -1) {
    perror("shmget");
    exit(1);
  }

  return shmid;
}

struct memory* AttachSharedMemory(int shmid) {
  struct memory* shm_ptr = (struct memory*)shmat(shmid, NULL, 0);

  if (shm_ptr == (struct memory*)-1) {
    perror("shmat");
    exit(1);
  }

  return shm_ptr;
}

void DetachSharedMemory(struct memory* shm_ptr) {
  if (shmdt(shm_ptr) == -1) {
    perror("shmdt");
    exit(1);
  }
}

void CleanupSharedMemory(int shmid, struct memory* shm_ptr) {
  DetachSharedMemory(shm_ptr);
  shmctl(shmid, IPC_RMID, NULL);
}

void InitializeSharedMemory(struct memory* shm_ptr) {
  shm_ptr->available_tickets = 100;
  shm_ptr->transactions = 0;

  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
  if (pthread_mutex_init(&shm_ptr->mutex, &attr) != 0) {
    perror("pthread_mutex_init");
    exit(1);
  }
  pthread_mutexattr_destroy(&attr);

  memset(shm_ptr->purchases_log, 0, sizeof(shm_ptr->purchases_log));
}

void PrintTicketReport(struct memory* shm_ptr, FILE* log_file) {
  pthread_mutex_lock(&shm_ptr->mutex);
  int tickets = shm_ptr->available_tickets;
  int transactions = shm_ptr->transactions;
  pthread_mutex_unlock(&shm_ptr->mutex);

  fprintf(log_file, "TICKET REPORT:\n");
  fprintf(log_file, "Available tickets: %d\n", tickets);

  if (tickets == 0) {
    fprintf(log_file, "STATUS: SOLD OUT...\n");
  } else {
    fprintf(log_file, "STATUS: Tickets available\n");
  }

  fprintf(log_file, "Total transactions: %d\n", transactions);
  fprintf(log_file, "============================\n\n");
  fflush(log_file);
}