#include <lib.h>

#define PACKET_BUF_SIZE	1024

static void dump_packet(unsigned char *p_buf, unsigned short p_len);

int main(void)
{
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

	unsigned char p_buf[PACKET_BUF_SIZE];
	unsigned short p_len;
	while (1) {
		receive_packet(p_buf, &p_len);
		if (p_len)
			dump_packet(p_buf, p_len);
	}

	return 0;
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
