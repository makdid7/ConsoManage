#ifndef FILE_H
#define FILE_H

#include "models.h"

/* persistence */
int getEventCount();

int loadAllEvents(Event repo[]);

int saveAllEvents(Event repo[], const int count);

int clearEventsFile();

/* containers & memory */
EventList *createEventContainer();

Event *createEventData(const int size);

UserList *createUserContainer();

User *createUserData(const int size);

void resizeEventListIfNeeded(EventList *events);

void resizeUserListIfNeeded(UserList *users);

void resizeTicketsOwnedListIfNeeded(User *currentUser);

void deleteAllEvents(EventList *events);

void deleteAllUsers(UserList *users);

void freeAllDynamicMemory(EventList *events, UserList *users);

#endif // FILE_H
