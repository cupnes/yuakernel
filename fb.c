#include <fb.h>

struct framebuffer fb;
struct framebuffer fb_real;
struct pixelformat color_fg;
struct pixelformat color_bg;

unsigned int start_x, start_y;

void fb_init(struct framebuffer *_fb)
{
	fb.base = _fb->base;
	fb.size = _fb->size;
	fb.hr = _fb->hr;
	fb.vr = _fb->vr;
	start_x = start_y = 0;

	fb_real.base = _fb->base;
	fb_real.size = _fb->size;
	fb_real.hr = _fb->hr;
	fb_real.vr = _fb->vr;
}

void set_start(unsigned int _sx, unsigned int _sy)
{
	start_x = _sx;
	start_y = _sy;
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

static inline void draw_px_real(unsigned int x, unsigned int y,
				unsigned char r, unsigned char g,
				unsigned char b)
{
	struct pixelformat *p = fb_real.base;
	p += y * fb_real.hr + x;

	p->b = b;
	p->g = g;
	p->r = r;
}

inline void draw_px(unsigned int x, unsigned int y,
		    unsigned char r, unsigned char g, unsigned char b)
{
	draw_px_real(start_x + x, start_y + y, r, g, b);
}

inline void draw_px_fg(unsigned int x, unsigned int y)
{
	draw_px(x, y, color_fg.r, color_fg.g, color_fg.b);
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

inline void fill_rect_bg(unsigned int x, unsigned int y,
			 unsigned int w, unsigned int h)
{
	fill_rect(x, y, w, h, color_bg.r, color_bg.g, color_bg.b);
}

void clear_screen(void)
{
	fill_rect(0, 0, fb.hr, fb.vr, color_bg.r, color_bg.g, color_bg.b);
}
