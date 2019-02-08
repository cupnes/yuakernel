#include <lib.h>

#define BG_FILE_NAME	"bg.bgra"

/* static void kbc_handler(unsigned char c); */

int main(void)
{
	puts("apps start\r\n");

	fb_init();

	struct framebuffer *f = (struct framebuffer *)&fb;

	puts("addrA:");
	puth((unsigned long long)f, 16);
	puts("\r\n");
	puth((unsigned long long)f->base, 16);
	puts("\r\n");
	puth(f->hr, 3);
	putc('x');
	puth(f->vr, 3);
	puts("\r\n");

	/* struct file *bg = open(BG_FILE_NAME); */
	/* fill_img(bg); */

	/* 	puts("beef sender\r\n"); */
	/* 	set_kbc_handler(kbc_handler); */

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
