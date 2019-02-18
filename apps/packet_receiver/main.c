#include <lib.h>

int main(void)
{
	unsigned int beef = 0xbeefbeef;
	send_packet(&beef, sizeof(beef));

	return 0;
}
