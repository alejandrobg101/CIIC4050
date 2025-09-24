#ifndef INCLUDE_FUNCTIONS_H_
#define INCLUDE_FUNCTIONS_H_

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

struct memory {
    int available_tickets;
    int purchased_tickets;
    int transactions;
};

#define SHM_KEY 1234

int Create_shared_memory();
int Attach_shared_memory();
struct memory* get_shared_memory(int shmid);
void Detach_shared_memory(struct memory* ptr);
void Destroy_shared_memory(int shmid);
struct memory* AttachMemory();
int BuyTickets(struct memory* shared_data, int requested_tickets);

#endif  // INCLUDE_FUNCTIONS_H_
