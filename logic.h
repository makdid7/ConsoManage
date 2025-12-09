#ifndef CONSOMANAGE_LOGIC_H
#define CONSOMANAGE_LOGIC_H

#include "models.h"

inline double getCostWithDiscount(double originalCost, const User *user);

inline char *generateSeatmap(User *users, Event *event);

#endif //CONSOMANAGE_LOGIC_H
