#ifndef _FBCON_H_
#define _FBCON_H_

#define HORIZONTAL_WRITING	0
#define VERTICAL_WRITING	1

void set_text_direction(unsigned char test_direction);
void putc(char _c);
void puts(char *s);
void putd(unsigned long long val, unsigned char num_digits);
void puth(unsigned long long val, unsigned char num_digits);

#endif
