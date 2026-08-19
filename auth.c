#include "auth.h"
#include "crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#else
#include <unistd.h>
#include <termios.h>
#endif

int verify_password(const char *input, struct VaultMeta *meta) {
    if (!input || !meta) {
        return 0;
    }
    char computed_hash[MAX_HASH_LEN];
    hash_password(input, computed_hash);
    if (strcmp(computed_hash, meta->password_hash) == 0) {
        return 1;
    }
    return 0;
}

void get_masked_input(char *buf, int max_len) {
    if (!buf || max_len <= 0) {
        return;
    }
    buf[0] = '\0';

#ifdef _WIN32
    if (!_isatty(_fileno(stdin))) {
        if (fgets(buf, max_len, stdin)) {
            buf[strcspn(buf, "\r\n")] = '\0';
        }
        return;
    }

    int idx = 0;
    while (1) {
        int ch = _getch();
        if (ch == '\r' || ch == '\n') {
            printf("\n");
            break;
        } else if (ch == '\b' || ch == 127) {
            if (idx > 0) {
                idx--;
                printf("\b \b");
                fflush(stdout);
            }
        } else if (ch == 0 || ch == 224) {
            _getch(); // consume extended key code
        } else if (ch == 3) {
            // Ctrl+C
            printf("\n");
            exit(0);
        } else if (isprint(ch) && idx < max_len - 1) {
            buf[idx++] = (char)ch;
            printf("*");
            fflush(stdout);
        }
    }
    buf[idx] = '\0';
#else
    if (!isatty(fileno(stdin))) {
        if (fgets(buf, max_len, stdin)) {
            buf[strcspn(buf, "\r\n")] = '\0';
        }
        return;
    }

    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int idx = 0;
    while (1) {
        int ch = getchar();
        if (ch == '\n' || ch == '\r' || ch == EOF) {
            printf("\n");
            break;
        } else if (ch == '\b' || ch == 127) {
            if (idx > 0) {
                idx--;
                printf("\b \b");
                fflush(stdout);
            }
        } else if (isprint(ch) && idx < max_len - 1) {
            buf[idx++] = (char)ch;
            printf("*");
            fflush(stdout);
        }
    }
    buf[idx] = '\0';
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}
