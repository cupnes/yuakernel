#ifndef _IV_H_
#define _IV_H_

#define MAX_IV_FILES	100

void view(unsigned long long idx);
void iv_init(void);
void iv_kbc_handler(char c);

extern unsigned long long iv_num_files;
extern unsigned long long iv_idx;

#endif
