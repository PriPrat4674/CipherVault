#ifndef DISPLAY_H
#define DISPLAY_H

#include "vault.h"

void print_banner(void);
void print_prompt(void);
void print_table(struct Record *records, int count);
void print_stats(struct Record *records, int count, struct VaultMeta *meta);
void cmd_about(void);

#endif /* DISPLAY_H */
