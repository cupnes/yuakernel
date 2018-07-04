#include <fbcon.h>
#include <font.h>

void kana_main(void)
{
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
