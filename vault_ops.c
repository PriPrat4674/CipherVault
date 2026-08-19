/*
 * vault_ops.c — Core record management commands for CipherVault
 * Implements add, view, update, delete, purge, change-password
 * Member B owns this file
 */

#include "vault.h"
#include "vault_ops.h"
#include "auth.h"
#include "crypto.h"
#include "fileio.h"
#include "display.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* ---------------------------------------------------------------
 * cmd_add_record
 * Prompts the user to choose a record type, fills in all fields,
 * encrypts a copy, and appends it to the vault file.
 * --------------------------------------------------------------- */
void cmd_add_record(int enc_key)
{
    struct Record r = {0};
    int type_choice = 0;

    printf("Select record type:\n");
    printf("1. Password\n");
    printf("2. Secure Note\n");
    printf("3. Contact\n");
    printf("Choice: ");
    if (scanf("%d", &type_choice) != 1) {
        type_choice = 0;
    }
    while (getchar() != '\n');

    if (type_choice != 1 && type_choice != 2 && type_choice != 3) {
        printf("Invalid type.\n");
        return;
    }

    r.type = type_choice;

    /* Determine the next available ID from current record count */
    struct Record temp_buf[MAX_RECORDS];
    int count = load_all_records(temp_buf, MAX_RECORDS);
    r.id = count + 1;

    /* Common field: title */
    printf("Title: ");
    fgets(r.title, MAX_TITLE_LEN, stdin);
    r.title[strcspn(r.title, "\r\n")] = '\0';

    /* Format current creation date */
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    if (tm_info) {
        strftime(r.created_date, MAX_DATE_LEN, "%d-%b-%Y", tm_info);
    } else {
        strncpy(r.created_date, "Aug-2026", MAX_DATE_LEN - 1);
        r.created_date[MAX_DATE_LEN - 1] = '\0';
    }

    r.is_deleted = 0;

    /* Type-specific field prompts */
    if (r.type == TYPE_PASSWORD) {
        printf("Username: ");
        fgets(r.field1, MAX_FIELD_LEN, stdin);
        r.field1[strcspn(r.field1, "\r\n")] = '\0';

        printf("Password: ");
        get_masked_input(r.field2, MAX_FIELD_LEN);

        printf("URL (optional): ");
        fgets(r.field3, 64, stdin);
        r.field3[strcspn(r.field3, "\r\n")] = '\0';

    } else if (r.type == TYPE_NOTE) {
        printf("Note body: ");
        fgets(r.field1, MAX_FIELD_LEN, stdin);
        r.field1[strcspn(r.field1, "\r\n")] = '\0';

        printf("Tags: ");
        fgets(r.field2, MAX_FIELD_LEN, stdin);
        r.field2[strcspn(r.field2, "\r\n")] = '\0';

        printf("Category: ");
        fgets(r.field3, 64, stdin);
        r.field3[strcspn(r.field3, "\r\n")] = '\0';

    } else { /* TYPE_CONTACT */
        printf("Full Name: ");
        fgets(r.field1, MAX_FIELD_LEN, stdin);
        r.field1[strcspn(r.field1, "\r\n")] = '\0';

        printf("Phone: ");
        fgets(r.field2, MAX_FIELD_LEN, stdin);
        r.field2[strcspn(r.field2, "\r\n")] = '\0';

        printf("Email: ");
        fgets(r.field3, 64, stdin);
        r.field3[strcspn(r.field3, "\r\n")] = '\0';
    }

    /* Encrypt a copy and persist it; keep r unmodified for display */
    struct Record r_enc = r;
    encrypt_record(&r_enc, enc_key);
    save_record(&r_enc);

    printf("Record saved. ID: %d\n", r.id);
}

/* ---------------------------------------------------------------
 * cmd_view_all
 * Loads every record, decrypts active ones in the local buffer,
 * and delegates formatted output to print_table().
 * --------------------------------------------------------------- */
