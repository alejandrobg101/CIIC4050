#include <stdio.h>
#include "../include/functions.h"

int main() {
    int shmid = Create_shared_memory();
    struct memory* shared_data = Get_shared_memory(shmid);

    shared_data->available_tickets = 20;
    shared_data->purchased_tickets = 0;
    shared_data->transactions = 0;
    printf("Ticket office started with 20 tickets available.\n");
    while (shared_data->available_tickets > 0) {
        printf("\nTICKET REPORT:\n");
        printf("Purchased tickets: %d\n", shared_data->purchased_tickets);
        printf("Available tickets: %d\n", shared_data->available_tickets);
        printf("Transactions: %d\n", shared_data->transactions);
        sleep(1);
    }
    printf("\nTICKET REPORT:\n");
    printf("Purchased tickets: %d\n", shared_data->purchased_tickets);
    printf("Available tickets: %d\n", shared_data->available_tickets);
    printf("Transactions: %d\n", shared_data->transactions);
    printf("SOLD OUT...\n");
    Detach_shared_memory(shared_data);
    Destroy_shared_memory(shmid);

    return 0;
}
