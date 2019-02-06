#include <lib.h>

static void kbc_handler(unsigned char c);

int main(void)
{
	puts("beef sender\r\n");
	set_kbc_handler(kbc_handler);

	return 0;
}

static void kbc_handler(unsigned char c)
{
	unsigned int beef = 0xbeefbeef;
	putc('[');
	putc(c);
	send_packet(&beef, sizeof(beef));
	putc(']');
}
