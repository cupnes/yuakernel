#include <lib.h>

#define BG_FILE_NAME	"bg.bgra"
#define FG_FILE_NAME	"yua.bgra"
#define YUA_WIDTH	250
#define CURSOR_MASK_SIZE	10240

static void kbc_handler(unsigned char c);
static void make_cursor_mask(void);
static void ls(void);

unsigned int cursor_x = 0;
unsigned int cursor_y = 0;
struct image *cursor_img;
struct image *cursor_mask;
unsigned char cursor_mask_data[CURSOR_MASK_SIZE];

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
	unsigned int next_y = cursor_y;
	switch (c) {
	case KEY_UP:
		next_y = cursor_y - FONT_HEIGHT;
		break;

	case KEY_DOWN:
		next_y = cursor_y + FONT_HEIGHT;
		break;
	}

	if (next_y != cursor_y) {
		draw_image(cursor_img, cursor_x, next_y);
		draw_image(cursor_mask, cursor_x, cursor_y);
		cursor_y = next_y;
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
	make_cursor_mask();
	draw_image(cursor_img, cursor_x, cursor_y);
}
