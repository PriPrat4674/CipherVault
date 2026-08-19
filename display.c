/*
 * display.c — Display and UI functions for CipherVault
 * Handles banner, table printing, stats, about panel
 * Member A owns this file
 */

#include "display.h"

void print_banner(void) {
    printf("+==================================================+\n");
    printf("|                   CIPHERVAULT                    |\n");
    printf("|        Secure Personal Data Manager in C         |\n");
    printf("|     Type 'help' to see the menu after login      |\n");
    printf("+==================================================+\n");
    printf("\n");
}

void print_prompt(void) {
    printf("vault> ");
}

void print_table(struct Record *records, int count) {
    if (!records || count == 0) {
        printf("  [No records to display]\n");
        return;
    }

    printf("ID   | Type       | Title                           | Preview\n");
    printf("---- | ---------- | --------------------------------| --------------------\n");

    for (int i = 0; i < count; i++) {
        if (records[i].is_deleted == 0) {
            const char *type_str = "Unknown";
            if (records[i].type == TYPE_PASSWORD) {
                type_str = "Password";
            } else if (records[i].type == TYPE_NOTE) {
                type_str = "Note";
            } else if (records[i].type == TYPE_CONTACT) {
                type_str = "Contact";
            }

            char preview[21];
            strncpy(preview, records[i].field1, 20);
            preview[20] = '\0';
            for (int p = 0; preview[p] != '\0'; p++) {
                if (!isprint((unsigned char)preview[p])) {
                    preview[p] = '?';
                }
            }

            printf("%-4d | %-10s | %-32s| %.20s\n",
                   records[i].id,
                   type_str,
                   records[i].title,
                   preview);
        }
    }

    printf("----+------------+--------------------------------+--------------------\n");
}

void print_stats(struct Record *records, int count, struct VaultMeta *meta) {
    int active_count = 0;
    int deleted_count = 0;
    int passwords = 0;
    int notes = 0;
    int contacts = 0;

    if (records) {
        for (int i = 0; i < count; i++) {
            if (records[i].is_deleted == 0) {
                active_count++;
                if (records[i].type == TYPE_PASSWORD) {
                    passwords++;
                } else if (records[i].type == TYPE_NOTE) {
                    notes++;
                } else if (records[i].type == TYPE_CONTACT) {
                    contacts++;
                }
            } else {
                deleted_count++;
            }
        }
    }

    int total = meta ? meta->total_records : count;
    int enc_key = meta ? meta->encryption_key : 0;
    const char *last_login = (meta && meta->last_login[0] != '\0') ? meta->last_login : "N/A";

    printf("======== VAULT STATISTICS ========\n");
    printf("Total Records    : %d\n", total);
    printf("Active Records   : %d\n", active_count);
    printf("Deleted (hidden) : %d\n", deleted_count);
    printf("--- By Type ---\n");
    printf("Passwords        : %d\n", passwords);
    printf("Notes            : %d\n", notes);
    printf("Contacts         : %d\n", contacts);
    printf("Encryption Key   : %d\n", enc_key);
    printf("Last Login       : %s\n", last_login);
    printf("==================================\n");
}

void cmd_about(void) {
    printf("=== HOW CIPHERVAULT ENCRYPTS YOUR DATA ===\n");
    printf("\n");
    printf("Step 1 — Caesar Cipher (alphabetic characters only)\n");
    printf("  Original : hello123\n");
    printf("  Shift+13 : uryyb123\n");
    printf("  Formula  : (char - 'a' + key) %% 26 + 'a'\n");
    printf("\n");
    printf("Step 2 — XOR Cipher (digits, symbols, spaces)\n");
    printf("  Original : 1  (ASCII 49, binary 00110001)\n");
    printf("  XOR 0x5A : k  (ASCII 107, binary 01101011)\n");
    printf("  Formula  : char ^ 0x5A\n");
    printf("\n");
    printf("Step 3 — Storage\n");
    printf("  Result is written to vault.dat in binary using fwrite()\n");
    printf("  Nothing is ever stored in plaintext on disk.\n");
    printf("==========================================\n");
}
