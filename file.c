//
// Created by mkd on 11/28/25.
//

#include "file.h"

#include <stdio.h>
#include <stdlib.h>

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

int deleteAllEvents() {

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