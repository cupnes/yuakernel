#include <lib.h>

#define BASE_X	500
#define BASE_Y	10

int main(void)
{
	struct datetime dt;

	while (1) {
		get_datetime(&dt);

		move_cursor(BASE_X, BASE_Y);
		putd(dt.year, 4);
		putc('-');
		putd(dt.mon, 2);
		putc('-');
		putd(dt.day, 2);

		move_cursor(BASE_X + FONT_WIDTH, BASE_Y + FONT_HEIGHT);
		putd(dt.hour, 2);
		putc(':');
		putd(dt.min, 2);
		putc(':');
		putd(dt.sec, 2);

		sleep(1000000);
	}

	return 0;
}
