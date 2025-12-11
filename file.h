//
// Created by mkd on 11/28/25.
//

#ifndef FILE_H
#define FILE_H
#include "models.h"

int getEventCount();

int loadAllEvents(Event repo[]);

int saveAllEvents(Event repo[], int count);

int deleteAllEvents();

void resizeEventListIfNeeded(EventList *events);

void freeAllDynamicMemory(EventList *events, UserList *users);

#endif // FILE_H
