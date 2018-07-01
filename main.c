#include <x86.h>
#include <intr.h>
#include <pic.h>
#include <acpi.h>
#include <efi.h>
#include <fb.h>
#include <kbc.h>
#include <fbcon.h>
#include <fs.h>
#include <hpet.h>
#include <sched.h>
#include <common.h>

struct __attribute__((packed)) platform_info {
	struct framebuffer fb;
	void *rsdp;
};

void do_taskA(void);

void start_kernel(struct EFI_SYSTEM_TABLE *_st, struct platform_info *pi,
		  void *_fs_start)
{
	/* フレームバッファ周りの初期化 */
	fb_init(&pi->fb);
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

	/* EFIの初期化 */
	efi_init(_st);

	/* ACPIの初期化 */
	acpi_init(pi->rsdp);

	/* CPU周りの初期化 */
	gdt_init();
	intr_init();

	/* 周辺ICの初期化 */
	pic_init();
	hpet_init();
	kbc_init();

	/* ファイルシステムの初期化 */
	fs_init(_fs_start);

	sched_init();

	/* CPUの割り込み有効化 */
	enable_cpu_intr();

	sched_start();

	do_taskA();

	/* haltして待つ */
	while (1)
		cpu_halt();
}

void do_taskA(void)
{
	while (1) {
		putc('A');
		sleep(100 * MS_TO_US);
	}
}
