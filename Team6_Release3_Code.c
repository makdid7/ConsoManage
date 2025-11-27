#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// hello
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

const int eventListInitialCapacity = 1;
const int userListInitialCapacity = 1;

int nextTicketID = 1;
int nextEventID = 1;

void initEventData(EventList *events);

void initUserList(UserList *users);

// -----Input Validation -----

int isValidEventName(char name[51]) {
    const int len = strlen(name);
    if (len == 0 || len > 50) {
        return 0;
    }

    for (int i = 0; i < len; i++) {
        char c = name[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') || c == ' ' || c == '-' || c == ',' ||
              isdigit(c))) {
            return 0;
        }
    }

    return 1;
}

int isValidEventLocation(char loc[51]) {
    const int len = strlen(loc);
    if (len == 0 || len > 50) {
        return 0;
    }

    for (int i = 0; i < len; i++) {
        char c = loc[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ' ||
              c == '-' || c == ',' || isdigit(c))) {
            return 0;
        }
    }

    return 1;
}

int isValidEventDate(char date[11]) {
    if (strlen(date) != 10) {
        return 0;
    }

    if (date[2] != '.' || date[5] != '.') {
        return 0;
    }

    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) {
            continue;
        }
        if (!isdigit(date[i])) {
            return 0;
        }
    }

    // have to do all this because our format includes pad zeros
    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 +
               (date[8] - '0') * 10 + (date[9] - '0');

    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 2025 ||
        year > 2075) {
        return 0;
    }

    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
        return 0;
    }

    if (month == 2) {
        int leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        if ((!leap && day > 28) || (leap && day > 29)) {
            return 0;
        }
    }

    return 1;
}

int isValidEventTime(char time[6]) {
    if (strlen(time) != 5) {
        return 0;
    }

    if (time[2] != ':') {
        return 0;
    }

    if (!isdigit(time[0]) || !isdigit(time[1]) || !isdigit(time[3]) ||
        !isdigit(time[4])) {
        return 0;
    }

    int hour = (time[0] - '0') * 10 + (time[1] - '0');
    int minute = (time[3] - '0') * 10 + (time[4] - '0');

    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        return 0;
    }

    return 1;
}

int isValidEventMaxSeatNumber(const int num) {
    if (num >= 1 && num <= 20) {
        return 1;
    }
    return 0;
}

int isValidEventPrice(const double price) {
    if (price >= 0 && price <= 100.0) {
        return 1;
    }
    return 0;
}

// -----Business Logic-----

double getCostWithDiscount(const double originalCost, const User *user) {
    if (user->age <= 12) {
        return originalCost * (1.0 - 0.4);
    }
    if (user->age <= 18) {
        return originalCost * (1.0 - 0.2);
    }
    if (user->age >= 65) {
        return originalCost * (1.0 - 0.45);
    }

    return originalCost;
}

void printEvent(Event *event) {
    // if (event->maxSeatNumber == 0) {
    //     return;
    //     // under normal circumstances, this cannot happen; we return because
    //     malloc gives memory to a non-existing event at the beginning of the
    //     program.
    // }
    printf("\n------------- Event Details -------------\n");
    printf("ID:       %04d\n", event->id);
    printf("Name:     %s\n", event->name);
    printf("Date:     %s\n", event->date);
    printf("Time:     %s\n", event->time);
    printf("Location: %s\n", event->location);
    printf("Seats:    %c01-%c%02d\n", 'A', event->maxSeatRow,
           event->maxSeatNumber);
    if (event->price == 0) {
        printf("Price:    Free\n");
    } else {
        printf("Price:    %.2lf\n", event->price);
    }
    printf("-----------------------------------------\n\n");
}

void printUser(User *user) {
    printf("\n------------- User Details -------------\n");
    printf("Full name: %s\n", user->fullName);
    printf("Age:       %d\n", user->age);
    printf("E-mail:    %s\n", user->email);
    printf("----------------------------------------\n\n");
}

