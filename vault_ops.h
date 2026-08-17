#ifndef VAULT_OPS_H
#define VAULT_OPS_H

#include "vault.h"

void cmd_add_record(int enc_key);
void cmd_view_all(int enc_key);
void cmd_update_record(int id, int enc_key);
void cmd_delete_record(int id);
void cmd_purge(int enc_key);
void cmd_change_password(struct VaultMeta *meta);

#endif /* VAULT_OPS_H */
