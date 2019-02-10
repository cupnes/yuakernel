#include <lib.h>

#define BASE_X	385
#define BASE_Y	12

static void draw(struct datetime *dt);
static void update(struct datetime *dt);

int main(void)
{
	struct file *window = open("urclockbg.bgra");

	struct datetime dt;
	get_datetime(&dt);

	while (1) {
		draw_fg(window);
		draw(&dt);

		sleep(1000000);

		update(&dt);
	}

	return 0;
}

static void draw(struct datetime *dt)
{
	move_cursor(BASE_X, BASE_Y);
	putd(dt->year, 4);
	putc('-');
	putd(dt->mon, 2);
	putc('-');
	putd(dt->day, 2);

	move_cursor(BASE_X + FONT_WIDTH, BASE_Y + FONT_HEIGHT);
	putd(dt->hour, 2);
	putc(':');
	putd(dt->min, 2);
	putc(':');
	putd(dt->sec, 2);
}

static void update(struct datetime *dt)
{
	dt->sec++;
	if (dt->sec >= 60) {
		dt->sec = 0;
		dt->min++;
	} else
		return;

	if (dt->min >= 60) {
		dt->min = 0;
		dt->hour++;
	} else
		return;

	if (dt->hour >= 24) {
		dt->hour = 0;
		dt->day++;
	} else
		return;

	/* FIXME: update year and mon */
}