int isValidRow(const Event *event, const char row) {
    if (row >= 'A' && row <= event->maxSeatRow) {
        return 1;
    }
    return 0;
}

// -----Event Creation -----

void createNewEvent(EventList *events) {
    Event newEvent;
    char buffer[100];

    while (1) {
        printf("Enter event name: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (isValidEventName(buffer)) {
            strcpy(newEvent.name, buffer);
            break;
        }

        printf("Please only use Latin characters, numbers, dashes, commas, and spaces.\n");
    }

    while (1) {
        printf("Enter event date (DD.MM.YYYY): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (isValidEventDate(buffer)) {
            strcpy(newEvent.date, buffer);
            break;
        }

        printf("Please use DD.MM.YYYY format, for example 16.10.2025.\n");
    }

    while (1) {
        printf("Enter event time (HH:MM): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (isValidEventTime(buffer)) {
            strcpy(newEvent.time, buffer);
            break;
        }

        printf("Incorrect format, please use HH:MM, eg 16:30.\n");
    }

    while (1) {
        printf("Enter location: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (isValidEventLocation(buffer)) {
            strcpy(newEvent.location, buffer);
            break;
        }

        printf("Please only use Latin letters, numbers, spaces, commas, and dashes. Max 50 characters.\n");
    }

    while (1) {
        printf("Enter max seat row (A-Z): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) == 1 && buffer[0] >= 'A' && buffer[0] <= 'Z') {
            newEvent.maxSeatRow = buffer[0];
            break;
        }

        printf("Please use an uppercase letter A-Z, for example V.\n");
    }

    while (1) {
        printf("Enter max seat number (1-20): ");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &newEvent.maxSeatNumber) == 1 &&
            isValidEventMaxSeatNumber(newEvent.maxSeatNumber)) {
            break;
        }

        printf("Please use a number from 1-20.\n");
    }

    while (1) {
        printf("Enter price (0.00-100): ");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%lf", &newEvent.price) == 1 &&
            isValidEventPrice(newEvent.price)) {
            break;
        }

        printf("Price must be between 0.00 and 100.00.\n");
    }

    newEvent.id = nextEventID;
    nextEventID++;

    // if (events->count >= maxEvents) {
    //     // we're reaching the event limit; let's resize to allow for 1 more.
    //     maxEvents++;
    //     Event *temp = realloc(EventList->data, sizeof(Event) * maxEvents);
    //     if (!temp) {
    //         // realloc failed
    //         printf("Sorry, we failed to reallocate memory to accommodate the
    //         new event!"); free(events); exit(1);
    //     }
    //     // realloc didn't fail; assign new memory to events
    //     events = temp;
    // }

    // resizing the dynamic events list, if needed

    if (events->count >= events->capacity) {
        // though in theory, ">" shouldn't ever happen
        const int newEventsListCapacity = events->capacity * 2;
        Event *temp = realloc(events->data, newEventsListCapacity * sizeof(Event));
        if (temp != NULL) {
            events->data = temp;
            events->capacity = newEventsListCapacity;
        } else {
            printf("Oops, failed to reallocate enough memory to resize events list. ConsoManage will now close.");
            exit(1);
        }
    }

    // confirmed we have space in the events list
    // add event / insert event
    events->data[events->count] = newEvent;
    events->count++;

    // in release 2 we had: events[eventCount] = event;
    // for dynamic memory we could use pointer arithmetic like this:
    // Event *newEventPointer = events->data + events->count;
    // works cuz events.data refers to address of first element of the dynamic
    // list of events
    // *newEventPointer = newEvent; //assign our new event to the memory at
    // address [first event]+[total # of events]

    printf("Event created successfully!\n");
    printEvent(&newEvent);
}

// -----Participant Flow -----

