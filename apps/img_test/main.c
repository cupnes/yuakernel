#include <lib.h>

#define IMG_BG	"bg.bgra"
#define IMG_YUA	"yua.bgra"
#define YUA_WIDTH	200

static void ls(void);

int main(void)
{
    struct file *bg = open(IMG_BG);
    draw_bg(bg);

    struct file *yua = open(IMG_YUA);
    draw_fg(yua);

    ls();

	return 0;
}

static void ls(void)
{
    struct file *f[MAX_FILES];
	unsigned long long num_files = get_files(f);
    unsigned long long i;
    move_cursor(YUA_WIDTH, 0);
	for (i = 0; i < num_files; i++) {
		puts((unsigned char *)f[i]->name);
        puts("\r\n");
        move_cursor(YUA_WIDTH, FONT_HEIGHT * i);
	}
	vputs((unsigned char *)"\r\n");


}
