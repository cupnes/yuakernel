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
	buf_idx = 0;
	unsigned char next_idx = (buf_idx + 1) % 2;

	unsigned int x, y;
	for (y = 0; y < SCREEN_HEIGHT; y++) {
		for (x = 0; x < SCREEN_WIDTH; x++) {
			screen_buf[next_idx][y][x] = 0;

			if ((x * y) % 2)
				screen_buf[buf_idx][y][x] = 1;
		}
	}
}

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

static unsigned char is_life(int x, int y)
{
	if (x < 0)
		x = SCREEN_WIDTH + x;
	if (y < 0)
		y = SCREEN_HEIGHT + y;

	return screen_buf[buf_idx][y][x];
}

static unsigned char get_neighbours_lives(int x, int y)
{
	unsigned char lives_num = 0;

	int nx, ny;
	for (ny = y - 1; ny <= y + 1; y++) {
		for (nx = x - 1; nx <= x + 1; nx++) {
			if ((ny == y) && (nx == x))
				continue;

			lives_num += is_life(x, y);
		}
	}

	return lives_num;
}

static void gen_next(void)
{
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
}

static void sleep_next(void)
{
	volatile unsigned int wait = 10000;
	while (wait--);
}
