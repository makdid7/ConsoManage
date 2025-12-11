//
// Created by mkd on 11/28/25.
//

#include "io.h"

#include <stdio.h>
#include <string.h>

#include "file.h"

int nextEventID;

void setNextEventID(int newID) {
    nextEventID = newID;
}

void printUser(User *user) {
    printf("\n------------- User Details -------------\n");
    printf("Full name: %s\n", user->fullName);
    printf("Age:       %d\n", user->age);
    printf("E-mail:    %s\n", user->email);
    printf("----------------------------------------\n\n");
}

void printEvent(Event *event) {

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



// -----Event Creation -----

void createNewEvent(EventList *events) {
    Event newEvent;
    char buffer[100];

    while (1) {
        printf("Enter event name: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (isValidEventName(buffer)) {
            strcpy(newEvent.name, buffer);
            break;
        }

        printf("Please only use Latin characters, numbers, dashes, commas, and spaces.\n");
    }

    while (1) {
        printf("Enter event date (DD.MM.YYYY): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (isValidEventDate(buffer)) {
            strcpy(newEvent.date, buffer);
            break;
        }

        printf("Please use DD.MM.YYYY format, for example 16.10.2025.\n");
    }

    while (1) {
        printf("Enter event time (HH:MM): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (isValidEventTime(buffer)) {
            strcpy(newEvent.time, buffer);
            break;
        }

        printf("Incorrect format, please use HH:MM, eg 16:30.\n");
    }

    while (1) {
        printf("Enter location: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (isValidEventLocation(buffer)) {
            strcpy(newEvent.location, buffer);
            break;
        }

        printf("Please only use Latin letters, numbers, spaces, commas, and dashes. Max 50 characters.\n");
    }

    while (1) {
        printf("Enter max seat row (A-Z): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) == 1 && buffer[0] >= 'A' && buffer[0] <= 'Z') {
            newEvent.maxSeatRow = buffer[0];
            break;
        }

        printf("Please use an uppercase letter A-Z, for example V.\n");
    }

    while (1) {
        printf("Enter max seat number (1-20): ");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &newEvent.maxSeatNumber) == 1 &&
            isValidEventMaxSeatNumber(newEvent.maxSeatNumber)) {
            break;
        }

        printf("Please use a number from 1-20.\n");
    }

    while (1) {
        printf("Enter price (0.00-100): ");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%lf", &newEvent.price) == 1 &&
            isValidEventPrice(newEvent.price)) {
            break;
        }

        printf("Price must be between 0.00 and 100.00.\n");
    }

    newEvent.id = nextEventID;
    nextEventID++;

    resizeEventListIfNeeded(events);

    // now, confirmed we have space in the events list
    // therefore, add event / insert event
    events->data[events->count] = newEvent;
    events->count++;

    saveAllEvents(events->data, events->count);

    printf("Event saved successfully!\n");
    printEvent(&newEvent);
}
