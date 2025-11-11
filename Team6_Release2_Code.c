#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

typedef struct {
    char ownerFullName[75];
    char row;
    int seatNumber;
    int id;
    char ticketReference[30];
    double pricePaid;
    int eventID;
} Ticket;

typedef struct {
    char fullName[75];
    int age;
    char email[51];
    Ticket ticketsOwned[1000];
    int ticketsCount;
} User;

typedef struct {
    int id;
    char name[52];
    char date[12];
    char time[7];
    char location[52];
    char maxSeatRow;
    int maxSeatNumber;
    double price;
} Event;

Event *events;
int eventCount = 0;
int maxEvents = 1;


User *users;
int userCount = 0;
int maxUsers = 1;

int nextTicketID = 1;
int nextEventID = 1;

// -----Input Validation -----

int isValidEventName(char name[51]) {
    int len = strlen(name);
    if (len == 0 || len > 50) {
        return 0;
    }

    int i;
    for (i = 0; i < len; i++) {
        char c = name[i];
        if (!((c >= 'A' && c <= 'Z') ||
              (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') ||
              c == ' ' || c == '-' || c == ',' || isdigit(c))) {
            return 0;
        }
    }

    return 1;
}

int isValidEventLocation(char loc[51]) {
    int len = strlen(loc);
    if (len == 0 || len > 50) {
        return 0;
    }

    int i;
    for (i = 0; i < len; i++) {
        char c = loc[i];
        if (!((c >= 'A' && c <= 'Z') ||
              (c >= 'a' && c <= 'z') ||
              c == ' ' || c == '-' || c == ',' || isdigit(c))) {
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

    int i;
    for (i = 0; i < 10; i++) {
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
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + (date[8] - '0') * 10 + (date[9] - '0');

    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 2025 || year > 2075) {
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

    if (!isdigit(time[0]) || !isdigit(time[1]) || !isdigit(time[3]) || !isdigit(time[4])) {
        return 0;
    }

    int hour = (time[0] - '0') * 10 + (time[1] - '0');
    int minute = (time[3] - '0') * 10 + (time[4] - '0');

    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        return 0;
    }

    return 1;
}

int isValidEventMaxSeatNumber(int num) {
    if (num >= 1 && num <= 20) {
        return 1;
    }
    return 0;
}

int isValidEventPrice(double price) {
    if (price >= 0 && price <= 100.0) {
        return 1;
    }
    return 0;
}

// -----Business Logic-----

double getCostWithDiscount(double originalCost, User *user) {
    if (user->age <= 12) { return originalCost * (1.0 - 0.4); }
    if (user->age <= 18) { return originalCost * (1.0 - 0.2); }
    if (user->age >= 65) { return originalCost * (1.0 - 0.45); }

    return originalCost;
}


void printEvent(Event *event) {
    if (event->maxSeatNumber == 0) {
        return;
        // under normal circumstances, this cannot happen; we return because malloc gives memory to a non-existing event at the beginning of the program.
    }
    printf("\n------------- Event Details -------------\n");
    printf("ID:       %04d\n", event->id);
    printf("Name:     %s\n", event->name);
    printf("Date:     %s\n", event->date);
    printf("Time:     %s\n", event->time);
    printf("Location: %s\n", event->location);
    printf("Seats:    %c01-%c%02d\n", 'A', event->maxSeatRow, event->maxSeatNumber);
    if (event->price == 0) {
        printf("Price:    Free\n");
    } else {
        printf("Price:    %.2lf\n", event->price);
    }
    printf("----------------------------------------\n\n");
}

int isValidRow(Event *event, char row) {
    if (row >= 'A' && row <= event->maxSeatRow) {
        return 1;
    }
    return 0;
}

// -----Event Creation -----

void createNewEvent() {
    Event event;
    char buffer[100];

    while (1) {
        printf("Enter event name: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (isValidEventName(buffer)) {
            strcpy(event.name, buffer);
            break;
        }

        printf("Please only use Latin characters, numbers, dashes, commas, and spaces.\n");
    }

    while (1) {
        printf("Enter event date (DD.MM.YYYY): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (isValidEventDate(buffer)) {
            strcpy(event.date, buffer);
            break;
        }

        printf("Please use DD.MM.YYYY format, for example 16.10.2025.\n");
    }

    while (1) {
        printf("Enter event time (HH:MM): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (isValidEventTime(buffer)) {
            strcpy(event.time, buffer);
            break;
        }

        printf("Incorrect format, please use HH:MM, eg 16:30.\n");
    }

    while (1) {
        printf("Enter location: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (isValidEventLocation(buffer)) {
            strcpy(event.location, buffer);
            break;
        }

        printf("Please only use Latin letters, numbers, spaces, commas, and dashes. Max 50 characters.\n");
    }

    while (1) {
        printf("Enter max seat row (A-Z): ");
        fgets(buffer, sizeof(buffer), stdin);
        if (
            sscanf(buffer, "%c", &event.maxSeatRow) == 1
            && strlen(buffer) != 1
            && isValidRow(&event, buffer[0])) {
            break;
        }

        printf("Please use an uppercase letter A-Z, for example V.\n");
    }

    while (1) {
        printf("Enter max seat number (1-20): ");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &event.maxSeatNumber) == 1 && isValidEventMaxSeatNumber(event.maxSeatNumber)) {
            break;
        }

        printf("Please use a number from 1-20.\n");
    }

    while (1) {
        printf("Enter price (0.00-100): ");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%lf", &event.price) == 1 && isValidEventPrice(event.price)) {
            break;
        }

        printf("Price must be between 0.00 and 100.00.\n");
    }

    event.id = nextEventID;
    nextEventID++;

    if (eventCount >= maxEvents) {
        // we're reaching the event limit; let's resize to allow for 1 more.
        maxEvents++;
        Event *temp = realloc(events, sizeof(Event) * maxEvents);
        if (!temp) {
            // realloc failed
            printf("Sorry, we failed to reallocate memory to accommodate the new event!");
            free(events);
            exit(1);
        }
        // realloc didn't fail; assign new memory to events
        events = temp;
    }

    // in release 2 we had: events[eventCount] = event;
    // for release 3 we're using pointer arithmetic:
    Event *newEventPointer = events + eventCount; //works cuz `events` refers to address of first element of `events`
    *newEventPointer = event; //assign our new event to the memory at address [first event]+[total # of events]
    eventCount++;

    printf("Event created successfully!\n");
    printEvent(&event);
}


