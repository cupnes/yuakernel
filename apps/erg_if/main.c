#include <lib.h>

#define BG_FILE_NAME	"bg.bgra"
#define FG_FILE_NAME	"yua.bgra"
#define YUA_WIDTH	250
#define CURSOR_MASK_SIZE	10240
#define FILELIST_BASE_Y	160
#define FILELIST_BASE_X	240
#define FILELIST_NAME_X	(FILELIST_BASE_X + 10)
#define FILELIST_MAX_NUM	9

static void kbc_handler(unsigned char c);
static void make_cursor_mask(void);
static void ls(void);

unsigned int cursor_x = 0;
unsigned int cursor_y = 0;
struct image *cursor_img;
struct image *cursor_mask;
unsigned char cursor_mask_data[CURSOR_MASK_SIZE];
unsigned char filelist_num = 0;

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
	static unsigned char current_file_idx = 0;
	unsigned char next_file_idx;

	next_file_idx = current_file_idx;
	switch (c) {
	case KEY_UP:
		if (current_file_idx > 0)
			next_file_idx = current_file_idx - 1;
		break;

	case KEY_DOWN:
		if (current_file_idx < (filelist_num - 1))
			next_file_idx = current_file_idx + 1;
		break;
	}

	if (next_file_idx != current_file_idx) {
		draw_image(cursor_img, FILELIST_BASE_X,
			   FILELIST_BASE_Y + (FONT_HEIGHT * next_file_idx));
		draw_image(cursor_mask, FILELIST_BASE_X,
			   FILELIST_BASE_Y + (FONT_HEIGHT * current_file_idx));
		current_file_idx = next_file_idx;
	}
}

static unsigned char is_trans_color(struct pixelformat *c)
{
	if (c->_reserved == 0)
		return 1;
	else
		return 0;
}

static void make_cursor_mask(void)
{
	struct pixelformat bg_px;
	get_px(cursor_x, cursor_y, &bg_px);

	cursor_mask = (struct image *)cursor_mask_data;
	cursor_mask->width = cursor_img->width;
	cursor_mask->height = cursor_img->height;

	unsigned int x, y;
	struct pixelformat *img = cursor_img->data;
	struct pixelformat *mask = cursor_mask->data;
	for (y = 0; y < cursor_img->height; y++) {
		for (x = 0; x < cursor_img->width; x++) {
			if (!is_trans_color(img)) {
				mask->b = bg_px.b;
				mask->g = bg_px.g;
				mask->r = bg_px.r;
				mask->_reserved = bg_px._reserved;
			} else {
				mask->b = 0;
				mask->g = 0;
				mask->r = 0;
				mask->_reserved = 0;
			}
			img++;
			mask++;
		}
	}
}

static unsigned char is_sysfile(struct file *f)
{
	return 0;
}

static void ls(void)
{
	struct file *ls_window = open("lsbg.bgra");
	draw_fg(ls_window);

	struct file *f[MAX_FILES];
	unsigned long long num_files = get_files(f);
	unsigned long long i;
	move_cursor(FILELIST_NAME_X, FILELIST_BASE_Y);
	for (i = 0; i < num_files; i++) {
		if (is_sysfile(f[i]))
			continue;

		puts(f[i]->name);
		move_cursor(FILELIST_NAME_X,
			    (FONT_HEIGHT * (i + 1)) + FILELIST_BASE_Y);
		filelist_num++;
	}

	cursor_x = FILELIST_BASE_X;
	cursor_y = FILELIST_BASE_Y;
	struct file *cursor_file = open("i.cursor");
	cursor_img = (struct image *)cursor_file->data;
	make_cursor_mask();
	draw_image(cursor_img, cursor_x, cursor_y);
}
