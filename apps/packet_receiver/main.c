#include <lib.h>

#define PACKET_BUF_SIZE	1024

static void kbc_handler(unsigned char c);
static void dump_packet(unsigned char *p_buf, unsigned short p_len);

unsigned char is_exit = 0;

int main(void)
{
	set_kbc_handler(kbc_handler);

	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

	unsigned char p_buf[PACKET_BUF_SIZE];
	unsigned short p_len;
	while (!is_exit) {
		receive_packet(p_buf, &p_len);
		if (p_len)
			dump_packet(p_buf, p_len);
	}

	return 0;
}

static void kbc_handler(unsigned char c __attribute__((unused)))
{
	is_exit = 1;
}

static void dump_packet(unsigned char *p_buf, unsigned short p_len)
{
	unsigned int i;
	for (i = 0; i < p_len; i++) {
		puth(p_buf[i], 2);
		putc(' ');
	}
	puts("\r\n");
}
