//
// Created by mkd on 11/28/25.
//

#include "file.h"

#include <stdio.h>
#include <string.h>

#include "models.h"

int loadAll(Event repo[]) {
    FILE* f = fopen("data.txt", "r");
    if (f == NULL) return 0;

    int count = 0;
    char line[300];

    while (fgets(line, sizeof(line), f) != NULL) {
        Event e;
        const int parsed = sscanf(
            line,
            "%d;%51[^;];%11[^;];%6[^;];%51[^;];%c;%d;%lf",
            &e.id,
            e.name,
            e.date,
            e.time,
            e.location,
            &e.maxSeatRow,
            &e.maxSeatNumber,
            &e.price
        );

        if (parsed != 8) continue;

        repo[count++] = e;
    }

    fclose(f);
    return count;
}

int saveAll(Event repo[], const int count) {
    FILE* f = fopen("data.txt", "w");
    if (f == NULL) return 1;

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
