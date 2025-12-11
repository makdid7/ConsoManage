//
// Created by mkd on 11/28/25.
//

#include "file.h"

#include <stdio.h>
#include <stdlib.h>

#include "logic.h"
#include "models.h"


int getEventCount() {
    FILE *f = fopen("data.txt", "r");
    if (f == NULL) return 0;

    int count = 0;
    char line[500];

    while (fgets(line, sizeof(line), f) != NULL) {
        Event e;
        const int parsed = sscanf(
            line,
            "%d;%51[^;];%11[^;];%5[^;];%51[^;];%c;%d;%lf",
            &e.id,
            e.name,
            e.date,
            e.time,
            e.location,
            &e.maxSeatRow,
            &e.maxSeatNumber,
            &e.price
        );

        if (parsed != 8) {
            continue;
        }

        count++;
    }

    fclose(f);
    return count;
}


int loadAllEvents(Event repo[]) {
    FILE *f = fopen("data.txt", "r");

    if (f == NULL) {
        printf("No data file! Creating empty list...\n");
        return 0;
    }

    int count = 0;
    int anyCount = 0;
    char line[300];

    while (fgets(line, sizeof(line), f) != NULL) {
        Event e;
        const int parsed = sscanf(
            line,
            "%d;%51[^;];%11[^;];%5[^;];%51[^;];%c;%d;%lf",
            &e.id,
            e.name,
            e.date,
            e.time,
            e.location,
            &e.maxSeatRow,
            &e.maxSeatNumber,
            &e.price
        );

        anyCount++;

        if (parsed != 8) {
            printf("Line %d parsing failed, skipping...\n", anyCount);
            continue;
        }

        repo[count++] = e;
    }

    printf("Parsed %d data entries.\n", count);

    fclose(f);
    return count;
}


int saveAllEvents(Event repo[], const int count) {
    FILE *f = fopen("data.txt", "w");
    if (f == NULL) {
        printf("Failed to open data file for writing\n");
        return 1;
    }

    for (int i = 0; i < count; i++) {
        Event e = repo[i];
        fprintf(
            f,
            "%d;%s;%s;%s;%s;%c;%d;%.2f\n",
            e.id,
            e.name,
            e.date,
            e.time,
            e.location,
            e.maxSeatRow,
            e.maxSeatNumber,
            e.price
        );
    }

    fclose(f);
    return 0;
}

void resizeEventListIfNeeded(EventList *events) {
    if (events->count >= events->capacity) {
        const int newEventsListCapacity = events->capacity * 2;
        Event *temp = realloc(events->data, newEventsListCapacity * sizeof(Event));
        if (temp != NULL) {
            events->data = temp;
            events->capacity = newEventsListCapacity;
        } else {
            printf("Oops, failed to reallocate enough memory to resize events list. Exiting ConsoManage...");
            exit(1);
        }
    }
}
void resizeUserListIfNeeded(UserList *users) {
    if (users->count >= users->capacity) {
        // we're reaching the user limit; let's resize to allow for more.
        users->capacity *= 2;
        User *tmp = realloc(users->data, sizeof(User) * users->capacity);
        if (tmp == NULL) {
            // realloc failed
            printf("Sorry, we failed to reallocate memory to accommodate the new user! Exiting ConsoManage...");
            exit(1);
        }
        // realloc didn't fail; assign new memory to us
        users->data = tmp;
    }
}
void resizeTicketsOwnedListIfNeeded(User* currentUser) {
    if (currentUser->ticketsCount >= currentUser->ticketsCapacity) {
        const int newCap = (currentUser->ticketsCapacity == 0)
                               ? 2
                               : currentUser->ticketsCapacity * 2;
        Ticket *tmp = realloc(currentUser->ticketsOwned, newCap * sizeof(Ticket));
        if (!tmp) {
            printf("Failed to allocate memory for tickets!\n");
            exit(1);
        }
        currentUser->ticketsOwned = tmp;
        currentUser->ticketsCapacity = newCap;
    }
}

void freeAllDynamicMemory(EventList *events, UserList *users) {
    free(events->data);
    events->data = NULL;
    free(events);
    events = NULL;

    free(users->data);
    users->data = NULL;
    free(users);
    users = NULL;
}

int clearEventsFile() {
    // Opening in "w" mode automatically truncates (clears) the file to zero length.
    FILE *f = fopen("data.txt", "w");

    if (f == NULL) {
        // Failed to open the file, maybe permission or path issue
        printf("Failed to open data file for clearing :(\n");
        return 0; // Failure
    }

    // File cleared, close immediately
    fclose(f);
    return 1; // Notify function-caller of success
}

EventList *createEventContainer() {
    EventList *tmp = malloc(sizeof(EventList));
    if (tmp == NULL) {
        printf("#createEventList Failed to allocate memory for events ER-Light "
            "container :(\nExiting ConsoManage...");
        exit(1);
    }

    return tmp;
}

Event *createEventData(const int size) {
    Event *tmp = malloc(sizeof(Event) * size);
    if (tmp == NULL) {
        printf("#createEventList Failed to allocate memory for events->data "
            ":(\nExiting ConsoManage...");
        exit(1);
    }

    return tmp;
}


User *createUserData(const int size) {
    User *tmp = malloc(sizeof(User) * size);
    if (tmp == NULL) {
        printf("#createEventList Failed to allocate memory for users->data "
            ":(\nExiting ConsoManage...");
        exit(1);
    }

    return tmp;
}

UserList *createUserContainer() {
    UserList *tmp = malloc(sizeof(UserList));
    if (tmp == NULL) {
        printf("#createUserList Failed to allocate memory for users ER-Light "
            "container :(\nExiting ConsoManage...");
        exit(1);
    }

    return tmp;
}

void deleteAllEvents(EventList *events) {
    free(events->data);
    events->data = NULL;
    events->count = 0;
    events->capacity = 1;
    Event *tmp = malloc(events->capacity * sizeof(Event));
    if (tmp != NULL) {
        events->data = tmp;
        initEventData(events, 0); // Re-init data
        clearEventsFile(); // delete from data file as well
        printf("All events have been deleted.\n");
    } else {
        printf("Failed to malloc! Quitting ConsoManage...");
        exit(1);
    }
}

void deleteAllUsers(UserList *users) {
    // Free ticketsOwned for each user
    for (int i = 0; i < users->count; i++) {
        free(users->data[i].ticketsOwned);
    }
    free(users->data);
    users->data = NULL;
    users->count = 0;
    users->capacity = 1;
    User *tmp = malloc(users->capacity * sizeof(User));
    if (tmp != NULL) {
        users->data = tmp;
        initUserList(users, 0); // Re-init data
        printf("All users have been deleted.\n");
    } else {
        printf("Failed to malloc! Quitting ConsoManage...");
        exit(1);
    }
}
