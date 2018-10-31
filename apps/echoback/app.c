#include <lib.h>

int main(void)
{
	while (1) {
		char c = getc();
		putc(c);
		if (c == '\n')
			putc('\r');
	}

	return 0;
}