void cmd_view_all(int enc_key)
{
    struct Record buf[MAX_RECORDS];
    int count = load_all_records(buf, MAX_RECORDS);

    if (count == 0) {
        printf("Vault is empty.\n");
        return;
    }

    /* Decrypt active records in the local buffer only (file untouched) */
    for (int i = 0; i < count; i++) {
        if (buf[i].is_deleted == 0) {
            struct Record temp = buf[i];
            decrypt_record(&temp, enc_key);
            buf[i] = temp;
        }
    }

    print_table(buf, count);
}

/* ---------------------------------------------------------------
 * cmd_update_record
 * Finds a record by ID, shows current values, prompts for new
 * values (ENTER keeps existing), re-encrypts, and overwrites.
 * --------------------------------------------------------------- */
void cmd_update_record(int id, int enc_key)
{
    if (id <= 0) {
        printf("Invalid record ID.\n");
        return;
    }

    struct Record buf[MAX_RECORDS];
    int count = load_all_records(buf, MAX_RECORDS);

    /* Locate the target record */
    int idx = -1;
    for (int i = 0; i < count; i++) {
        if (buf[i].id == id && buf[i].is_deleted == 0) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("Record ID not found.\n");
        return;
    }

    /* Decrypt in-place for editing */
    decrypt_record(&buf[idx], enc_key);

    printf("Current values for ID %d:\n", id);
    printf("Title  : %s\n", buf[idx].title);
    printf("Field1 : %s\n", buf[idx].field1);
    printf("Field2 : %s\n", buf[idx].field2);
    printf("Field3 : %s\n", buf[idx].field3);

    printf("Enter new values (press ENTER to keep current):\n");

    char temp_input[MAX_FIELD_LEN];

    /* Title */
    printf("Title: ");
    fgets(temp_input, MAX_FIELD_LEN, stdin);
    temp_input[strcspn(temp_input, "\r\n")] = '\0';
    if (strlen(temp_input) > 0) {
        strncpy(buf[idx].title, temp_input, MAX_TITLE_LEN - 1);
        buf[idx].title[MAX_TITLE_LEN - 1] = '\0';
    }

    /* Field1 */
    printf("Field1: ");
    fgets(temp_input, MAX_FIELD_LEN, stdin);
    temp_input[strcspn(temp_input, "\r\n")] = '\0';
    if (strlen(temp_input) > 0) {
        strncpy(buf[idx].field1, temp_input, MAX_FIELD_LEN - 1);
        buf[idx].field1[MAX_FIELD_LEN - 1] = '\0';
    }

    /* Field2 */
    printf("Field2: ");
    if (buf[idx].type == TYPE_PASSWORD) {
        get_masked_input(temp_input, MAX_FIELD_LEN);
    } else {
        fgets(temp_input, MAX_FIELD_LEN, stdin);
        temp_input[strcspn(temp_input, "\r\n")] = '\0';
    }
    if (strlen(temp_input) > 0) {
        strncpy(buf[idx].field2, temp_input, MAX_FIELD_LEN - 1);
        buf[idx].field2[MAX_FIELD_LEN - 1] = '\0';
    }

    /* Field3 */
    printf("Field3: ");
    fgets(temp_input, MAX_FIELD_LEN, stdin);
    temp_input[strcspn(temp_input, "\r\n")] = '\0';
    if (strlen(temp_input) > 0) {
        strncpy(buf[idx].field3, temp_input, 63);
        buf[idx].field3[63] = '\0';
    }

    /* Re-encrypt modified record and persist */
    encrypt_record(&buf[idx], enc_key);
    update_record_in_file(&buf[idx]);

    printf("Record updated.\n");
}

/* ---------------------------------------------------------------
 * cmd_delete_record
 * Soft-deletes a record by setting its is_deleted flag to 1 and
 * rewriting just that slot via update_record_in_file().
 * --------------------------------------------------------------- */
void cmd_delete_record(int id)
{
    if (id <= 0) {
        printf("Invalid record ID.\n");
        return;
    }

    struct Record buf[MAX_RECORDS];
    int count = load_all_records(buf, MAX_RECORDS);

    int idx = -1;
    for (int i = 0; i < count; i++) {
        if (buf[i].id == id && buf[i].is_deleted == 0) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("Record ID not found or already deleted.\n");
        return;
    }

    buf[idx].is_deleted = 1;
    update_record_in_file(&buf[idx]);

    printf("Record %d marked as deleted.\n", id);
}

