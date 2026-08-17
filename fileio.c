/*
 * fileio.c — Binary file I/O for CipherVault
 * Handles all fread/fwrite/fseek operations on vault.dat and vault.meta
 * Member A owns this file
 */

#include "fileio.h"
#include "crypto.h"

int save_record(struct Record *r) {
    if (!r) {
        return 0;
    }

    FILE *fp = fopen(VAULT_FILE, "ab");
    if (!fp) {
        printf("Error: Cannot open vault file.\n");
        return 0;
    }

    fwrite(r, sizeof(struct Record), 1, fp);
    fclose(fp);

    struct VaultMeta meta;
    if (load_meta(&meta)) {
        meta.total_records++;
        save_meta(&meta);
    }

    return 1;
}

int load_all_records(struct Record *buf, int max) {
    if (!buf || max <= 0) {
        return 0;
    }

    FILE *fp = fopen(VAULT_FILE, "rb");
    if (!fp) {
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    if (file_size > 0 && (file_size % (long)sizeof(struct Record)) != 0) {
        printf("Warning: vault file may be corrupted.\n");
    }
    fseek(fp, 0, SEEK_SET);

    int count = 0;
    while (count < max && fread(&buf[count], sizeof(struct Record), 1, fp) == 1) {
        count++;
    }

    fclose(fp);
    return count;
}

int update_record_in_file(struct Record *r) {
    if (!r || r->id <= 0) {
        return 0;
    }

    FILE *fp = fopen(VAULT_FILE, "r+b");
    if (!fp) {
        printf("Error: Cannot open vault for update.\n");
        return 0;
    }

    long offset = (long)(r->id - 1) * (long)sizeof(struct Record);
    if (fseek(fp, offset, SEEK_SET) != 0) {
        fclose(fp);
        return 0;
    }

    fwrite(r, sizeof(struct Record), 1, fp);
    fclose(fp);
    return 1;
}

void init_vault(const char *master_password) {
    struct VaultMeta meta;
    hash_password(master_password, meta.password_hash);
    meta.total_records = 0;
    strcpy(meta.last_login, "Never");
    meta.encryption_key = generate_key(master_password);
    save_meta(&meta);

    FILE *fp = fopen(VAULT_FILE, "wb");
    if (fp) {
        fclose(fp);
    }

    printf("Vault initialized successfully.\n");
}

int load_meta(struct VaultMeta *meta) {
    if (!meta) {
        return 0;
    }

    FILE *fp = fopen(META_FILE, "rb");
    if (!fp) {
        return 0;
    }

    size_t read_bytes = fread(meta, sizeof(struct VaultMeta), 1, fp);
    fclose(fp);

    return (read_bytes == 1) ? 1 : 0;
}

int save_meta(struct VaultMeta *meta) {
    if (!meta) {
        return 0;
    }

    FILE *fp = fopen(META_FILE, "wb");
    if (!fp) {
        printf("Error: Cannot save vault metadata.\n");
        return 0;
    }

    fwrite(meta, sizeof(struct VaultMeta), 1, fp);
    fclose(fp);
    return 1;
}
