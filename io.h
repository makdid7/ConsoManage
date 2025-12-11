#ifndef IO_H
#define IO_H

#include "models.h"

// format validation
// print data to console
// read raw user input
// report success/failure of input format
// uses Models only, no Logic, no FileIO

void printUser(User *user);

void printEvent(Event *event);

void printUserTickets(User *user, EventList *events);

void printSeatmap(char* seatmap, char maxSeatRow, int maxSeatNumber);

#endif //IO_H
