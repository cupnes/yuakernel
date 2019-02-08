#include <fb.h>
#include <common.h>

struct framebuffer fb;
struct pixelformat color_fg;
struct pixelformat color_bg;

void fb_init(struct framebuffer *_fb)
{
	fb.base = _fb->base;
	fb.size = _fb->size;
	fb.hr = _fb->hr;
	fb.vr = _fb->vr;
}

void set_fg(unsigned char r, unsigned char g, unsigned char b)
{
	color_fg.b = b;
	color_fg.g = g;
	color_fg.r = r;
}

void set_bg(unsigned char r, unsigned char g, unsigned char b)
{
	color_bg.b = b;
	color_bg.g = g;
	color_bg.r = r;
}

inline void draw_px(unsigned int x, unsigned int y,
		    unsigned char r, unsigned char g, unsigned char b)
{
	struct pixelformat *p = fb.base;
	p += y * fb.hr + x;

	p->b = b;
	p->g = g;
	p->r = r;
}

inline void draw_px_fg(unsigned int x, unsigned int y)
{
	draw_px(x, y, color_fg.r, color_fg.g, color_fg.b);
}

unsigned char is_same_color(struct pixelformat *a, struct pixelformat *b)
{
	if ((a->b == b->b) && (a->g == b->g) && (a->r == b->r))
		return 1;
	else
		return 0;
}

void draw_fg(struct file *img)
{
	struct pixelformat alpha;
	memcpy(&alpha, fb.base, sizeof(struct pixelformat));

	struct pixelformat *img_ptr = (struct pixelformat *)img->data;
	unsigned int x, y;
	for (y = 0; y < fb.vr; y++) {
		for (x = 0; x < fb.hr; x++) {
			if (!is_same_color(img_ptr, &alpha)) {
				draw_px(x, y, img_ptr->r, img_ptr->g, img_ptr->b);
			}
			img_ptr++;
		}
	}
}

void draw_bg(struct file *img)
{
	memcpy(fb.base, img->data, img->size);
}

inline void fill_rect(unsigned int x, unsigned int y,
		      unsigned int w, unsigned int h,
		      unsigned char r, unsigned char g, unsigned char b)
{
	unsigned int i, j;
	for (i = y; i < (y + h); i++)
		for (j = x; j < (x + w); j++)
			draw_px(j, i, r, g, b);
}

void clear_screen(void)
{
	fill_rect(0, 0, fb.hr, fb.vr, color_bg.r, color_bg.g, color_bg.b);
}
