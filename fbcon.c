#include <fbcon.h>
#include <fb.h>
#include <font.h>

/* 64bit unsignedの最大値0xffffffffffffffffは
 * 10進で18446744073709551615(20桁)なので'\0'含め21文字分のバッファで足りる */
#define MAX_STR_BUF	21

unsigned int cursor_x = 0, cursor_y = 0;

void putc(char _c)
{
	unsigned int x, y;

	unsigned char c = (unsigned char)_c;
	if (c == '\r') {
		cursor_x = 0;
	} else if (c == '\n') {
		cursor_y += FONT_HEIGHT;
		if ((cursor_y + FONT_HEIGHT) >= fb.vr) {
			cursor_x = cursor_y = 0;
			clear_screen();
		}
	} else if (c < 128) {
		/* カーソル座標(cursor_x,cursor_y)へ文字を描画 */
		for (y = 0; y < FONT_HEIGHT; y++)
			for (x = 0; x < FONT_WIDTH; x++)
				if (font_bitmap[(unsigned int)c][y][x])
					draw_px_fg(cursor_x + x, cursor_y + y);

		/* カーソル座標の更新 */
		cursor_x += FONT_WIDTH;
		if ((cursor_x + FONT_WIDTH) >= fb.hr) {
			cursor_x = 0;
			cursor_y += FONT_HEIGHT;
			if ((cursor_y + FONT_HEIGHT) >= fb.vr) {
				cursor_x = cursor_y = 0;
				clear_screen();
			}
		}
	} else {
		/* カーソル座標(cursor_x,cursor_y)へ文字を描画 */
		for (y = 0; y < FONT_HIRA_HEIGHT; y++)
			for (x = 0; x < FONT_HIRA_WIDTH; x++)
				if (font_hira_bitmap[(unsigned int)c][y][x])
					draw_px_fg(cursor_x + x, cursor_y + y);

		/* カーソル座標の更新 */
		cursor_x += FONT_HIRA_WIDTH;
		if ((cursor_x + FONT_HIRA_WIDTH) >= fb.hr) {
			cursor_x = 0;
			cursor_y += FONT_HIRA_HEIGHT;
			if ((cursor_y + FONT_HIRA_HEIGHT) >= fb.vr) {
				cursor_x = cursor_y = 0;
				clear_screen();
			}
		}
	}
}

void puts(char *s)
{
	while (*s != '\0')
		putc(*s++);
}

void putd(unsigned long long val, unsigned char num_digits)
{
	char str[MAX_STR_BUF];

	int i;
	for (i = num_digits - 1; i >= 0; i--) {
		int digit = val % 10;
		str[i] = '0' + digit;
		val /= 10;
	}
	str[num_digits] = '\0';

	puts(str);
}

void puth(unsigned long long val, unsigned char num_digits)
{
	char str[MAX_STR_BUF];

	int i;
	for (i = num_digits - 1; i >= 0; i--) {
		unsigned char v = (unsigned char)(val & 0x0f);
		if (v < 0xa)
			str[i] = '0' + v;
		else
			str[i] = 'A' + (v - 0xa);
		val >>= 4;
	}
	str[num_digits] = '\0';

	puts(str);
}
