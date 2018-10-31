#include <lib.h>

int main(void)
{
	syscall(SYSCALL_PUTC, 'A', 0, 0);
	while (1);

	return 0;
}
