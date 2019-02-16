#include <lib.h>

#define BG_FILE_NAME	"bg.bgra"
#define FG_FILE_NAME	"yua.bgra"
#define YUA_WIDTH	250

/* static void kbc_handler(unsigned char c); */
static void ls(void);

int main(void)
{
	struct file *bg = open(BG_FILE_NAME);
	draw_bg(bg);

	struct file *yua = open(FG_FILE_NAME);
	/* draw_fg(yua); */

	ls();

	exec_bg(open("urclock"));

	return 0;
}

/* static void kbc_handler(unsigned char c) */
/* { */
/* 	unsigned int beef = 0xbeefbeef; */
/* 	putc('['); */
/* 	putc(c); */
/* 	send_packet(&beef, sizeof(beef)); */
/* 	putc(']'); */
/* } */

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

	struct file *cursor_file = open("i.cursor");
	draw_image((struct image *)&cursor_file->data,
		   YUA_WIDTH - 10, PADDING_Y);
}
