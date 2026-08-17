/*
 * test_b.c — Standalone integration test for Member B's modules
 * Tests vault_ops.c and search.c WITHOUT requiring main.c or Member A's sources
 * Compile: gcc test_b.c vault_ops.c search.c -o test_b
 * Run:     ./test_b
 */

#include <stdio.h>
#include <string.h>

#include "vault.h"
#include "vault_ops.h"
#include "search.h"

/* ================================================================
 * STUBS — crypto.c
 * ================================================================ */
void encrypt_string(char *str, int key)          { (void)str; (void)key; }
void decrypt_string(char *str, int key)          { (void)str; (void)key; }
void encrypt_record(struct Record *r, int key)   { (void)r;   (void)key; }
void decrypt_record(struct Record *r, int key)   { (void)r;   (void)key; }

void hash_password(const char *pwd, char *out_hash)
{
    (void)pwd;
    strcpy(out_hash, "TESTHASH");
}

int generate_key(const char *pwd)
{
    (void)pwd;
    return 13;
}

/* ================================================================
 * STUBS — fileio.c
 * ================================================================ */
int save_record(struct Record *r)
{
    printf("[STUB] save_record called for id=%d\n", r->id);
    return 1;
}

int load_all_records(struct Record *buf, int max)
{
    (void)buf;
    (void)max;
    return 0; /* empty vault */
}

int update_record_in_file(struct Record *r)
{
    printf("[STUB] update_record id=%d\n", r->id);
    return 1;
}

void init_vault(const char *pwd)
{
    (void)pwd;
    printf("[STUB] init_vault called\n");
}

int load_meta(struct VaultMeta *m)
{
    m->encryption_key  = 13;
    m->total_records   = 0;
    strcpy(m->password_hash, "TESTHASH");
    strcpy(m->last_login,    "Never");
    return 1;
}

int save_meta(struct VaultMeta *m)
{
    (void)m;
    return 1;
}

/* ================================================================
 * STUBS — display.c
 * ================================================================ */
void print_banner(void)
{
    printf("[CipherVault Test Mode]\n");
}

void print_table(struct Record *r, int n)
{
    (void)r;
    printf("[STUB] print_table called, %d records\n", n);
}

void print_stats(struct Record *r, int n, struct VaultMeta *m)
{
    (void)r; (void)n; (void)m;
    printf("[STUB] stats\n");
}

void cmd_about(void)
{
    printf("[STUB] about\n");
}

/* ================================================================
 * STUB — verify_password (used internally by cmd_change_password)
 * ================================================================ */
int verify_password(const char *input, struct VaultMeta *meta)
{
    (void)input; (void)meta;
    return 1;
}

/* ================================================================
 * main — runs each Member B function in sequence
 * ================================================================ */
int main(void)
{
    printf("=== Member B Module Tests ===\n");

    printf("\nTest 1: cmd_add_record — will prompt for input, follow prompts\n");
    cmd_add_record(13);

    printf("\nTest 2: cmd_view_all — should show empty vault\n");
    cmd_view_all(13);

    printf("\nTest 3: cmd_search — will prompt for mode, enter 1 then any keyword\n");
    cmd_search(13);

    printf("\nTest 4: cmd_sort — will prompt for mode, enter 1\n");
    cmd_sort(13);

    printf("\n=== All stubs completed ===\n");

    return 0;
}
