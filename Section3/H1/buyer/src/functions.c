#include "functions.h"

#include <stdio.h>

struct memory* AttachToSharedMemory() {
  key_t key = ftok("ticket_office", 65);
  int shmid = shmget(key, sizeof(struct memory), 0666);

  if (shmid == -1) {
    perror("shmget in buyer");
    exit(1);
  }

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

void BuyTickets(struct memory* shm_ptr, int tickets_to_buy, int buyer_id) {
  pthread_mutex_lock(&shm_ptr->mutex);

  if (shm_ptr->available_tickets >= tickets_to_buy) {
    shm_ptr->available_tickets -= tickets_to_buy;

    shm_ptr->purchases_log[shm_ptr->transactions] = tickets_to_buy;
    shm_ptr->transactions++;

    printf(
        "Buyer %d: Successfully purchased %d tickets. %d tickets "
        "remaining.\n",
        buyer_id, tickets_to_buy, shm_ptr->available_tickets);
  } else {
    printf(
        "Buyer %d: Failed to purchase %d tickets. "
        "Only %d tickets available.\n",
        buyer_id, tickets_to_buy, shm_ptr->available_tickets);
  }

  pthread_mutex_unlock(&shm_ptr->mutex);
}

void* BuyerThread(void* arg) {
  int buyer_id = *(int*)arg;
  struct memory* shm_ptr;

  srand(time(NULL) + buyer_id);

  shm_ptr = AttachToSharedMemory();

  printf("Buyer %d: Started and connected to shared memory\n", buyer_id);

  while (1) {
    int tickets_to_buy = (rand() % 5) + 1;

    pthread_mutex_lock(&shm_ptr->mutex);
    int tickets_available = shm_ptr->available_tickets;
    if (tickets_available == 0) {
      pthread_mutex_unlock(&shm_ptr->mutex);
      printf("Buyer %d: No more tickets available. Exiting.\n", buyer_id);
      break;
    }

    if (shm_ptr->available_tickets >= tickets_to_buy) {
      shm_ptr->available_tickets -= tickets_to_buy;
      shm_ptr->purchases_log[shm_ptr->transactions] = tickets_to_buy;
      shm_ptr->transactions++;

      printf(
          "Buyer %d: Successfully purchased %d tickets. %d tickets "
          "remaining.\n",
          buyer_id, tickets_to_buy, shm_ptr->available_tickets);
    } else {
      printf(
          "Buyer %d: Failed to purchase %d tickets. Only %d tickets "
          "available.\n",
          buyer_id, tickets_to_buy, shm_ptr->available_tickets);
    }
    pthread_mutex_unlock(&shm_ptr->mutex);

    usleep(100000);
  }

  DetachSharedMemory(shm_ptr);
  printf("Buyer %d: Disconnected and exiting\n", buyer_id);

  pthread_exit(NULL);
}