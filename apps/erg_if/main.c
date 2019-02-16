#include <lib.h>

#define BG_FILE_NAME	"bg.bgra"
#define FG_FILE_NAME	"yua.bgra"
#define YUA_WIDTH	250

static void kbc_handler(unsigned char c);
static void ls(void);

unsigned int cursor_x = 0;
unsigned int cursor_y = 0;
struct image *cursor_img;

int main(void)
{
	set_kbc_handler(kbc_handler);

	struct file *bg = open(BG_FILE_NAME);
	draw_bg(bg);

	/* struct file *yua = open(FG_FILE_NAME); */
	/* draw_fg(yua); */

	ls();

	exec_bg(open("urclock"));

	return 0;
}

static void kbc_handler(unsigned char c)
{
	switch (c) {
	case KEY_UP:
		cursor_y -= FONT_HEIGHT;
		break;

	case KEY_DOWN:
		cursor_y += FONT_HEIGHT;
		break;
	}

	draw_image(cursor_img, cursor_x, cursor_y);
}

#define PADDING_Y	170
static void ls(void)
{
	struct file *ls_window = open("lsbg.bgra");
	draw_fg(ls_window);

	struct file *f[MAX_FILES];
	unsigned long long num_files = get_files(f);
	unsigned long long i;
	move_cursor(YUA_WIDTH, PADDING_Y);
	for (i = 0; i < num_files; i++) {
		puts(f[i]->name);
		move_cursor(YUA_WIDTH, (FONT_HEIGHT * (i + 1)) + PADDING_Y);
	}

	cursor_x = YUA_WIDTH - 10;
	cursor_y = PADDING_Y;
	struct file *cursor_file = open("i.cursor");
	cursor_img = (struct image *)cursor_file->data;
	draw_image(cursor_img, cursor_x, cursor_y);
}
