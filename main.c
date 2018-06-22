#include <x86.h>
#include <intr.h>
#include <pic.h>
#include <fb.h>
#include <kbc.h>
#include <fbcon.h>
#include <fs.h>
#include <common.h>
#include <iv.h>

struct platform_info {
	struct framebuffer fb;
	void *rsdp;
};

void start_kernel(void *_t __attribute__ ((unused)), struct platform_info *pi,
		  void *_fs_start)
{
	/* フレームバッファ周りの初期化 */
	fb_init(&pi->fb);
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

	/* RSDPのシグネチャを表示 */
	char *s = (char *)pi->rsdp;
	putc(*s++);	/* 'R' */
	putc(*s++);	/* 'S' */
	putc(*s++);	/* 'D' */
	putc(*s++);	/* ' ' */
	putc(*s++);	/* 'P' */
	putc(*s++);	/* 'T' */
	putc(*s++);	/* 'R' */
	putc(*s);	/* ' ' */
	while (1);

	/* CPU周りの初期化 */
	gdt_init();
	intr_init();

	/* 周辺ICの初期化 */
	pic_init();
	kbc_init();

	/* ファイルシステムの初期化 */
	fs_init(_fs_start);

	/* 画像ビューアの初期化 */
	iv_init();

	/* CPUの割り込み有効化 */
	enable_cpu_intr();

	/* haltして待つ */
	while (1)
		cpu_halt();
}
