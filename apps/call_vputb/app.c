#include <lib.h>

#define WAIT_CLK	1000000
int main(void)
{
	enq_task(open("vputb"));

	while (1) {
		unsigned long long wait = WAIT_CLK;
		vputc('A');
		while (wait--);
	}

	return 0;
}
