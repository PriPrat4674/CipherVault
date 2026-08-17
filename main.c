/*
 * main.c — Entry point and menu dispatcher for CipherVault
 * Handles authentication, command routing, session management
 * Member A owns this file
 */

#include "vault.h"
#include "crypto.h"
#include "display.h"
#include "fileio.h"
#include "vault_ops.h"
#include "search.h"
#include <time.h>

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

void export_report(struct Record *recs, int n) {
    FILE *fp = fopen(EXPORT_FILE, "w");
    if (!fp) {
        printf("Error: Cannot create export file.\n");
        return;
    }

    fprintf(fp, "=== CIPHERVAULT EXPORT ===\n\n");

    struct VaultMeta meta;
    int key = 0;
    if (load_meta(&meta)) {
        key = meta.encryption_key;
    }

    for (int i = 0; i < n; i++) {
        if (recs[i].is_deleted == 1) {
            continue;
        }
        struct Record copy = recs[i];
        if (key != 0) {
            decrypt_record(&copy, key);
        }
        fprintf(fp, "[%d] %s\n    Field1: %s\n    Field2: %s\n    Field3: %s\n    Added : %s\n\n",
                copy.id,
                copy.title,
                copy.field1,
                copy.field2,
                copy.field3,
                copy.created_date);
    }

    fclose(fp);
    printf("Decrypted report written to vault_export.txt\n");
}

void main_menu_loop(struct VaultMeta *meta) {
    char input[256];
    char cmd[64];

    while (1) {
        print_prompt();
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        input[strcspn(input, "\r\n")] = '\0';

        if (sscanf(input, "%s", cmd) != 1) {
            continue;
        }

        if (strcmp(cmd, "help") == 0) {
            printf("=== CIPHERVAULT COMMANDS ===\n");
            printf("  add          Add a new record (password / note / contact)\n");
            printf("  view         View all records\n");
            printf("  search       Search records by keyword or type\n");
            printf("  sort         Sort records (by id / title / type / date)\n");
            printf("  update <id>  Update a record by ID\n");
            printf("  delete <id>  Soft-delete a record by ID\n");
            printf("  purge        Permanently remove all deleted records\n");
            printf("  export       Export decrypted report to vault_export.txt\n");
            printf("  passwd       Change master password\n");
            printf("  stats        Show vault statistics dashboard\n");
            printf("  about        How the encryption works\n");
            printf("  clear        Clear the screen\n");
            printf("  exit         Lock vault and exit\n");
            printf("============================\n");
        } else if (strcmp(cmd, "add") == 0) {
            cmd_add_record(meta->encryption_key);
        } else if (strcmp(cmd, "view") == 0) {
            cmd_view_all(meta->encryption_key);
        } else if (strcmp(cmd, "search") == 0) {
            cmd_search(meta->encryption_key);
        } else if (strcmp(cmd, "sort") == 0) {
            cmd_sort(meta->encryption_key);
        } else if (strcmp(cmd, "update") == 0) {
            int id = 0;
            if (sscanf(input, "%*s %d", &id) == 1) {
                cmd_update_record(id, meta->encryption_key);
            } else {
                printf("Usage: update <id>\n");
            }
        } else if (strcmp(cmd, "delete") == 0) {
            int id = 0;
            if (sscanf(input, "%*s %d", &id) == 1) {
                cmd_delete_record(id);
            } else {
                printf("Usage: delete <id>\n");
            }
        } else if (strcmp(cmd, "purge") == 0) {
            cmd_purge(meta->encryption_key);
        } else if (strcmp(cmd, "export") == 0) {
            struct Record buf[MAX_RECORDS];
            int count = load_all_records(buf, MAX_RECORDS);
            export_report(buf, count);
        } else if (strcmp(cmd, "passwd") == 0) {
            cmd_change_password(meta);
        } else if (strcmp(cmd, "stats") == 0) {
            struct Record buf[MAX_RECORDS];
            int count = load_all_records(buf, MAX_RECORDS);
            print_stats(buf, count, meta);
        } else if (strcmp(cmd, "about") == 0) {
            cmd_about();
        } else if (strcmp(cmd, "clear") == 0) {
            printf("\033[2J\033[H");
            print_banner();
        } else if (strcmp(cmd, "exit") == 0) {
            printf("Locking vault. Goodbye.\n");
            return;
        } else {
            printf("Unknown command: %s. Type 'help' for options.\n", cmd);
        }
    }
}

int main(void) {
    struct VaultMeta meta;
    char pwd_input[128];
    int tries = 0;
    char computed[MAX_HASH_LEN];
    (void)computed;

    print_banner();

    if (load_meta(&meta) == 0) {
        printf("First run detected. Set your master password: ");
        if (fgets(pwd_input, sizeof(pwd_input), stdin)) {
            pwd_input[strcspn(pwd_input, "\r\n")] = '\0';
        }
        init_vault(pwd_input);
        load_meta(&meta);
    }

    do {
        printf("Enter master password: ");
        if (fgets(pwd_input, sizeof(pwd_input), stdin)) {
            pwd_input[strcspn(pwd_input, "\r\n")] = '\0';
        }

        if (verify_password(pwd_input, &meta)) {
            break;
        } else {
            tries++;
            printf("Wrong password. %d attempt(s) remaining.\n", MAX_LOGIN_TRIES - tries);
        }

        if (tries >= MAX_LOGIN_TRIES) {
            printf("Vault locked. Too many failed attempts.\n");
            exit(1);
        }
    } while (tries < MAX_LOGIN_TRIES);

    printf("Access granted. Welcome back.\n");
    printf("Last login: %s\n\n", meta.last_login);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t) {
        strftime(meta.last_login, sizeof(meta.last_login), "%Y-%m-%d %H:%M", t);
    } else {
        strncpy(meta.last_login, "2026-08-17", sizeof(meta.last_login) - 1);
        meta.last_login[sizeof(meta.last_login) - 1] = '\0';
    }
    save_meta(&meta);

    main_menu_loop(&meta);

    return 0;
}
