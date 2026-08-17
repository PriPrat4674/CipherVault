/*
 * auth.c — Authentication module for CipherVault
 * Handles password verification
 * Member A owns this file
 */

#include "auth.h"
#include "crypto.h"
#include <string.h>

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
