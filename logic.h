#ifndef LOGIC_H
#define LOGIC_H

#include "models.h"

double getCostWithDiscount(double originalCost, const User *user);

char* generateSeatmap(const UserList* users, const Event* event);

int nearestPowerOfTwo(int x);

void initEventData(EventList *events, int initialEventCount);

void initUserList(UserList *users, int userCount);

void setNextEventID(int newID);

int getMaxEventID(const EventList *events);

#endif //LOGIC_H
