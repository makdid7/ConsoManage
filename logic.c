#include "logic.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"


double getCostWithDiscount(const double originalCost, const User *user) {
    if (user->age <= 12) {
        return originalCost * (1.0 - 0.4);
    }
    if (user->age <= 18) {
        return originalCost * (1.0 - 0.2);
    }
    if (user->age >= 65) {
        return originalCost * (1.0 - 0.45);
    }

    return originalCost;
}



int isSeatTaken(const UserList *users, const Event *event, const char row,
                       const int seatNumber) {
    for (int i = 0; i < users->count; i++) {
        for (int j = 0; j < users->data[i].ticketsCount; j++) {
            const Ticket ticket = users->data[i].ticketsOwned[j];
            if (ticket.eventID == event->id && ticket.row == row &&
                ticket.seatNumber == seatNumber) {
                return 1;
            }
        }
    }
    return 0;
}

int isValidEmail(const char *email) {
    const size_t len = strlen(email);
    if (len == 0 || len > 50)
        return 0;
    const char *at = strchr(email, '@');
    if (!at)
        return 0;
    // simple check: at least one char before and after '@'
    if (at == email)
        return 0;
    if (*(at + 1) == '\0')
        return 0;
    return 1;
}


char* generateSeatmap(const UserList* users, const Event* event) {
    const int rows = event->maxSeatRow - 'A' + 1;
    const int seats = event->maxSeatNumber;

    const int cellSize = 3; // so accommodates for like "05\0" or "--\0"
    char* seatmap = malloc(rows * seats * cellSize);
    if (!seatmap) {
        printf("Couldn't allocate memory for generateSeatmap, exiting ConsoManage...\n");
        exit(1);
    }

    for (char row = 'A'; row <= event->maxSeatRow; row++) {
        for (int seat = 1; seat <= seats; seat++) {

            const int rowIndex = row - 'A';
            const int index = (rowIndex * seats + (seat - 1)) * cellSize;

            if (isSeatTaken(users, event, row, seat)) {
                snprintf(&seatmap[index], cellSize, "--");
            } else {
                snprintf(&seatmap[index], cellSize, "%02d", seat);
            }
        }
    }

    return seatmap;
}


int getMaxEventID(const EventList *events) {
    if (events->count == 0) {
        return 0; // If no events, max ID is 0
    }

    int maxID = 0;
    for (int i = 0; i < events->count; i++) {
        if (events->data[i].id > maxID) {
            maxID = events->data[i].id;
        }
    }
    return maxID;
}


int nearestPowerOfTwo(const int x) {
    // get nearest integer closest to a power of two (nearest cannot be lower than reference value)
    int p = 1;

    while (p <= x) {
        p *= 2;
    }

    return p;
}

void initEventData(EventList *events, const int initialEventCount) {
    // gives default values to all allocated (but not yet used) events

    events->count = initialEventCount;
    events->capacity = nearestPowerOfTwo(events->count);

    for (int i = 0; i < events->count; i++) {
        // accessing data in the "(*(arr + i)).reserve" way
        Event *e = events->data + i; // pointer to the i-th Event (pointer arithmetic! :D)

        e->id = 0;
        e->name[0] = '\0';
        e->date[0] = '\0';
        e->time[0] = '\0';
        e->location[0] = '\0';
        e->maxSeatRow = 'A';
        e->maxSeatNumber = 0;
        e->price = 0.0;
    }
}


void initUserList(UserList *users, const int userCount) {

    // gives default values to all allocated (but not yet used) users

    users->count = userCount;
    users->capacity = nearestPowerOfTwo(users->count);

    for (int i = 0; i < users->capacity; i++) {
        User *u = users->data + i; // pointer to the i-th User

        u->fullName[0] = '\0';
        u->age = 0;
        u->email[0] = '\0';
        u->ticketsCount = 0;
        u->ticketsCapacity = 0;
        u->ticketsOwned = NULL;
    }
}

