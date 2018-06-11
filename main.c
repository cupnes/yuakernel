#include <efi.h>
#include <x86.h>
#include <intr.h>
#include <pic.h>
#include <fb.h>
#include <kbc.h>
#include <fbcon.h>
#include <fs.h>
#include <common.h>
#include <iv.h>
#include <acpi.h>
#include <hpet.h>

#define SEC_TO_US	1000000

/* begin: poiboot.h */
struct platform_info {
	struct framebuffer fb;
	void *rsdp;
};
/* end: poiboot.h */

void handler(void);
void handler2(void);

unsigned char counter = 0;

void start_kernel(struct EFI_SYSTEM_TABLE *_st __attribute__ ((unused)),
		  struct platform_info *pi, void *_fs_start)
{
	disable_cpu_intr();

	/* フレームバッファ周りの初期化 */
	fb_init(&pi->fb);
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);

	/* CPU周りの初期化 */
	gdt_init();
	intr_init();

	/* 周辺ICの初期化 */
	pic_init();
	kbc_init();

	/* ファイルシステムの初期化 */
	if (_fs_start)
		fs_init(_fs_start);

	acpi_init(pi->rsdp);

	hpet_init();

	/* 画像ビューアの初期化 */
	iv_init();

	ptimer_setup(3 * SEC_TO_US, handler);
	ptimer_start();

	/* CPUの割り込み有効化 */
	enable_cpu_intr();

	/* haltして待つ */
	while (1)
		cpu_halt();
}

void handler(void)
{
	if (iv_idx < iv_num_files - 1)
		view(++iv_idx);
}
