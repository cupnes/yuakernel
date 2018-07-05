#ifndef _FBCON_H_
#define _FBCON_H_

#define HORIZONTAL_WRITING	0
#define VERTICAL_WRITING	1

void putc_pos(unsigned int pos_x, unsigned int pos_y, unsigned char c);
void putc(char _c);
void puts(char *s);
void putd(unsigned long long val, unsigned char num_digits);
void puth(unsigned long long val, unsigned char num_digits);
void vcursor_reset(void);
void vputc(unsigned char c);
void vputs(unsigned char *s);

#endif