int isValidEventID(const int id, const EventList *events) {
    if (id == -1) {
        return 1;
    }

    if (id < -1) {
        return 0;
    }

    for (int i = 0; i < events->count; i++) {
        if (events->data[i].id == id) {
            return 1;
        }
    }
    return 0;
}

int isSeatTaken(const UserList *users, const Event *event, const char row,
                const int seatNumber) {
    for (int i = 0; i < users->count; i++) {
        for (int j = 0; j < users->data[i].ticketsCount; j++) {
            const Ticket ticket = users->data[i].ticketsOwned[j];
            if (ticket.eventID == event->id && ticket.row == row &&
                ticket.seatNumber == seatNumber) {
                return 1;
            }
        }
    }
    return 0;
}

void listUserTickets(User *user, const EventList *events) {
    printf("\n---------- Tickets Owned by %s ----------\n", user->fullName);
    if (user->ticketsCount == 0) {
        printf("No tickets purchased yet.\n");
    } else {
        for (int i = 0; i < user->ticketsCount; i++) {
            Ticket ticket = user->ticketsOwned[i];
            Event event;
            int found = 0;
            for (int k = 0; k < events->count; k++) {
                if (events->data[k].id == ticket.eventID) {
                    event = events->data[k];
                    found = 1;
                    break;
                }
            }

            printf("Ticket Reference: %s\n", ticket.ticketReference);
            if (found) {
                printf("Event: %s\n", event.name);
            } else {
                printf("Event: Unknown\n");
            }
            printf("Seat: %c%02d\n", ticket.row, ticket.seatNumber);
            printf("Price Paid: %.2lf\n\n", ticket.pricePaid);
        }
    }
    printf("------------------------------------------\n");
}

int isValidEmail(const char *email) {
    size_t len = strlen(email);
    if (len == 0 || len > 50)
        return 0;
    const char *at = strchr(email, '@');
    if (!at)
        return 0;
    // simple check: at least one char before and after '@'
    if (at == email)
        return 0;
    if (*(at + 1) == '\0')
        return 0;
    return 1;
}

// -----Utility-----

void printSeatMap(const UserList *users, const Event *event) {
    printf("------------- Seat Map -------------\n");

    for (char row = 'A'; row <= event->maxSeatRow; row++) {
        printf("%c: ", row);
        for (int seat = 1; seat <= event->maxSeatNumber; seat++) {
            if (isSeatTaken(users, event, row, seat)) {
                printf("-- ");
            } else {
                printf("%02d ", seat);
            }
        }
        printf("\n");
    }

    printf("-----------------------------------\n\n");
}

