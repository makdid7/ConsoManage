#ifndef LOGIC_H
#define LOGIC_H

#include "models.h"

double getCostWithDiscount(double originalCost, const User *user);

char* generateSeatmap(const UserList* users, const Event* event);

#endif //LOGIC_H
