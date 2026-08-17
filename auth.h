#ifndef AUTH_H
#define AUTH_H

#include "vault.h"

int verify_password(const char *input, struct VaultMeta *meta);

#endif /* AUTH_H */