void participantFlow(const EventList *events, UserList *users) {
    char buffer[100];
    User user;

    // -----Enter Participant Info-----

    while (1) {
        printf("Enter full name: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        int len = strlen(buffer);
        int valid = 1;
        for (int i = 0; i < len; i++) {
            char c = buffer[i];
            if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ' ||
                  c == '-' || c == ',')) {
                valid = 0;
                break;
            }
        }

        if (len > 75) {
            valid = 0;
        }

        if (valid) {
            strcpy(user.fullName, buffer);
            break;
        }

        printf("Please only use Latin characters, dashes, spaces, or comma, max 75 chars.\n");
    }

    while (1) {
        printf("Enter age: ");
        fgets(buffer, sizeof(buffer), stdin);
        int age;
        if (sscanf(buffer, "%d", &age) == 1 && age >= 0 && age <= 110) {
            user.age = age;
            break;
        }
        printf("Age must be 0-110 years.\n");
    }

    while (1) {
        printf("Enter email: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (isValidEmail(buffer)) {
            strcpy(user.email, buffer);
            break;
        }

        printf("Invalid email address.\n");
    }

    if (users->count >= users->capacity) {
        // we're reaching the user limit; let's resize to allow for more.
        users->capacity *= 2;
        User *tmp = realloc(users->data, sizeof(User) * users->capacity);
        if (tmp == NULL) {
            // realloc failed
            printf(
                "Sorry, we failed to reallocate memory to accommodate the new user!");
            exit(1);
        }
        // realloc didn't fail; assign new memory to us
        users->data = tmp;
    }

    user.ticketsCount = 0;
    user.ticketsCapacity = 0;
    user.ticketsOwned = NULL;
    const int currentUserIndex = users->count;

    User *newUserPointer = users->data + users->count;
    // works cuz `users->data` refers to address of first element of `users`
    *newUserPointer = user; // assign our new user to the memory at address [first user]+[total # of users]
    users->count++;

    printf("User created successfully!\n");
    printUser(&user);

    // -----Participant Menu-----

    int choice;
    while (1) {
        printf("\nParticipant Menu:\n");
        printf("[1] See all events\n");
        printf("[2] Register for an event\n");
        printf("[3] List my tickets\n");
        printf("[4] Exit to main menu\n");
        printf("Choice: ");
        fgets(buffer, sizeof(buffer), stdin);

        if (sscanf(buffer, "%d", &choice) != 1) {
            printf("Invalid input. Try again.\n");
            continue;
        }

        switch (choice) {
            case 1: {
                if (events->count == 0) {
                    printf("Nothing is happening at the moment.\n");
                }
                for (int i = 0; i < events->count; i++) {
                    printEvent(&(events->data[i]));
                }
                break;
            }
            case 2: {
                int eventID;
                while (1) {
                    printf("Enter Event ID: ");
                    fgets(buffer, sizeof(buffer), stdin);
                    if (sscanf(buffer, "%d", &eventID) == 1 &&
                        isValidEventID(eventID, events)) {
                        break;
                    }
                    printf("Invalid Event ID. Try again.\n");
                }

                Event event;
                int found = 0;
                for (int i = 0; i < events->count; i++) {
                    if (events->data[i].id == eventID) {
                        event = events->data[i];
                        found = 1;
                        break;
                    }
                }

                if (!found) {
                    printf("Event not found.\n");
                    continue;
                }

                printSeatMap(users, &event);

                char row;
                int seatNumber;
                while (1) {
                    printf("Enter row (A-Z): ");
                    fgets(buffer, sizeof(buffer), stdin);
                    if (sscanf(buffer, "%c", &row) == 1 && isValidRow(&event, row)) {
                        break;
                    }
                    printf("Please use uppercase letter A-Z. Make sure row %c exists.\n",
                           buffer[0]);
                }

                while (1) {
                    printf("Enter seat number (1-20): ");
                    fgets(buffer, sizeof(buffer), stdin);
                    if (sscanf(buffer, "%d", &seatNumber) == 1 && seatNumber >= 1 &&
                        seatNumber <= event.maxSeatNumber) {
                        if (isSeatTaken(users, &event, row, seatNumber)) {
                            printf("Seat already taken. Please choose another.\n");
                        } else {
                            break;
                        }
                    } else {
                        printf("Please use a number from 1-20.\n");
                    }
                }

                Ticket ticket;
                strcpy(ticket.ownerFullName, user.fullName);
                ticket.row = row;
                ticket.seatNumber = seatNumber;
                ticket.eventID = event.id;
                ticket.id = nextTicketID;
                nextTicketID++;
                sprintf(ticket.ticketReference, "TK-%04d%c%02d", event.id, row, seatNumber);
                ticket.pricePaid = getCostWithDiscount(event.price, &user);

                // Dynamic Allocation for ticketsOwned
                User *currentUser = &users->data[currentUserIndex];
                if (currentUser->ticketsCount >= currentUser->ticketsCapacity) {
                    const int newCap = (currentUser->ticketsCapacity == 0)
                                     ? 2
                                     : currentUser->ticketsCapacity * 2;
                    Ticket *tmp = realloc(currentUser->ticketsOwned, newCap * sizeof(Ticket));
                    if (!tmp) {
                        printf("Failed to allocate memory for tickets!\n");
                        exit(1);
                    }
                    currentUser->ticketsOwned = tmp;
                    currentUser->ticketsCapacity = newCap;
                }

                currentUser->ticketsOwned[currentUser->ticketsCount] = ticket;
                currentUser->ticketsCount++;

                printf("Ticket booked successfully! Reference: %s, Price: %.2lf\n",
                       ticket.ticketReference, ticket.pricePaid);
                break;
            }
            case 3: {
                listUserTickets(&users->data[currentUserIndex], events);
                break;
            }
            case 4: {
                return;
            }
            default: {
                printf("Invalid choice.\n");
                break;
            }
        }
    }
}

