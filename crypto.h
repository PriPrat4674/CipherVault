#ifndef CRYPTO_H
#define CRYPTO_H

#include "vault.h"

void encrypt_string(char *str, int key);
void decrypt_string(char *str, int key);
void encrypt_record(struct Record *r, int key);
void decrypt_record(struct Record *r, int key);
void hash_password(const char *pwd, char *out_hash);
int  generate_key(const char *password);

#endif /* CRYPTO_H */
