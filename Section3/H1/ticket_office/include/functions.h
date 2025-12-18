#ifndef TICKET_OFFICE_INCLUDE_FUNCTIONS_H_
#define TICKET_OFFICE_INCLUDE_FUNCTIONS_H_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct memory {
  int available_tickets;
  int purchases_log[100];
  int transactions;
  pthread_mutex_t mutex;
};

int CreateSharedMemory();
struct memory* AttachSharedMemory(int shmid);
void DetachSharedMemory(struct memory* shm_ptr);
void CleanupSharedMemory(int shmid, struct memory* shm_ptr);
void InitializeSharedMemory(struct memory* shm_ptr);
void PrintTicketReport(struct memory* shm_ptr, FILE* log_file);

#endif  // TICKET_OFFICE_INCLUDE_FUNCTIONS_H_
