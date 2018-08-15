#include <x86.h>
#include <intr.h>
#include <pic.h>
#include <acpi.h>
#include <fb.h>
#include <kbc.h>
#include <fbcon.h>
#include <fs.h>
#include <hpet.h>
#include <sched.h>
#include <common.h>
#include <cover.h>

struct __attribute__((packed)) platform_info {
	struct framebuffer fb;
	void *rsdp;
};

#define PCI_IO_CONFIG_ADDR	0x0cf8
#define PCI_IO_CONFIG_DATA	0x0cfc

#define PCI_VID_INTEL		0x8086
#define PCI_DID_IWM7265		0x095b

union pci_config_address {
	unsigned int raw;
	struct __attribute__((packed)) {
		unsigned int _ro_zero:2;
		unsigned int reg_addr:6;
		unsigned int func_num:3;
		unsigned int dev_num:5;
		unsigned int bus_num:8;
		unsigned int _reserved:7;
		unsigned int enable_bit:1;
	};
};

void do_taskA(void);

void start_kernel(void *_t __attribute__((unused)), struct platform_info *pi,
		  void *_fs_start)
{
	/* フレームバッファ周りの初期化 */
	fb_init(&pi->fb);
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

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




	/* PCI test */
	union pci_config_address addr;
	addr.raw = 0;
	addr.enable_bit = 1;
	addr.bus_num = 4;
	addr.reg_addr = 0x10;
	io_write32(PCI_IO_CONFIG_ADDR, addr.raw);
	unsigned int bar0 = io_read32(PCI_IO_CONFIG_DATA);

	addr.reg_addr = 0x14;
	io_write32(PCI_IO_CONFIG_ADDR, addr.raw);
	unsigned int bar1 = io_read32(PCI_IO_CONFIG_DATA);

	unsigned long long bar = (bar1 << 31) | (bar0 & 0xfffffff0);
	puth(bar, 16);

	while (1);



	/* /\* スケジューラの初期化 *\/ */
	/* sched_init(); */

	/* CPUの割り込み有効化 */
	enable_cpu_intr();

	/* /\* スケジューラの開始 *\/ */
	/* sched_start(); */

	/* /\* タスクAの開始 *\/ */
	/* do_taskA(); */

	/* 表紙アプリ開始 */
	cover_main();

	/* haltして待つ */
	while (1)
		cpu_halt();
}

void do_taskA(void)
{
	while (1) {
		putc('A');
		volatile unsigned long long wait = 10000000;
		while (wait--);
	}
}