void organizerFlow(EventList *events, UserList *users) {
    char buffer[100];
    while (1) {
        printf("\nOrganizer Menu:\n");
        printf("[1] Create a new event.\n");
        printf("[2] View all existing events.\n");
        printf("[3] Return to the main menu.\n");
        printf("[X] Delete all events. This is irreversible.\n");
        printf("[Y] Delete all users. This is irreversible.\n");
        printf("Choice: ");

        char organizerChoice;
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%c", &organizerChoice) != 1) {
            printf("Invalid input.\n\n");
        } else {
            if (organizerChoice == 'X') {
                free(events->data);
                events->data = NULL;
                events->count = 0;
                events->capacity = eventListInitialCapacity;
                Event *tmp = malloc(events->capacity * sizeof(Event));
                if (tmp != NULL) {
                    events->data = tmp;
                    initEventData(events); // Re-init data
                    printf("All events have been deleted.\n");
                } else {
                    printf("Failed to malloc! Quitting ConsoManage...");
                    exit(1);
                }
            } else if (organizerChoice == 'Y') {
                // Free ticketsOwned for each user (R4: might move to another choice)
                for (int i = 0; i < users->count; i++) {
                    free(users->data[i].ticketsOwned);
                }
                free(users->data);
                users->data = NULL;
                users->count = 0;
                users->capacity = userListInitialCapacity;
                User *tmp = malloc(users->capacity * sizeof(User));
                if (tmp != NULL) {
                    users->data = tmp;
                    initUserList(users); // Re-init data
                    printf("All users have been deleted.\n");
                } else {
                    printf("Failed to malloc! Quitting ConsoManage...");
                    exit(1);
                }
            } else if (organizerChoice == '1') {
                createNewEvent(events);
            } else if (organizerChoice == '2') {
                if (events->count <= 0) {
                    printf("There are no existing events.\n");
                } else {
                    for (int i = 0; i < events->count; i++) {
                        printEvent(&events->data[i]);
                    }
                }
            } else if (organizerChoice == '3') {
                break;
            }
        }
    }
}

// void resetEventList(EventList *events) {
//     // reset the heap / inner contents of the dynamic events list
//
//     free(events->data);
//     events->data = NULL;
//
//     events->count = 0;
//     events->capacity = eventListInitialCapacity;
//
//     Event *temp = malloc(sizeof(Event) * events->capacity);
//     if (temp != NULL) {
//         events->data = temp;
//     } else {
//         printf("Sorry, we failed to allocate memory for `events->data`!
//         Closing ConsoManage..."); exit(1);
//     }
// }
//
//
// void resetUserList(UserList *users) {
//     // reset the heap / inner contents of the dynamic events list
//
//     free(users->data);
//     users->data = NULL;
//
//     users->count = 0;
//     users->capacity = userListInitialCapacity;
//
//     User *tmp = malloc(sizeof(User) * users->capacity);
//     if (tmp != NULL) {
//         users->data = tmp;
//     } else {
//         printf("Sorry, we failed to allocate memory for `users->data`!
//         Closing ConsoManage..."); exit(1);
//     }
// }

// Release 3 Main Req number 2: three "Array-Handling Functions"

