#include <lib.h>

#define BG_FILE_NAME	"bg.bgra"
#define FG_FILE_NAME	"yua.bgra"

/* static void kbc_handler(unsigned char c); */

int main(void)
{
	struct file *bg = open(BG_FILE_NAME);
	draw_bg(bg);

	struct file *yua = open(FG_FILE_NAME);
	draw_fg(yua);

	return 0;
}

/* static void kbc_handler(unsigned char c) */
/* { */
/* 	unsigned int beef = 0xbeefbeef; */
/* 	putc('['); */
/* 	putc(c); */
/* 	send_packet(&beef, sizeof(beef)); */
/* 	putc(']'); */
/* } */
