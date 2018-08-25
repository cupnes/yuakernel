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

#define CSR_HW_IF_CONFIG_REG_NIC_READY	(0x00400000) /* PCI_OWN_SEM */
#define CSR_HW_IF_CONFIG_REG_PREPARE	(0x08000000) /* WAKE_ME */

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

	/* dump config registers */
	unsigned char raddr;
	unsigned int config_data;
	unsigned int bar0;
	unsigned long long bar;
	unsigned short command;
	for (raddr = 0x00; raddr < 0x30; raddr += 4) {
		addr.reg_addr = raddr;
		io_write32(PCI_IO_CONFIG_ADDR, addr.raw);
		config_data = io_read32(PCI_IO_CONFIG_DATA);

		if (raddr == 0x04)
			command = config_data;
		if (raddr == 0x10)
			bar0 = config_data;
		if (raddr == 0x14)
			bar = config_data;
		if (raddr == 0x18) {
			unsigned long long tmp = config_data;
			bar += tmp << 32;
		}

		puth(config_data, 8);
		if ((raddr + 4) % 8 == 0)
			puts("\r\n");
		else
			putc(' ');
	}

	/* set mem access to Command */
	/* puts("command: "); */
	/* puth(command, 4); */
	/* puts("\r\n"); */
	/* command |= 0x02; */
	/* addr.reg_addr = 0x04; */
	/* io_write32(PCI_IO_CONFIG_ADDR, addr.raw); */
	/* config_data = command; */
	/* puts("set: "); */
	/* puth(config_data, 8); */
	/* puts("\r\n"); */
	/* io_write32(PCI_IO_CONFIG_DATA, config_data); */

	/* io_write32(PCI_IO_CONFIG_ADDR, addr.raw); */
	/* config_data = io_read32(PCI_IO_CONFIG_DATA); */
	/* puth(config_data, 8); */
	/* puts("\r\n"); */

	/* dump bar */
	puts("bar0: ");
	puth(bar0, 8);
	puts("\r\n");
	/* puts("bar: "); */
	/* puth(bar, 16); */
	/* puts("\r\n"); */

	unsigned int bar_wdata =
		CSR_HW_IF_CONFIG_REG_PREPARE | CSR_HW_IF_CONFIG_REG_NIC_READY;

	/* io access to bar */
	unsigned int hicr_data = io_read32(bar0);
	puts("hicr_data: ");
	puth(hicr_data, 8);
	puts("\r\n");

	hicr_data |= CSR_HW_IF_CONFIG_REG_PREPARE;
	puts("hicr_data: ");
	puth(hicr_data, 8);
	puts("\r\n");

	io_write32(bar0, hicr_data);

	hicr_data = io_read32(bar0);
	puts("hicr_data: ");
	puth(hicr_data, 8);
	puts("\r\n");

	unsigned long long hicr_wait;
	for (hicr_wait = 0;
	     (io_read32(bar0) & CSR_HW_IF_CONFIG_REG_NIC_READY) == 0;
	     hicr_wait++);
	puts("wait: ");
	puth(hicr_wait, 16);
	puts("\r\n");

	hicr_data = io_read32(bar0);
	puts("hicr_data: ");
	puth(hicr_data, 8);
	puts("\r\n");


	/* mem access to bar */
	/* volatile unsigned long long *hw_if_config_reg = */
	/* 	(unsigned long long *)bar; */
	/* puts("HICR: "); */
	/* puth(*hw_if_config_reg, 16); */
	/* puts("\r\n"); */

	/* *hw_if_config_reg |= bar_wdata; */

	/* puts("HICR: "); */
	/* puth(*hw_if_config_reg, 16); */
	/* puts("\r\n"); */

	/* unsigned long long hicr_wait; */
	/* for (hicr_wait = 0; */
	/*      (*hw_if_config_reg & CSR_HW_IF_CONFIG_REG_NIC_READY) == 0; */
	/*      hicr_wait++); */
	/* puts("wait: "); */
	/* puth(hicr_wait, 16); */
	/* puts("\r\n"); */

	/* puts("HICR: "); */
	/* puth(*hw_if_config_reg, 16); */
	/* puts("\r\n"); */

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
