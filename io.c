#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "logic.h"
#include "models.h"

int nextEventID;
int nextTicketID = 0;

void setNextEventID(int newID) {
    nextEventID = newID;
}

/* printing helpers unchanged */
void printUser(User *user) {
    printf("\n------------- User Details -------------\n");
    printf("Full name: %s\n", user->fullName);
    printf("Age:       %d\n", user->age);
    printf("E-mail:    %s\n", user->email);
    printf("----------------------------------------\n\n");
}

void printEvent(Event *event) {
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
    printf("-----------------------------------------\n\n");
}

void printUserTickets(User *user, EventList *events) {
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


void printSeatmap(char *seatmap, char maxSeatRow, int maxSeatNumber) {
    printf("------------- Seatmap -------------\n");

    const int seats = maxSeatNumber;

    for (char row = 'A'; row <= maxSeatRow; row++) {
        printf("%c: ", row);

        const int rowIndex = row - 'A';

        for (int seat = 1; seat <= seats; seat++) {
            const int cellSize = 3;
            const int index = (rowIndex * seats + (seat - 1)) * cellSize;

            printf("%s ", &seatmap[index]);
        }

        printf("\n");
    }

    printf("----------------------------------\n\n");
}


void participantFlow(EventList *events, UserList *users) {
    char buffer[100];
    User user;

    // -----Enter Participant Info-----
    while (1) {
        printf("Enter full name: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (isValidFullName(buffer)) {
            strcpy(user.fullName, buffer);
            break;
        }

        printf("Invalid name, please try another spelling.\n");
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

    resizeUserListIfNeeded(users);

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

                // Generate and show this even'ts seatmap

                char *seatmap = generateSeatmap(users, &event);
                printSeatmap(seatmap, event.maxSeatRow, event.maxSeatNumber);
                free(seatmap);
                seatmap = NULL;

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
                resizeTicketsOwnedListIfNeeded(currentUser);

                currentUser->ticketsOwned[currentUser->ticketsCount] = ticket;
                currentUser->ticketsCount++;

                printf("Ticket booked successfully! Reference: %s, Price: %.2lf\n",
                       ticket.ticketReference, ticket.pricePaid);
                break;
            }
            case 3: {
                printUserTickets(&users->data[currentUserIndex], events);
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
                deleteAllEvents(events);
            } else if (organizerChoice == 'Y') {
                deleteAllUsers(users);
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

    resizeEventListIfNeeded(events);

    // now, confirmed we have space in the events list
    // therefore, add event / insert event
    events->data[events->count] = newEvent;
    events->count++;

    saveAllEvents(events->data, events->count);

    printf("Event saved successfully!\n");
    printEvent(&newEvent);
}
