#include <lib.h>

int main(void)
{
	while (1) {
		char c = ser_getc();
		if (c == '\r')
			ser_putc('\n');
		ser_putc(c);
	}

	return 0;
}