/* ---------------------------------------------------------------
 * cmd_purge
 * Hard-deletes all soft-deleted records: compacts the vault file
 * with re-assigned sequential IDs and updates the metadata.
 * --------------------------------------------------------------- */
void cmd_purge(int enc_key)
{
    printf("WARNING: This will permanently remove all deleted records. Continue? (y/n): ");
    char confirm = 'n';
    if (scanf(" %c", &confirm) != 1) {
        confirm = 'n';
    }
    while (getchar() != '\n');

    if (confirm != 'y' && confirm != 'Y') {
        printf("Purge cancelled.\n");
        return;
    }

    struct Record buf[MAX_RECORDS];
    struct Record clean[MAX_RECORDS];

    int count       = load_all_records(buf, MAX_RECORDS);
    int clean_count = 0;
    int removed     = count;   /* will subtract active records below */

    /* Build compacted array with sequential IDs */
    for (int i = 0; i < count; i++) {
        if (buf[i].is_deleted == 0) {
            clean[clean_count]    = buf[i];
            clean[clean_count].id = clean_count + 1;
            clean_count++;
        }
    }
    removed -= clean_count;  /* number of purged (deleted) records */

    /* Rewrite vault file from scratch */
    FILE *fp = fopen(VAULT_FILE, "wb");
    if (fp) {
        for (int i = 0; i < clean_count; i++) {
            fwrite(&clean[i], sizeof(struct Record), 1, fp);
        }
        fclose(fp);
    }

    /* Update metadata */
    struct VaultMeta meta;
    load_meta(&meta);
    meta.total_records = clean_count;
    save_meta(&meta);

    printf("Purge complete. %d records permanently deleted.\n", removed);

    (void)enc_key; /* reserved for potential future use */
}

/* ---------------------------------------------------------------
 * cmd_change_password
 * Verifies the current password, accepts a new one, re-encrypts
 * every active record with the new key, and updates the metadata.
 * --------------------------------------------------------------- */
void cmd_change_password(struct VaultMeta *meta)
{
    char current_input[MAX_FIELD_LEN];

    /* Verify current password */
    printf("Enter current password: ");
    get_masked_input(current_input, MAX_FIELD_LEN);

    if (!verify_password(current_input, meta)) {
        printf("Incorrect current password.\n");
        return;
    }

    /* Capture new password */
    char new_pwd[MAX_FIELD_LEN];
    char confirm_pwd[MAX_FIELD_LEN];

    printf("Enter new password: ");
    get_masked_input(new_pwd, MAX_FIELD_LEN);

    printf("Confirm new password: ");
    get_masked_input(confirm_pwd, MAX_FIELD_LEN);

    if (strcmp(new_pwd, confirm_pwd) != 0) {
        printf("Passwords do not match.\n");
        return;
    }

    /* Save old key BEFORE updating meta so we can re-encrypt records */
    int old_key = meta->encryption_key;

    /* Update metadata with new credentials */
    hash_password(new_pwd, meta->password_hash);
    meta->encryption_key = generate_key(new_pwd);
    int new_key = meta->encryption_key;

    /* Load, decrypt with old key, re-encrypt with new key */
    struct Record buf[MAX_RECORDS];
    int count = load_all_records(buf, MAX_RECORDS);

    for (int i = 0; i < count; i++) {
        if (buf[i].is_deleted == 0) {
            decrypt_record(&buf[i], old_key);
            encrypt_record(&buf[i], new_key);
        }
    }

    /* Rewrite vault file with re-encrypted records */
    FILE *fp = fopen(VAULT_FILE, "wb");
    if (fp) {
        for (int i = 0; i < count; i++) {
            fwrite(&buf[i], sizeof(struct Record), 1, fp);
        }
        fclose(fp);
    }

    save_meta(meta);

    printf("Password changed. All records re-encrypted with new key.\n");
}