// -----Participant Flow -----

int isValidEventID(int id) {
    if (id == -1) {
        return 1;
    }

    if (id < -1) {
        return 0;
    }

    for (int i = 0; i < eventCount; i++) {
        if (events[i].id == id) {
            return 1;
        }
    }
    return 0;
}

int isSeatTaken(Event *event, char row, int seatNumber) {
    for (int i = 0; i < userCount; i++) {
        for (int j = 0; j < users[i].ticketsCount; j++) {
            Ticket ticket = users[i].ticketsOwned[j];
            if (ticket.eventID == event->id && ticket.row == row && ticket.seatNumber == seatNumber) {
                return 1;
            }
        }
    }
    return 0;
}

void listUserTickets(User *user) {
    printf("\n---------- Tickets Owned by %s ----------\n", user->fullName);
    if (user->ticketsCount == 0) {
        printf("No tickets purchased yet.\n");
    } else {
        for (int i = 0; i < user->ticketsCount; i++) {
            Ticket ticket = user->ticketsOwned[i];
            Event event;
            int found = 0;
            int k;
            for (k = 0; k < eventCount; k++) {
                if (events[k].id == ticket.eventID) {
                    event = events[k];
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
    printf("-----------------------------------------\n");
}


int isValidEmail(char *email[100]) {
    return 1;
    if (strlen(*email) < 0 || strlen(*email) > 50) {
        return 0;
    }

    for (int i = 0; i < strlen(*email); ++i) {
        if (*email[i] == '@') {
            return 1;
        }
    }

    return 0;
}


// -----Utility-----


void printSeatMap(Event *event) {
    printf("------------- Seat Map -------------\n");

    for (int row = 'A'; row <= event->maxSeatRow; row++) {
        printf("%c: ", row);
        for (int seat = 1; seat <= event->maxSeatNumber; seat++) {
            if (isSeatTaken(event, row, seat)) {
                printf("-- ");
            } else {
                printf("%02d ", seat);
            }
        }
        printf("\n");
    }

    printf("-----------------------------------\n\n");
}

void participantFlow() {
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
            if (!((c >= 'A' && c <= 'Z') ||
                  (c >= 'a' && c <= 'z') ||
                  c == ' ' || c == '-' || c == ',')) {
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

        if (isValidEmail(&buffer)) {
            strcpy(user.email, buffer);
            break;
        }

        printf("Invalid email address.\n");
    }

    if (userCount >= maxUsers) {
        // we're reaching the event limit; let's resize to allow for 1 more.
        maxUsers++;
        User *temp = realloc(users, sizeof(User) * maxUsers);
        if (!temp) {
            // realloc failed
            printf("Sorry, we failed to reallocate memory to accommodate the new user!");
            free(users);
            exit(1);
        }
        // realloc didn't fail; assign new memory to us
        users = temp;
    }

    user.ticketsCount = 0;
    // release 2 / for static array: users[userCount] = user;ers
    int currentUserIndex = userCount;


    User *newUserPointer = users + userCount; //works cuz `users` refers to address of first element of `users`
    *newUserPointer = user; //assign our new user to the memory at address [first user]+[total # of users]
    userCount++;

    printf("User created successfully!\n");
    printEvent(&user);

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

        if (choice == 1) {
            if (eventCount == 0) {
                printf("Nothing is happening at the moment.\n");
            }
            int i;
            for (i = 0; i < eventCount; i++) {
                printEvent(&events[i]);
            }
        } else if (choice == 2) {
            int eventID;
            while (1) {
                printf("Enter Event ID: ");
                fgets(buffer, sizeof(buffer), stdin);
                if (sscanf(buffer, "%d", &eventID) == 1 && isValidEventID(eventID)) {
                    break;
                }
                printf("Invalid Event ID. Try again.\n");
            }

            Event event;
            int found = 0;
            int i;
            for (i = 0; i < eventCount; i++) {
                if (events[i].id == eventID) {
                    event = events[i];
                    found = 1;
                    break;
                }
            }

            if (!found) {
                printf("Event not found.\n");
                continue;
            }

            printSeatMap(&event);

            char row;
            int seatNumber;
            while (1) {
                printf("Enter row (A-Z): ");
                fgets(buffer, sizeof(buffer), stdin);
                if (sscanf(buffer, "%c", &row) == 1 && isValidRow(&event, row)) {
                    break;
                }
                printf("Please use uppercase letter A-Z. Make sure row %c exists.\n", buffer[0]);
            }

            while (1) {
                printf("Enter seat number (1-20): ");
                fgets(buffer, sizeof(buffer), stdin);
                if (sscanf(buffer, "%d", &seatNumber) == 1 && seatNumber >= 1 && seatNumber <= event.maxSeatNumber) {
                    if (isSeatTaken(&event, row, seatNumber)) {
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

            users[currentUserIndex].ticketsOwned[users[currentUserIndex].ticketsCount] = ticket;
            users[currentUserIndex].ticketsCount++;

            printf("Ticket booked successfully! Reference: %s, Price: %.2lf\n", ticket.ticketReference,
                   ticket.pricePaid);
        } else if (choice == 3) {
            listUserTickets(&users[currentUserIndex]);
        } else if (choice == 4) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
}


// -----Starting point of the app-----

int main() {
    char buffer[100];
    int userType;

    events = malloc(sizeof(Event) * maxEvents);
    if (!events) {
        printf("Sorry, we didn't manage to allocate memory properly for `events`!");
        exit(1);
    }

    //same logic for users
    users = malloc(sizeof(User) * maxUsers);
    if (!users) {
        printf("Sorry, we didn't manage to allocate memory properly for `users`!");
        exit(1);
    }

    while (1) {
        printf("Are you a Participant [1] or Organizer [2]? (Enter -1 to quit program.)");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &userType) != 1) {
            printf("Invalid input.\n");
            continue;
        }
        if (userType == 1) {
            participantFlow();
        } else if (userType == 2) {
            while (1) {
                printf("[1] Create a new event.\n");
                printf("[2] View all existing events.\n");
                printf("[3] Return to the main menu.\n");
                printf("[X] Delete all events. This is irreversible.\n");
                printf("[Y] Delete all users. This is irreversible.\n");
                char organizerChoice;
                fgets(buffer, sizeof(buffer), stdin);
                if (sscanf(buffer, "%c", &organizerChoice) != 1) {
                    printf("Invalid input.\n\n");
                } else {
                    if (organizerChoice == 'X') {
                        free(events);
                        eventCount = 0;
                        maxEvents = 1;
                        events = malloc(sizeof(Event) * maxEvents);
                        if (!events) {
                            printf("Sorry, we failed to allocate memory for `events`!");
                            exit(1);
                        }
                        break;
                    }
                    if (organizerChoice == 'Y') {
                        free(users);
                        userCount = 0;
                        maxUsers = 1;
                        users = malloc(sizeof(User) * maxUsers);
                        if (!users) {
                            printf("Sorry, we failed to allocate memory for `users`!");
                            exit(1);
                        }
                        break;
                    }
                    if (organizerChoice == '1') {
                        createNewEvent();
                    } else if (organizerChoice == '2') {

                        for (int i = 0; i < eventCount; i++) {
                            printEvent(&events[i]);
                        }
                    } else if (organizerChoice == '3') {
                        break;
                    }
                }
            }
        } else if (userType == -1) {
            break;
        } else {
            printf("Please choose 1 or 2.\n");
        }
    }

    free(events);
    free(users);

    return 0;
}
