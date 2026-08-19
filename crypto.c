#include "crypto.h"

void encrypt_string(char *str, int key) {
    if (!str) {
        return;
    }
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];
        if (c >= 'A' && c <= 'Z') {
            str[i] = (char)(((c - 'A' + key) % 26) + 'A');
        } else if (c >= 'a' && c <= 'z') {
            str[i] = (char)(((c - 'a' + key) % 26) + 'a');
        } else {
            str[i] = (char)(c ^ 0x80);
        }
    }
}

void decrypt_string(char *str, int key) {
    if (!str) {
        return;
    }
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];
        if (c >= 'A' && c <= 'Z') {
            str[i] = (char)(((c - 'A' - key + 26) % 26) + 'A');
        } else if (c >= 'a' && c <= 'z') {
            str[i] = (char)(((c - 'a' - key + 26) % 26) + 'a');
        } else {
            str[i] = (char)(c ^ 0x80);
        }
    }
}

void encrypt_record(struct Record *r, int key) {
    if (!r) {
        return;
    }
    encrypt_string(r->field1, key);
    encrypt_string(r->field2, key);
    encrypt_string(r->field3, key);
}

void decrypt_record(struct Record *r, int key) {
    if (!r) {
        return;
    }
    decrypt_string(r->field1, key);
    decrypt_string(r->field2, key);
    decrypt_string(r->field3, key);
}

void hash_password(const char *pwd, char *out_hash) {
    if (!pwd || !out_hash) {
        return;
    }
    int sum = 0;
    for (int i = 0; pwd[i] != '\0'; i++) {
        sum += (unsigned char)pwd[i];
    }
    int hashed = sum ^ 0xDEAD;
    sprintf(out_hash, "%X", hashed);
}

int generate_key(const char *password) {
    if (!password) {
        return 1;
    }
    int sum = 0;
    for (int i = 0; password[i] != '\0'; i++) {
        sum += (unsigned char)password[i];
    }
    return (sum % 25) + 1;
}
