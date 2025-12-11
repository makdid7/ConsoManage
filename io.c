//
// Created by mkd on 11/28/25.
//

#include "io.h"

#include <stdio.h>


void printUser(User *user) {
    printf("\n------------- User Details -------------\n");
    printf("Full name: %s\n", user->fullName);
    printf("Age:       %d\n", user->age);
    printf("E-mail:    %s\n", user->email);
    printf("----------------------------------------\n\n");
}

void printEvent(Event *event) {
    // if (event->maxSeatNumber == 0) {
    //     return;
    //     // under normal circumstances, this cannot happen; we return because
    //     malloc gives memory to a non-existing event at the beginning of the
    //     program.
    // }
    printf("\n------------- Event Details -------------\n");
    printf("ID:       %04d\n", event->id);
    printf("Name:     %s\n", event->name);
    printf("Date:     %s\n", event->date);
    printf("Time:     %s\n", event->time);
    printf("Location: %s\n", event->location);
    printf("Seats:    %c01-%c%02d\n", 'A', event->maxSeatRow, event->maxSeatNumber);
    if (event->price == 0) {
        printf("Price:    Free\n");
    } else {
        printf("Price:    %.2lf\n", event->price);
    }
    printf("-----------------------------------------\n\n");
}

void printUserTickets(User *user, EventList *events) {
    printf("\n---------- Tickets Owned by %s ----------\n", user->fullName);
    if (user->ticketsCount == 0) {
        printf("No tickets purchased yet.\n");
    } else {
        for (int i = 0; i < user->ticketsCount; i++) {
            Ticket ticket = user->ticketsOwned[i];
            Event event;
            int found = 0;
            for (int k = 0; k < events->count; k++) {
                if (events->data[k].id == ticket.eventID) {
                    event = events->data[k];
                    found = 1;
                    break;
                }
            }

            printf("Ticket Reference: %s\n", ticket.ticketReference);
            if (found) {
                printf("Event: %s\n", event.name);
            } else {
                printf("Event: Unknown\n");
            }
            printf("Seat: %c%02d\n", ticket.row, ticket.seatNumber);
            printf("Price Paid: %.2lf\n\n", ticket.pricePaid);
        }
    }
    printf("------------------------------------------\n");
}


void printSeatmap(char* seatmap, char maxSeatRow, int maxSeatNumber) {
    printf("------------- Seatmap -------------\n");

    const int seats = maxSeatNumber;

    for (char row = 'A'; row <= maxSeatRow; row++) {
        printf("%c: ", row);

        const int rowIndex = row - 'A';

        for (int seat = 1; seat <= seats; seat++) {

            const int cellSize = 3;
            const int index = (rowIndex * seats + (seat - 1)) * cellSize;

            printf("%s ", &seatmap[index]);
        }

        printf("\n");
    }

    printf("----------------------------------\n\n");
}