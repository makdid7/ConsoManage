#ifndef CONSOMANAGE_LOGIC_H
#define CONSOMANAGE_LOGIC_H

#include "models.h"

inline int isValidEventName(char name[51]);

inline int isValidEventLocation(char loc[51]);

inline int isValidEventDate(char date[11]);

inline int isValidEventTime(char time[6]);

inline int isValidEventMaxSeatNumber(int num);

inline int isValidEventPrice(double price);

inline double getCostWithDiscount(double originalCost, const User *user);

inline int isValidRow(const Event *event, char row);

inline int isValidEventID(int id, const EventList *events);

inline int isSeatTaken(const UserList *users, const Event *event, char row,
                       int seatNumber);

inline int isValidEmail(const char *email);

inline char* generateSeatmap(User* users, Event* event);

#endif //CONSOMANAGE_LOGIC_H
