#include <lib.h>

static void kbc_handler(char c);

int main(void)
{
	set_kbc_handler(kbc_handler);

	while (1);

	return 0;
}

static void kbc_handler(char c)
{
	putc(c);
	if (c == '\n')
		putc('\r');
}
