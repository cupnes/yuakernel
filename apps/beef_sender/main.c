#include <lib.h>

int main(void)
{
	puts("beef sender\r\n");

	unsigned short data = 0xbeef;
	while (1) {
		getc();
		putc('a');
		send_packet(&data, 2);
	}

	return 0;
}
