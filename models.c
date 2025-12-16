#include "models.h"

#include <ctype.h>
#include <string.h>

int isValidEventName(char name[51]) {
    const int len = strlen(name);
    if (len == 0 || len > 50) {
        return 0;
    }

    for (int i = 0; i < len; i++) {
        const char c = name[i];
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
        const char c = loc[i];
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
    const int day = (date[0] - '0') * 10 + (date[1] - '0');
    const int month = (date[3] - '0') * 10 + (date[4] - '0');
    const int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 +
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

int isValidRow(const Event *event, const char row) {
    if (row >= 'A' && row <= event->maxSeatRow) {
        return 1;
    }
    return 0;
}

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

int isValidFullName(char *fullName) {
    const int len = strlen(fullName);

    if (len < 1 || len > 75) {
        return 0;
    }
    for (int i = 0; i < len; i++) {
        const char c = fullName[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ' ||
              c == '-' || c == ',')) {
            return 0;
        }
    }


    return 1;
}

int isValidEmail(const char *email) {
    const size_t len = strlen(email);
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
