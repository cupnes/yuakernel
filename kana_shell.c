#include <fbcon.h>
#include <font.h>
#include <kbc.h>

static void kbc_handler(char c)
{
	puth(c, 2);
	putc(' ');
}

void kana_main(void)
{
	kbc_set_handler(kbc_handler);

	unsigned char s[] = {
		FONT_hira_ko,
		FONT_hira_n,
		FONT_hira_ni,
		FONT_hira_chi,
		FONT_hira_ha,
		'\0'};
	puts((char *)s);
	puts("HELLO\r\n");
}
