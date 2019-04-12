#include <lib.h>

/* sysfile definition */
#define SFN_INIT_EXE	"init"
#define SFN_BG_IMG	"bg.bgra"
#define SFN_YUA_IMG	"i.yua"
#define SFN_YUAM_IMG	"i.yuam"
#define SFN_YUA43_IMG	"i.yua0143"
#define SFN_YUAM43_IMG	"i.yuam43"
#define SFN_URC_EXE	"urclock"
#define SFN_URC_WIN	"urclockbg.bgra"
#define SFN_LS_WIN	"lsbg.bgra"
#define SFN_LS_CUR	"i.cursor"
enum SYSFILE_ID {
	SFID_INIT_EXE,
	SFID_BG_IMG,
	SFID_YUA_IMG,
	SFID_YUAM_IMG,
	SFID_YUA43_IMG,
	SFID_YUAM43_IMG,
	SFID_URC_EXE,
	SFID_URC_WIN,
	SFID_LS_WIN,
	SFID_LS_CUR,
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

#define GENIUS_TH	10

static void open_sysfiles(void);
static void redraw(void);
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
unsigned char current_file_idx = 0;
int urclock_tid;
unsigned char is_43 = 0;
unsigned char is_megane = 0;
unsigned int exec_counter = 0;

#define OSUNC_NUM_SLIDES 15
struct file e_osunc = {
	"e.osunc", 0
};
static void osunc_init(void);
static void osunc_start(void);
static void osunc_kbdhdr(unsigned char c);
unsigned char is_running_osunc = 0;
char osunc_file_names[OSUNC_NUM_SLIDES][10] = {
	"i.osunc00",
	"i.osunc01",
	"i.osunc02",
	"i.osunc03",
	"i.osunc04",
	"i.osunc05",
	"i.osunc06",
	"i.osunc07",
	"i.osunc08",
	"i.osunc09",
	"i.osunc10",
	"i.osunc11",
	"i.osunc12",
	"i.osunc13",
	"i.osunc14"
};
struct file *osunc_files[OSUNC_NUM_SLIDES];
unsigned int osunc_idx;

/* TODO: current_yua */

int main(void)
{
	open_sysfiles();
	/* current_yua = sysfile_list[SFID_YUA_IMG]; */
	osunc_init();

	redraw();

	return 0;
}

static void open_sysfiles(void)
{
	sysfile_list[SFID_INIT_EXE] = open(SFN_INIT_EXE);
	sysfile_list[SFID_BG_IMG] = open(SFN_BG_IMG);
	sysfile_list[SFID_YUA_IMG] = open(SFN_YUA_IMG);
	sysfile_list[SFID_YUAM_IMG] = open(SFN_YUAM_IMG);
	sysfile_list[SFID_YUA43_IMG] = open(SFN_YUA43_IMG);
	sysfile_list[SFID_YUAM43_IMG] = open(SFN_YUAM43_IMG);
	sysfile_list[SFID_URC_EXE] = open(SFN_URC_EXE);
	sysfile_list[SFID_URC_WIN] = open(SFN_URC_WIN);
	sysfile_list[SFID_LS_WIN] = open(SFN_LS_WIN);
	sysfile_list[SFID_LS_CUR] = open(SFN_LS_CUR);
}

static void redraw(void)
{
	set_kbc_handler(kbc_handler);

	set_bg(BG_R, BG_G, BG_B);

	draw_bg(sysfile_list[SFID_BG_IMG]);

	if (exec_counter >= GENIUS_TH)
		is_megane = 1;

	struct file *f;
	if (!is_43) {
		if (!is_megane)
			f = sysfile_list[SFID_YUA_IMG];
		else
			f = sysfile_list[SFID_YUAM_IMG];
	} else {
		if (!is_megane)
			f = sysfile_list[SFID_YUA43_IMG];
		else
			f = sysfile_list[SFID_YUAM43_IMG];
	}
	draw_image((struct image *)f->data, 30, 0);

	ls();

	urclock_tid = exec_bg(sysfile_list[SFID_URC_EXE]);
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
		redraw();
		return;
	}

	if (is_running_osunc) {
		osunc_kbdhdr(c);
		if (!is_running_osunc) {
			finish_task(urclock_tid);
			redraw();
		}
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
			exec_counter++;
			is_running_task = 1;
			finish_task(urclock_tid);
			view_image(filelist[current_file_idx]);
			break;

		case 'e':
			exec_counter++;
			finish_task(urclock_tid);
			if (filelist[current_file_idx] == &e_osunc)
				osunc_start();
			else {
				exec(filelist[current_file_idx]);
				redraw();
			}
			break;
		}
		break;

	case 't':
		is_43 = !is_43;
		finish_task(urclock_tid);
		redraw();
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

static unsigned char is_sysfile(struct file *f)
{
	int i;
	for (i = 0; i < SFID_MAX; i++) {
		if (f == sysfile_list[i])
			return 1;
	}
	for (i = 0; i < OSUNC_NUM_SLIDES; i++) {
		if (f == osunc_files[i])
			return 1;
	}
	return 0;
}

static void ls(void)
{
	draw_fg(sysfile_list[SFID_LS_WIN]);

	struct file *f[MAX_FILES];
	unsigned long long num_files = get_files(f);

	f[num_files] = &e_osunc;
	num_files++;

	unsigned long long i;
	move_cursor(FILELIST_NAME_X, FILELIST_BASE_Y);
	filelist_num = 0;
	for (i = 0; i < num_files; i++) {
		if (is_sysfile(f[i]))
			continue;

		puts(f[i]->name);
		move_cursor(FILELIST_NAME_X,
			    (FONT_HEIGHT * (filelist_num + 1))
			    + FILELIST_BASE_Y);
		filelist[filelist_num] = f[i];
		filelist_num++;
	}

	struct file *cursor_file = sysfile_list[SFID_LS_CUR];
	cursor_img = (struct image *)cursor_file->data;
	cursor_mask = (struct image *)cursor_mask_data;
	make_mask(FILELIST_BASE_X, FILELIST_BASE_Y, cursor_img, cursor_mask);

	draw_image(cursor_img, FILELIST_BASE_X,
		   FILELIST_BASE_Y + (FONT_HEIGHT * current_file_idx));
}

static void osunc_init(void)
{
	unsigned int i;
	for (i = 0; i < OSUNC_NUM_SLIDES; i++)
		osunc_files[i] = open(osunc_file_names[i]);
}

static void osunc_start(void)
{
	is_running_osunc = 1;
	osunc_idx = 0;
	draw_image((struct image *)osunc_files[osunc_idx]->data, 0, 0);
}

static void osunc_kbdhdr(unsigned char c)
{
	switch (c) {
	case 'j':
		if (osunc_idx < (OSUNC_NUM_SLIDES - 1))
			draw_image(
				(struct image *)osunc_files[++osunc_idx]->data,
				0, 0);
		break;

	case 'k':
		if (osunc_idx > 0)
			draw_image(
				(struct image *)osunc_files[--osunc_idx]->data,
				0, 0);
		break;

	case 'e':
		is_running_osunc = 0;
		break;
	}
}
