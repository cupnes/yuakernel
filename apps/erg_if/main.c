#include <lib.h>

/* sysfile definition */
#define SFN_BG_IMG	"bg.bgra"
#define SFN_YUA_IMG	"yua.bgra"
enum SYSFILE_ID {
	SFID_BG_IMG,
	SFID_YUA_IMG,
	SFID_MAX
};

/* other definition */
#define YUA_WIDTH	250
#define CURSOR_MASK_SIZE	10240
#define FILELIST_BASE_Y	160
#define FILELIST_BASE_X	240
#define FILELIST_NAME_X	(FILELIST_BASE_X + 10)
#define FILELIST_MAX_NUM	9
#define BG_R	0
#define BG_G	0
#define BG_B	0
#define MAX_SYSFILES	20

static void open_sysfiles(void);
static void init(void);
static void kbc_handler(unsigned char c);
static void make_mask(unsigned int base_x, unsigned int base_y,
		      struct image *img, struct image *mask);
static void ls(void);

struct file *sysfile_list[MAX_SYSFILES] = { 0 };

struct image *cursor_img;
struct image *cursor_mask;
unsigned char cursor_mask_data[CURSOR_MASK_SIZE];
struct file *filelist[MAX_FILES];
unsigned char filelist_num;
unsigned char current_file_idx;
int urclock_tid;

int main(void)
{
	open_sysfiles();
	init();

	return 0;
}

static void open_sysfiles(void)
{
	sysfile_list[SFID_BG_IMG] = open(SFN_BG_IMG);
	sysfile_list[SFID_YUA_IMG] = open(SFN_YUA_IMG);
}

static void init(void)
{
	set_kbc_handler(kbc_handler);

	set_bg(BG_R, BG_G, BG_B);

	draw_bg(sysfile_list[SFID_BG_IMG]);

	/* draw_fg(sysfile_list[SFID_YUA_IMG]); */

	ls();

	urclock_tid = exec_bg(open("urclock"));
}

static void view_image(struct file *img_file)
{
	set_bg(BG_R, BG_G, BG_B);
	clear_screen();
	draw_image((struct image *)img_file->data, 0, 0);
}

static void kbc_handler(unsigned char c)
{
	static unsigned char is_running_task = 0;

	if (is_running_task) {
		is_running_task = 0;
		init();
		return;
	}

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

	case KEY_ENTER:
		switch (filelist[current_file_idx]->name[0]) {
		case 'i':
			is_running_task = 1;
			finish_task(urclock_tid);
			view_image(filelist[current_file_idx]);
			break;
		}
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

static void make_mask(unsigned int base_x, unsigned int base_y,
		      struct image *img, struct image *mask)
{
	mask->width = img->width;
	mask->height = img->height;

	struct pixelformat *img_p = img->data;
	struct pixelformat *mask_p = mask->data;

	unsigned int x, y;
	for (y = base_y; y < (base_y + img->height); y++) {
		for (x = base_x; x < (base_x + img->width); x++) {
			if (!is_trans_color(img_p)) {
				struct pixelformat tmp_p;
				get_px(x, y, &tmp_p);
				mask_p->b = tmp_p.b;
				mask_p->g = tmp_p.g;
				mask_p->r = tmp_p.r;
				mask_p->_reserved = tmp_p._reserved;
			} else {
				mask_p->b = 0;
				mask_p->g = 0;
				mask_p->r = 0;
				mask_p->_reserved = 0;
			}
			img_p++;
			mask_p++;
		}
	}
}

static unsigned char is_sysfile(struct file *f __attribute__((unused)))
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
	filelist_num = 0;
	for (i = 0; i < num_files; i++) {
		if (is_sysfile(f[i]))
			continue;

		puts(f[i]->name);
		move_cursor(FILELIST_NAME_X,
			    (FONT_HEIGHT * (i + 1)) + FILELIST_BASE_Y);
		filelist[filelist_num] = f[i];
		filelist_num++;
	}

	struct file *cursor_file = open("i.cursor");
	cursor_img = (struct image *)cursor_file->data;
	cursor_mask = (struct image *)cursor_mask_data;
	make_mask(FILELIST_BASE_X, FILELIST_BASE_Y, cursor_img, cursor_mask);

	current_file_idx = 0;
	draw_image(cursor_img, FILELIST_BASE_X,
		   FILELIST_BASE_Y + (FONT_HEIGHT * current_file_idx));
}
