/*
 * search.c — Search and Sort functions for CipherVault
 * Implements keyword search (3 modes) and bubble sort (4 criteria)
 * Member B owns this file
 */

#include "vault.h"
#include "search.h"
#include "crypto.h"
#include "fileio.h"
#include "display.h"

#include <stdio.h>
#include <string.h>
<<<<<<< HEAD
#include <ctype.h>
=======
>>>>>>> 147ca3881da07e9e62d139f2406bd89a6c5cac84

/* ---------------------------------------------------------------
 * contains — static helper
 * Manual substring search; returns 1 if needle found in haystack,
<<<<<<< HEAD
 * 0 otherwise. Does not use strstr(). Case-insensitive.
=======
 * 0 otherwise. Does not use strstr().
>>>>>>> 147ca3881da07e9e62d139f2406bd89a6c5cac84
 * --------------------------------------------------------------- */
static int contains(const char *haystack, const char *needle)
{
    if (needle == NULL || needle[0] == '\0') return 1;
    if (haystack == NULL || haystack[0] == '\0') return 0;

    int h_len = 0;
    int n_len = 0;

    while (haystack[h_len] != '\0') h_len++;
    while (needle[n_len]   != '\0') n_len++;

    for (int i = 0; i <= h_len - n_len; i++) {
        int match = 1;
        for (int j = 0; j < n_len; j++) {
<<<<<<< HEAD
            if (tolower((unsigned char)haystack[i + j]) != tolower((unsigned char)needle[j])) {
=======
            if (haystack[i + j] != needle[j]) {
>>>>>>> 147ca3881da07e9e62d139f2406bd89a6c5cac84
                match = 0;
                break;
            }
        }
        if (match) return 1;
    }
    return 0;
}

/* ---------------------------------------------------------------
 * swap_records — static helper
 * Swaps two Record structs in-place via a temporary copy.
 * --------------------------------------------------------------- */
static void swap_records(struct Record *a, struct Record *b)
{
    struct Record temp = *a;
    *a = *b;
    *b = temp;
}

/* ---------------------------------------------------------------
 * cmd_search
 * Offers three search modes — by title keyword, by record type,
 * or by keyword across all fields — and displays matching results.
 * --------------------------------------------------------------- */
void cmd_search(int enc_key)
{
    int mode;

    printf("Search by:\n");
    printf("1. Title keyword\n");
    printf("2. Record type (Password / Note / Contact)\n");
    printf("3. Keyword in any field\n");
    printf("Choice: ");
    scanf("%d", &mode);
    while (getchar() != '\n');

    struct Record buf[MAX_RECORDS];
    struct Record results[MAX_RECORDS];
    struct Record dec[MAX_RECORDS];

    int count        = load_all_records(buf, MAX_RECORDS);
    int result_count = 0;

    /* Build a fully decrypted copy of all active records */
    for (int i = 0; i < count; i++) {
        dec[i] = buf[i];
        if (dec[i].is_deleted == 0) {
            decrypt_record(&dec[i], enc_key);
        }
    }

    if (mode == 1) {
        /* ---- Mode 1: title keyword ---- */
        char keyword[MAX_FIELD_LEN];
        printf("Enter keyword: ");
        fgets(keyword, MAX_FIELD_LEN, stdin);
        keyword[strcspn(keyword, "\n")] = '\0';

        for (int i = 0; i < count; i++) {
            if (dec[i].is_deleted == 0 && contains(dec[i].title, keyword)) {
                results[result_count++] = dec[i];
            }
        }

    } else if (mode == 2) {
        /* ---- Mode 2: record type ---- */
        int type_choice;
        printf("Type (1=Password, 2=Note, 3=Contact): ");
        scanf("%d", &type_choice);
        while (getchar() != '\n');

        for (int i = 0; i < count; i++) {
            if (dec[i].is_deleted == 0 && dec[i].type == type_choice) {
                results[result_count++] = dec[i];
            }
        }

    } else if (mode == 3) {
        /* ---- Mode 3: keyword in any field ---- */
        char kw[MAX_FIELD_LEN];
        printf("Enter keyword: ");
        fgets(kw, MAX_FIELD_LEN, stdin);
        kw[strcspn(kw, "\n")] = '\0';

        for (int i = 0; i < count; i++) {
            if (dec[i].is_deleted == 0 &&
                (contains(dec[i].title,  kw) ||
                 contains(dec[i].field1, kw) ||
                 contains(dec[i].field2, kw) ||
                 contains(dec[i].field3, kw))) {
                results[result_count++] = dec[i];
            }
        }

    } else {
        printf("Invalid search mode.\n");
        return;
    }

    if (result_count == 0) {
        printf("No records found.\n");
    } else {
        printf("%d record(s) found:\n", result_count);
        print_table(results, result_count);
    }
}

/* ---------------------------------------------------------------
 * cmd_sort
 * Loads and decrypts all active records, then applies a bubble
 * sort on one of four criteria, and displays the sorted table.
 * --------------------------------------------------------------- */
void cmd_sort(int enc_key)
{
    int mode;

    printf("Sort by:\n");
    printf("1. ID (default order)\n");
    printf("2. Title (A-Z)\n");
    printf("3. Type\n");
    printf("4. Date Added\n");
    printf("Choice: ");
    scanf("%d", &mode);
    while (getchar() != '\n');

    struct Record buf[MAX_RECORDS];
    int count = load_all_records(buf, MAX_RECORDS);

    /* Decrypt all active records in buf */
    for (int i = 0; i < count; i++) {
        if (buf[i].is_deleted == 0) {
            decrypt_record(&buf[i], enc_key);
        }
    }

    /* Bubble sort — comparator changes per mode */
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            int do_swap = 0;

            if (mode == 1) {
                do_swap = (buf[j].id > buf[j + 1].id);
            } else if (mode == 2) {
                do_swap = (strcmp(buf[j].title, buf[j + 1].title) > 0);
            } else if (mode == 3) {
                do_swap = (buf[j].type > buf[j + 1].type);
            } else if (mode == 4) {
                do_swap = (strcmp(buf[j].created_date, buf[j + 1].created_date) > 0);
            }

            if (do_swap) {
                swap_records(&buf[j], &buf[j + 1]);
            }
        }
    }

    print_table(buf, count);

    if (mode == 1) {
        printf("Showing ID sorted records.\n");
    } else if (mode == 2) {
        printf("Showing Title sorted records.\n");
    } else if (mode == 3) {
        printf("Showing Type sorted records.\n");
    } else if (mode == 4) {
        printf("Showing Date Added sorted records.\n");
    } else {
        printf("Invalid sort mode.\n");
    }
}
