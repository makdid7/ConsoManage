#include "logic.h"

#include <ctype.h>
#include <string.h>




inline double getCostWithDiscount(const double originalCost, const User *user) {
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



inline int isSeatTaken(const UserList *users, const Event *event, const char row,
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

inline int isValidEmail(const char *email) {
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

inline char* generateSeatmap(User* users, Event* event) {
    char* seatmap;
    // for (char row = 'A'; row <= event->maxSeatRow; row++) {
    //     printf("%c: ", row);
    //     for (int seat = 1; seat <= event.maxSeatNumber; seat++) {
    //         if (isSeatTaken(users, &event, row, seat)) {
    //             printf("--");
    //         } else {
    //             printf("%02d", seat);
    //         }
    //         printf(" ");
    //     }
    //     printf("\n");
    // }

    return seatmap;
}