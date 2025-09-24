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

// Shared memory structure
struct memory {
    int available_tickets;
    int purchased_tickets;
    int transactions;
};

// Shared memory key
#define SHM_KEY 1234

// Function prototypes
int Create_shared_memory();
int Attach_shared_memory();
struct memory* Get_shared_memory(int shmid);
void Detach_shared_memory(struct memory* ptr);
void Destroy_shared_memory(int shmid);

#endif  // INCLUDE_FUNCTIONS_H_