struct Event *createEventData(const int size) {
    Event *tmp = malloc(sizeof(Event) * size);
    if (tmp == NULL) {
        printf("#createEventList Failed to allocate memory for events->data "
            ":(\nExiting ConsoManage...");
        exit(1);
    }

    return tmp;
}

void initEventData(EventList *events) {
    // gives default values to all allocated (but not yet used) events

    events->count = 0;
    events->capacity = eventListInitialCapacity;

    for (int i = 0; i < eventListInitialCapacity; i++) {
        // accessing data in the "(*(arr + i)).reserve" way
        struct Event *e =
                events->data + i; // pointer to the i-th Event (pointer arithmetic! :D)

        e->id = 0;
        e->name[0] = '\0';
        e->date[0] = '\0';
        e->time[0] = '\0';
        e->location[0] = '\0';
        e->maxSeatRow = 'A';
        e->maxSeatNumber = 0;
        e->price = 0.0;
    }
}

void displayEventsList(const Event *a, const int size) {
    // accessing data in the "arr[i].reserve" way
    for (int i = 0; i < size; ++i) {
        Event event = a[i];
        printEvent(&event);
    }
}

struct EventList *createEventContainer() {
    struct EventList *tmp = malloc(sizeof(EventList));
    if (tmp == NULL) {
        printf("#createEventList Failed to allocate memory for events ER-Light "
            "container :(\nExiting ConsoManage...");
        exit(1);
    }

    return tmp;
}

// Similar procedure for the users list:

void initUserList(UserList *users) {
    // gives default values to all allocated (but not yet used) users

    users->count = 0;
    users->capacity = userListInitialCapacity;

    for (int i = 0; i < userListInitialCapacity; i++) {
        struct User *u = users->data + i; // pointer to the i-th User

        u->fullName[0] = '\0';
        u->age = 0;
        u->email[0] = '\0';
        u->ticketsCount = 0;
        u->ticketsCapacity = 0;
        u->ticketsOwned = NULL;
    }
}

struct User *createUserData(const int size) {
    struct User *tmp = malloc(sizeof(User) * size);
    if (tmp == NULL) {
        printf("#createEventList Failed to allocate memory for users->data "
            ":(\nExiting ConsoManage...");
        exit(1);
    }

    return tmp;
}

struct UserList *createUserContainer() {
    struct UserList *tmp = malloc(sizeof(UserList));
    if (tmp == NULL) {
        printf("#createUserList Failed to allocate memory for users ER-Light "
            "container :(\nExiting ConsoManage...");
        exit(1);
    }

    return tmp;
}

void displayUsersList(const User *a, const int size) {
    // accessing data in the "arr[i].field" way
    for (int i = 0; i < size; ++i) {
        User user = a[i];
        printUser(&user);
    }
}

// -----Starting point of the app-----

int main() {
    char buffer[100];
    int userType;

    // events = malloc(sizeof(Event) * maxEvents);
    EventList *events = createEventContainer();
    events->data = createEventData(eventListInitialCapacity);
    initEventData(events);
    UserList *users = createUserContainer();
    users->data = createUserData(userListInitialCapacity);
    initUserList(users);

    while (1) {
        printf("--------------------------Welcome to ConsoManage--------------------------\n");
        printf("Are you a Participant [1] or Organizer [2]? (Enter [-1] to quit the program.)\n");
        printf("Choice: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &userType) != 1) {
            printf("Invalid input.\n");
            continue;
        }
        if (userType == 1) {
            participantFlow(events, users);
        } else if (userType == 2) {
            organizerFlow(events, users);
        } else if (userType == -1) {
            break;
        } else {
            printf("Please choose [1] or [2].\n");
        }
    }

    free(events->data);
    events->data = NULL;
    // container only freed once at the end of program; freeing during use looks
    // like memory leakage.
    free(events);
    events = NULL;

    free(users->data);
    users->data = NULL;
    // container only freed once at the end of program; freeing during use looks
    // like memory leakage.
    free(users);
    users = NULL;

    return 0;
}
