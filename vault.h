#ifndef VAULT_H
#define VAULT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define VAULT_FILE      "vault.dat"
#define META_FILE       "vault.meta"
#define EXPORT_FILE     "vault_export.txt"

#define MAX_RECORDS     200
#define MAX_FIELD_LEN   128
#define MAX_TITLE_LEN   64
#define MAX_DATE_LEN    20
#define MAX_HASH_LEN    64


#define MAX_LOGIN_TRIES 3
#define XOR_KEY         0x5A
#define TYPE_PASSWORD   1
#define TYPE_NOTE       2
#define TYPE_CONTACT    3

struct Record {
    int id;
    int type;
    char title[MAX_TITLE_LEN];
    char field1[MAX_FIELD_LEN];
    char field2[MAX_FIELD_LEN];
    char field3[64];
    char created_date[MAX_DATE_LEN];
    int is_deleted;
};

struct VaultMeta {
    char password_hash[MAX_HASH_LEN];
    int total_records;
    char last_login[MAX_DATE_LEN];
    int encryption_key;
};

/* --- Functions implemented in crypto.c --- */
void encrypt_string(char *str, int key);
void decrypt_string(char *str, int key);
void encrypt_record(struct Record *r, int key);
void decrypt_record(struct Record *r, int key);
void hash_password(const char *pwd, char *out_hash);
int  generate_key(const char *password);

/* --- Functions implemented in fileio.c --- */
int  save_record(struct Record *r);
int  load_all_records(struct Record *buf, int max);
int  update_record_in_file(struct Record *r);
void init_vault(const char *master_password);
int  load_meta(struct VaultMeta *meta);
int  save_meta(struct VaultMeta *meta);

/* --- Functions implemented in vault.c --- */
void cmd_add_record(int enc_key);
void cmd_view_all(int enc_key);
void cmd_update_record(int id, int enc_key);
void cmd_delete_record(int id);
void cmd_purge(int enc_key);
void cmd_change_password(struct VaultMeta *meta);

/* --- Functions implemented in search.c --- */
void cmd_search(int enc_key);
void cmd_sort(int enc_key);

/* --- Functions implemented in display.c --- */
void print_banner(void);
void print_prompt(void);
void print_table(struct Record *records, int count);
void print_stats(struct Record *records, int count, struct VaultMeta *meta);
void cmd_about(void);

/* --- Functions implemented in auth.c --- */
int  verify_password(const char *input, struct VaultMeta *meta);
void get_masked_input(char *buf, int max_len);

/* --- Functions implemented in main.c --- */
void main_menu_loop(struct VaultMeta *meta);
void export_report(struct Record *recs, int n);

#endif /* VAULT_H */
