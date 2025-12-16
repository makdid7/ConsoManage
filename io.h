#ifndef IO_H
#define IO_H

#include "models.h"

/* UI helpers */
void printUser(User *user);

void printEvent(Event *event);

void printUserTickets(User *user, EventList *events);

void printSeatmap(char *seatmap, char maxSeatRow, int maxSeatNumber);

/* UI flows */
void participantFlow(EventList *events, UserList *users);

void organizerFlow(EventList *events, UserList *users);

void createNewEvent(EventList *events);

/* shared UI state */
void setNextEventID(int newID);

#endif // IO_H
