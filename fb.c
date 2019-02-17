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

void get_px(unsigned int x, unsigned int y, struct pixelformat *val)
{
	struct pixelformat *p = fb.base;
	p += y * fb.hr + x;

	val->b = p->b;
	val->g = p->g;
	val->r = p->r;
	val->_reserved = p->_reserved;
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

#define SAME_BUF	30
unsigned char is_same_color(struct pixelformat *a, struct pixelformat *b)
{
	int diff_b = a->b - b->b;
	if (diff_b < 0)
		diff_b *= -1;
	if (diff_b > SAME_BUF)
		return 0;

	int diff_g = a->g - b->g;
	if (diff_g < 0)
		diff_g *= -1;
	if (diff_g > SAME_BUF)
		return 0;

	int diff_r = a->r - b->r;
	if (diff_r < 0)
		diff_r *= -1;
	if (diff_r > SAME_BUF)
		return 0;

	return 1;
}

unsigned char is_trans_color(struct pixelformat *c)
{
	/* if ((c->b == 0) && (c->g == 0) && (c->r == 0) && (c->_reserved == 0)) */
	if (c->_reserved == 0)
		return 1;
	else
		return 0;
}

void draw_fg(struct file *img)
{
	/* struct pixelformat alpha; */
	/* memcpy(&alpha, fb.base, sizeof(struct pixelformat)); */

	struct pixelformat *img_ptr = (struct pixelformat *)img->data;
	unsigned int x, y;
	for (y = 0; y < fb.vr; y++) {
		for (x = 0; x < fb.hr; x++) {
			if (!is_trans_color(img_ptr))
				draw_px(x, y, img_ptr->r, img_ptr->g, img_ptr->b);
			img_ptr++;
		}
	}
}

void draw_bg(struct file *img)
{
	memcpy(fb.base, img->data, img->size);
}

void draw_image(struct image *img, unsigned int px, unsigned int py)
{
	unsigned int x, y;
	struct pixelformat *p = img->data;
	for (y = py; (y < (py + img->height)) && (y < fb.vr); y++) {
		for (x = px; (x < (px + img->width)) && (x < fb.hr); x++) {
			if (!is_trans_color(p))
				draw_px(x, y, p->r, p->g, p->b);
			p++;
		}
	}
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
