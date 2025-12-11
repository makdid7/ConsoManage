#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "io.h"
#include "models.h"
#include "logic.h"

// hello

const int eventListInitialCapacity = 1;
const int userListInitialCapacity = 1;

int nextTicketID;

// Array-Handling Functions


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
                resizeTicketsOwnedListIfNeeded(&user);

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


// -----Starting point of the app-----

int main() {
    char buffer[100];
    UserType userType;

    // Allocate memory for the event list
    EventList *events = createEventContainer();
    const int initialEventCount = getEventCount();
    int initialCapacity;

    if (initialEventCount < 1) {
        initialCapacity = 1;
        events->data = createEventData(initialCapacity);
    } else {
        // Calculate the nearest power of two before allocating
        initialCapacity = nearestPowerOfTwo(initialEventCount);
        events->data = createEventData(initialCapacity);
    }

    // initEventData(events, initialEventCount);

    // Set the capacity to what was actually allocated
    events->capacity = initialCapacity;

    // Load events and set the actual count using the return value
    const int actualLoadedCount = loadAllEvents(events->data);
    events->count = actualLoadedCount;

    // Set next ID to 1 + the latest preloaded event's ID
    const int maxEventID = getMaxEventID(events);
    setNextEventID(maxEventID + 1);

    ///\\\///\\\///\\\

    // Allocate memory for the user list
    UserList *users = createUserContainer();
    users->data = createUserData(userListInitialCapacity);
    initUserList(users, 0);

    // Enter main/base UI cycle
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
            // enum: 1 == Participant
            participantFlow(events, users);
        } else if (userType == 2) {
            // enum: 2 == Organizer
            organizerFlow(events, users);
        } else if (userType == -1) {
            break; // Gracefully exit the main/base UI cycle
        } else {
            // Invalid input, re-starting main/base UI cycle
            printf("Please choose [1] or [2].\n");
        }
    }

    // End of main/base UI cycle

    // printf("1");
    saveAllEvents(events->data, events->count);
    // printf("2");
    freeAllDynamicMemory(events, users);

    printf("---------------------Thank you for using ConsoManage!---------------------");

    return 0;
}
