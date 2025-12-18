#ifndef BUYER_INCLUDE_FUNCTIONS_H_
#define BUYER_INCLUDE_FUNCTIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

struct memory {
    int available_tickets;
    int purchases_log[100];
    int transactions;
    pthread_mutex_t mutex;
};

struct memory* AttachToSharedMemory();
void DetachSharedMemory(struct memory* shm_ptr);
void BuyTickets(struct memory* shm_ptr, int tickets_to_buy, int buyer_id);
void* BuyerThread(void* arg);

#endif  // BUYER_INCLUDE_FUNCTIONS_H_
