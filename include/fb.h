#pragma once

#include <fs.h>

struct __attribute__((packed)) pixelformat {
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char _reserved;
};

struct __attribute__((packed)) framebuffer {
	struct pixelformat *base;
	unsigned long long size;
	unsigned int hr;
	unsigned int vr;
};

struct __attribute__((packed)) image {
	unsigned int width;
	unsigned int height;
	struct pixelformat data[0];
};

extern struct framebuffer fb;

void fb_init(struct framebuffer *_fb);
void set_fg(unsigned char r, unsigned char g, unsigned char b);
void set_bg(unsigned char r, unsigned char g, unsigned char b);
void get_px(unsigned int x, unsigned int y, struct pixelformat *val);
void draw_px(unsigned int x, unsigned int y,
	     unsigned char r, unsigned char g, unsigned char b);
void draw_px_fg(unsigned int x, unsigned int y);
void draw_px_bg(unsigned int x, unsigned int y);
void draw_fg(struct file *img);
void draw_bg(struct file *img);
void draw_image(struct image *img, unsigned int px, unsigned int py);
void fill_rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h,
	       unsigned char r, unsigned char g, unsigned char b);
void clear_screen(void);
