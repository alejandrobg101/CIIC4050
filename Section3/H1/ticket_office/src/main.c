#include <stdio.h>

#include "functions.h"

int main() {
  int shmid;
  struct memory* shm_ptr;

  printf("=== TICKET OFFICE STARTING ===\n");
  printf("Initializing shared memory with 100 tickets...\n");

  shmid = CreateSharedMemory();
  shm_ptr = AttachSharedMemory(shmid);

  InitializeSharedMemory(shm_ptr);

  printf("Shared memory initialized successfully!\n");
  printf("Starting ticket office reports...\n\n");

  FILE* log_file = fopen("logs/ticket_office.log", "w");
  if (!log_file) {
    perror("fopen");
    exit(1);
  }

  int sold_out_count = 0;

  while (1) {
    sleep(1);

    pthread_mutex_lock(&shm_ptr->mutex);
    int tickets_available = shm_ptr->available_tickets;
    pthread_mutex_unlock(&shm_ptr->mutex);

    if (tickets_available == 0) {
      sold_out_count++;
      if (sold_out_count >= 5) {
        break;
      }
    } else {
      sold_out_count = 0;
    }

    PrintTicketReport(shm_ptr, log_file);
  }

  fclose(log_file);
  pthread_mutex_destroy(&shm_ptr->mutex);
  CleanupSharedMemory(shmid, shm_ptr);

  printf("=== TICKET OFFICE SHUTTING DOWN ===\n");
  return 0;
}