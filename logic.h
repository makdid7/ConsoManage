#ifndef LOGIC_H
#define LOGIC_H

#include "models.h"

/* business logic */
double getCostWithDiscount(const double originalCost, const User *user);

int isSeatTaken(const UserList *users, const Event *event, char row, int seatNumber);

char *generateSeatmap(const UserList *users, const Event *event);

/* utilities */
int getMaxEventID(const EventList *events);

int nearestPowerOfTwo(const int x);

/* initialization */
void initEventData(EventList *events, const int initialEventCount);

void initUserList(UserList *users, const int userCount);

#endif // LOGIC_H
