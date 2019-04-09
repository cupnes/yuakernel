#include <lib.h>

#define BUF_NUM	2
unsigned char buf_idx;
unsigned char screen_buf[BUF_NUM][SCREEN_HEIGHT][SCREEN_WIDTH];

static void init(void);
static void draw(void);
static void gen_next(void);
static void sleep_next(void);

int main(void)
{
	set_fg(255, 255, 255);
	set_bg(0, 0, 0);
	clear_screen();

	init();
	draw();
	sleep_next();

	while (1) {
		gen_next();
		draw();
		sleep_next();
	}

	return 0;
}

static void init(void)
{
	struct datetime dt;
	get_datetime(&dt);
	unsigned short seed = ((unsigned short)dt.min << 8) | dt.sec;
	set_seed(seed);

	buf_idx = 0;
	unsigned char next_idx = (buf_idx + 1) % 2;

	unsigned int x, y;
	for (y = 0; y < SCREEN_HEIGHT; y++) {
		for (x = 0; x < SCREEN_WIDTH; x++) {
			screen_buf[next_idx][y][x] = 0;

			/* if ((x == y) || (y == x + 1) || (y == x + 2) || (y == SCREEN_HEIGHT - x) || (y == SCREEN_HEIGHT - x - 1)) */
			/* 	screen_buf[buf_idx][y][x] = 1; */

			/* if (((x + y) % 2) && ((x + y) % 3)) */
			/* 	screen_buf[buf_idx][y][x] = 1; */

			if ((rand() % 10000) >= 5000)
				screen_buf[buf_idx][y][x] = 1;
			else
				screen_buf[buf_idx][y][x] = 0;
		}
	}
}

#ifndef RUN_LOCAL
static void draw(void)
{
	unsigned char old_idx = (buf_idx + 1) % 2;
	unsigned int x, y;
	for (y = 0; y < SCREEN_HEIGHT; y++) {
		for (x = 0; x < SCREEN_WIDTH; x++) {
			if (screen_buf[buf_idx][y][x]
			    != screen_buf[old_idx][y][x]) {
				if (screen_buf[buf_idx][y][x])
					draw_px_fg(x, y);
				else
					draw_px_bg(x, y);
			}
		}
	}
}
#else
static void draw(void)
{
/* #ifdef RUN_LOCAL */
/* 	printf("## begin: %s\n", __func__); */
/* #endif */

	unsigned int x, y;
	for (y = 0; y < SCREEN_HEIGHT; y++) {
		for (x = 0; x < SCREEN_WIDTH; x++) {
			if (screen_buf[buf_idx][y][x])
				putchar('*');
			else
				putchar(' ');
		}
		putchar('\n');
	}
	printf("--\n");

/* #ifdef RUN_LOCAL */
/* 	printf("## end: %s\n", __func__); */
/* #endif */
}
#endif

static unsigned char is_life(int x, int y)
{
/* #ifdef RUN_LOCAL */
/* 	printf("## is_life: x=%d, y=%d\n", x, y); */
/* #endif */

	if (x < 0)
		x = SCREEN_WIDTH + x;
	if (y < 0)
		y = SCREEN_HEIGHT + y;

/* #ifdef RUN_LOCAL */
/* 	printf("## is_life: end: x=%d, y=%d\n", x, y); */
/* #endif */

	return screen_buf[buf_idx][y][x];
}

static unsigned char get_neighbours_lives(int x, int y)
{
/* #ifdef RUN_LOCAL */
/* 	printf("## %s: begin: x=%d, y=%d\n", __func__, x, y); */
/* #endif */

	unsigned char lives_num = 0;

	int nx, ny;
	for (ny = y - 1; ny <= y + 1; ny++) {
		for (nx = x - 1; nx <= x + 1; nx++) {
			if ((ny == y) && (nx == x))
				continue;

			lives_num += is_life(nx, ny);
		}
	}

/* #ifdef RUN_LOCAL */
/* 	printf("## %s: end: x=%d, y=%d, lives_num=%d\n", __func__, x, y, lives_num); */
/* #endif */

	return lives_num;
}

static void gen_next(void)
{
/* #ifdef RUN_LOCAL */
/* 	printf("## begin: %s\n", __func__); */
/* #endif */

	unsigned char next_idx = (buf_idx + 1) % 2;
	unsigned int x, y;
	for (y = 0; y < SCREEN_HEIGHT; y++) {
		for (x = 0; x < SCREEN_WIDTH; x++) {
			unsigned char nbs = get_neighbours_lives(x, y);
			if (is_life(x, y)) {
				if ((nbs == 2) || (nbs == 3))
					screen_buf[next_idx][y][x] = 1;
				else
					screen_buf[next_idx][y][x] = 0;
			} else {
				if (nbs == 3)
					screen_buf[next_idx][y][x] = 1;
				else
					screen_buf[next_idx][y][x] = 0;
			}
		}
	}

	buf_idx = next_idx;

/* #ifdef RUN_LOCAL */
/* 	printf("## end: %s\n", __func__); */
/* #endif */
}

#ifndef RUN_LOCAL
static void sleep_next(void)
{
	volatile unsigned int wait = 10000;
	while (wait--);
}
#else
static void sleep_next(void)
{
	getchar();
}
#endif
