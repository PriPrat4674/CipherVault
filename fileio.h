#ifndef FILEIO_H
#define FILEIO_H

#include "vault.h"

int  save_record(struct Record *r);
int  load_all_records(struct Record *buf, int max);
int  update_record_in_file(struct Record *r);
void init_vault(const char *master_password);
int  load_meta(struct VaultMeta *meta);
int  save_meta(struct VaultMeta *meta);

#endif /* FILEIO_H */
