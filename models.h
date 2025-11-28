//
// Created by mkd on 11/28/25.
//

#ifndef CONSOMANAGE_MODELS_H
#define CONSOMANAGE_MODELS_H

typedef struct {
    char ownerFullName[75];
    char row;
    int seatNumber;
    int id;
    char ticketReference[30];
    double pricePaid;
    int eventID;
} Ticket;

typedef struct User {
    char fullName[75];
    int age;
    char email[51];
    Ticket *ticketsOwned;
    int ticketsCount;
    int ticketsCapacity;
} User;

typedef struct Event {
    int id;
    char name[52];
    char date[12];
    char time[7];
    char location[52];
    char maxSeatRow;
    int maxSeatNumber;
    double price;
} Event;

typedef struct EventList {
    Event *data;
    int count;
    int capacity;
} EventList;

typedef struct UserList {
    User *data;
    int count;
    int capacity;
} UserList;

typedef enum {
    PARTICIPANT = 1,
    ORGANIZER = 2
} UserType;

#endif //CONSOMANAGE_MODELS_H