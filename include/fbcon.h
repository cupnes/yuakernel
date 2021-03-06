#pragma once

void putc(char c);
void puts(char *s);
void putd(unsigned long long val, unsigned char num_digits);
void puth(unsigned long long val, unsigned char num_digits);
void vcursor_reset(void);
void vputc(unsigned char c);
void vputs(unsigned char *s);
void move_cursor(unsigned int x, unsigned int y);
unsigned int get_cursor_y(void);
void set_font_size(unsigned char _font_size);
