#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <bluetooth/bluetooth.h>

int bachk(const char *str) {
    if (!str) {
        return -1;
    }

    if (strlen(str) != 17) {
        return -1;
    }

    while (*str) {
        if (!isxdigit(*str++)) {
            return -1;
        }

        if (!isxdigit(*str++)) {
            return -1;
        }

        if (*str == 0) {
            break;
        }

        if (*str++ != ':') {
            return -1;
        }
    }

    return 0;
}

int str2ba(const char *str, bdaddr_t *ba) {
    int i;

    if (bachk(str) < 0) {
        memset(ba, 0, sizeof(*ba));
        return -1;
    }

    for (i = 5; i >= 0; i--, str += 3) {
        ba->b[i] = strtol(str, NULL, 16);
    }

    return 0;
}
