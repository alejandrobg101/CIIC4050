#include "functions.h"
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
