#ifndef MODELS_H
#define MODELS_H

typedef struct {
    int id;
    char name[52];
    char date[12];
    char time[6];
    char location[52];
    char maxSeatRow;
    int maxSeatNumber;
    double price;
} Event;

typedef struct {
    char ownerFullName[76];
    char row;
    int seatNumber;
    int eventID;
    int id;
    char ticketReference[16];
    double pricePaid;
} Ticket;

typedef struct {
    char fullName[76];
    int age;
    char email[52];
    int ticketsCount;
    int ticketsCapacity;
    Ticket *ticketsOwned;
} User;

typedef enum {
    PARTICIPANT = 1,
    ORGANIZER = 2
} UserType;

typedef struct {
    Event *data;
    int count;
    int capacity;
} EventList;

typedef struct {
    User *data;
    int count;
    int capacity;
} UserList;

/* validation helpers */
int isValidEventName(char name[51]);

int isValidEventLocation(char loc[51]);

int isValidEventDate(char date[11]);

int isValidEventTime(char time[6]);

int isValidEventMaxSeatNumber(const int num);

int isValidEventPrice(const double price);

int isValidRow(const Event *event, const char row);

int isValidEventID(const int id, const EventList *events);

int isValidFullName(char *fullName);

int isValidEmail(const char *email);

#endif // MODELS_H
