//
// Created by mkd on 11/28/25.
//

#ifndef FILE_H
#define FILE_H
#include "models.h"

int getEventCount();

int loadAllEvents(Event repo[]);

int saveAllEvents(Event repo[], int count);

int clearEventsFile();

void resizeEventListIfNeeded(EventList *events);

void resizeUserListIfNeeded(UserList *users);

void resizeTicketsOwnedListIfNeeded(User *currentUser);

void freeAllDynamicMemory(EventList *events, UserList *users);

EventList *createEventContainer();

Event *createEventData(int size);

User *createUserData(int size);

UserList *createUserContainer();

void deleteAllEvents(EventList *events);

void deleteAllUsers(UserList *users);

#endif // FILE_H
