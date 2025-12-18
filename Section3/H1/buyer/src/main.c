#include <stdio.h>

#include "functions.h"

void* BuyerThread(void* arg);

int main() {
  pthread_t threads[3];
  int buyer_ids[3] = {1, 2, 3};
  int i;

  printf("=== BUYER PROGRAM STARTING ===\n");
  printf("Creating 3 buyer threads...\n");

  for (i = 0; i < 3; i++) {
    if (pthread_create(&threads[i], NULL, BuyerThread, &buyer_ids[i]) != 0) {
      perror("pthread_create");
      exit(1);
    }
    printf("Created buyer thread %d\n", buyer_ids[i]);
  }

  printf("All buyer threads created successfully!\n");
  printf("Starting ticket purchases...\n\n");

  for (i = 0; i < 3; i++) {
    if (pthread_join(threads[i], NULL) != 0) {
      perror("pthread_join");
      exit(1);
    }
    printf("Buyer thread %d completed\n", buyer_ids[i]);
  }

  printf("\n=== ALL BUYERS FINISHED ===\n");

  return 0;
}