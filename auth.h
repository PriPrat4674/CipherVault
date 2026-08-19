#ifndef AUTH_H
#define AUTH_H

#include "vault.h"

int  verify_password(const char *input, struct VaultMeta *meta);
void get_masked_input(char *buf, int max_len);

#endif /* AUTH_H */
