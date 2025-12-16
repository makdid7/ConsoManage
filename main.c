#include <stdio.h>

#include "file.h"
#include "io.h"
#include "logic.h"
#include "models.h"

const int eventListInitialCapacity = 1;
const int userListInitialCapacity = 1;

int main() {
    char buffer[100];
    UserType userType;

    EventList *events = createEventContainer();
    const int initialEventCount = getEventCount();
    int initialCapacity =
            (initialEventCount < 1)
                ? 1
                : nearestPowerOfTwo(initialEventCount);

    events->data = createEventData(initialCapacity);
    events->capacity = initialCapacity;
    events->count = loadAllEvents(events->data);

    setNextEventID(getMaxEventID(events) + 1);

    UserList *users = createUserContainer();
    users->data = createUserData(userListInitialCapacity);
    initUserList(users, 0);

    while (1) {
        printf("--------------------------Welcome to ConsoManage--------------------------\n");
        printf("Are you a Participant [1] or Organizer [2]? (Enter [-1] to quit the program.)\n");
        printf("Choice: ");
        fgets(buffer, sizeof(buffer), stdin);

        if (sscanf(buffer, "%d", &userType) != 1) continue;

        if (userType == 1) {
            participantFlow(events, users);
        } else if (userType == 2) {
            organizerFlow(events, users);
        } else if (userType == -1) {
            break;
        }
    }

    saveAllEvents(events->data, events->count);
    freeAllDynamicMemory(events, users);

    printf("---------------------Thank you for using ConsoManage!---------------------");
    return 0;
}
