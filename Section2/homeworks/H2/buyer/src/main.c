
#include <stdio.h>
#include "../include/functions.h"

int main() {
    struct memory* shared_data = AttachMemory();
    int tickets_to_buy;
    printf("Welcome to the ticket buyer system!\n");
    while (shared_data->available_tickets > 0) {
        printf("\nHow many tickets do you want to buy? ");
        scanf("%d", &tickets_to_buy);
        BuyTickets(shared_data, tickets_to_buy);
        sleep(2);
    }
    printf("\nNo more tickets available. Thank you!\n");
    Detach_shared_memory(shared_data);
    return 0;
}
