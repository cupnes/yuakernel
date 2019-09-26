#include <x86.h>
#include <mp.h>
#include <intr.h>
#include <pic.h>
#include <acpi.h>
#include <serial.h>
#include <syscall.h>
#include <fb.h>
#include <kbc.h>
#include <fbcon.h>
#include <fs.h>
#include <hpet.h>
#include <sched.h>
#include <common.h>
#include <pci.h>
#include <nic.h>
#include <cmos.h>

/* #define RUN_QEMU */
/* #define DEBUG_DUMP_PACKETS */

void debug_dump_address_translation(unsigned long long linear_address);

struct __attribute__((packed)) platform_info {
	struct framebuffer fb;
	void *rsdp;
	void *fs_start;
	unsigned int nproc;
};

#define INIT_APP	"init"

struct file *ap_task[NUM_AP] = { NULL };

void start_kernel(void *_t __attribute__((unused)), struct platform_info *pi,
		  unsigned long long pnum)
{
	if (pnum) {
		/* CPU周りの初期化 */
		gdt_init();
		intr_init();

		/* システムコールの初期化 */
		syscall_init();

		while (1) {
			while (!ap_task[pnum - 1]);
			/* putc('B'); */
			/* puth(pnum, 1); */
			/* puts("\r\n"); */
			exec(ap_task[pnum - 1]);
			/* putc('E'); */
			/* puth(pnum, 1); */
			/* puts("\r\n"); */
			ap_task[pnum - 1] = NULL;
		}
	}

	/* フレームバッファ周りの初期化 */
	fb_init(&pi->fb);
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	/* clear_screen(); */

	/* ACPIの初期化 */
	acpi_init(pi->rsdp);

	/* CPU周りの初期化 */
	gdt_init();
	intr_init();

	/* 周辺ICの初期化 */
	ser_init();
	pic_init();
	hpet_init();
	kbc_init();
#ifndef RUN_QEMU
	nic_init(I218V_BUS_NUM, I218V_DEV_NUM, I218V_FUNC_NUM);
#else
	nic_init(QEMU_NIC_BUS_NUM, QEMU_NIC_DEV_NUM, QEMU_NIC_FUNC_NUM);
#endif

#ifdef DEBUG_DUMP_PACKETS
	/* 受信したフレームをダンプし続ける */
	while (1) {
		if (dump_packet_ser() > 0)
			puts("\r\n");
	}
#endif

	/* システムコールの初期化 */
	syscall_init();

	/* ファイルシステムの初期化 */
	fs_init(pi->fs_start);

	/* スケジューラの初期化 */
	sched_init();

	/* CPUの割り込み有効化 */
	enable_cpu_intr();

	/* スケジューラの開始 */
	sched_start();

	/* initアプリ起動 */
	exec(open(INIT_APP));

	/* haltして待つ */
	while (1)
		cpu_halt();
}
