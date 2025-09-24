#include "../include/functions.h"
#include <stdio.h>

int Create_shared_memory() {
    int shmid = shmget(SHM_KEY, sizeof(struct memory), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    return shmid;
}

int Attach_shared_memory() {
    int shmid = shmget(SHM_KEY, sizeof(struct memory), 0);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    return shmid;
}

struct memory* Get_shared_memory(int shmid) {
    struct memory* ptr = (struct memory*)shmat(shmid, NULL, 0);
    if (ptr == (struct memory*)-1) {
        perror("shmat");
        exit(1);
    }
    return ptr;
}

void Detach_shared_memory(struct memory* ptr) {
    if (shmdt(ptr) == -1) {
        perror("shmdt");
        exit(1);
    }
}

void Destroy_shared_memory(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(1);
    }
}

struct memory* AttachMemory() {
    int shmid = Attach_shared_memory();
    return Get_shared_memory(shmid);
}

int BuyTickets(struct memory* shared_data, int requested_tickets) {
    if (requested_tickets <= 0) {
        printf("Try again, invalid number of tickets\n");
        return 0;
    }
    int tickets_to_buy = requested_tickets;
    if (tickets_to_buy > 5) {
        tickets_to_buy = 5;
    }
    if (tickets_to_buy > shared_data->available_tickets) {
        tickets_to_buy = shared_data->available_tickets;
    }
    shared_data->available_tickets -= tickets_to_buy;
    shared_data->purchased_tickets += tickets_to_buy;
    shared_data->transactions++;
    printf("Company purchased %d tickets. Available: %d\n",
           tickets_to_buy, shared_data->available_tickets);
    return tickets_to_buy;
}
