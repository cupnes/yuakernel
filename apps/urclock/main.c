#include <lib.h>

#define BASE_X	400
#define BASE_Y	10

static void draw(struct datetime *dt);
static void update(struct datetime *dt);

int main(void)
{
	/* struct datetime dt; */
	/* get_datetime(&dt); */

	while (1) {
		/* draw(&dt); */
		putc('.');

		sleep(1000000);

		/* update(&dt); */
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
	/* putc(':'); */
	/* putd(dt->sec, 2); */
}

static void update(struct datetime *dt)
{
	dt->sec++;
	if (dt->sec >= 60)
		dt->min++;
	else
		return;

	if (dt->min >= 60)
		dt->hour++;
	else
		return;

	if (dt->hour >= 24)
		dt->day++;
	else
		return;

	/* FIXME: update year and mon */
}
