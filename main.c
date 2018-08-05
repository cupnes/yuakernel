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
/* PCI */
unsigned short PCIConfigReadWord(unsigned char bus, unsigned char slot,
				 unsigned char func, unsigned char offset);
unsigned short PCICheckVendor(unsigned char bus, unsigned char slot);
unsigned short PCICheckDevice(unsigned char bus, unsigned char slot);

void start_kernel(void *_t __attribute__((unused)), struct platform_info *pi,
		  void *_fs_start)
{
	/* フレームバッファ周りの初期化 */
	fb_init(&pi->fb);
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

	/* PCI test */
	union pci_config_address addr;
	addr.raw = 0;
	addr.enable_bit = 1;
	io_write32(PCI_IO_CONFIG_ADDR, addr.raw);
	unsigned short vendor_id = io_read32(PCI_IO_CONFIG_DATA);
	puth(vendor_id, 4);

	putc(' ');

	addr.reg_addr = 2;
	io_write32(PCI_IO_CONFIG_ADDR, addr.raw);
	volatile unsigned short dev_id = io_read32(PCI_IO_CONFIG_DATA);
	puth(dev_id, 4);

	puts("\r\n");

	if (dev_id == 0x1237)
		puts("Yes\r\n");
	else
		puts("No\r\n");

	vendor_id = PCICheckVendor(0, 0);
	puth(vendor_id, 4);

	putc(' ');

	dev_id = PCICheckDevice(0, 0);
	puth(dev_id, 4);

	puts("\r\n");

	if (dev_id == 0x1237)
		puts("Yes\r\n");
	else
		puts("No\r\n");

	while (1);

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

/* PCI */
unsigned short PCIConfigReadWord(unsigned char bus, unsigned char slot,
				 unsigned char func, unsigned char offset)
{
	unsigned int address;
	unsigned int lbus = (unsigned int)bus;
	unsigned int lslot = (unsigned int)slot;
	unsigned int lfunc = (unsigned int)func;
	unsigned short tmp = 0;

	/* コンフィギュレーションアドレスを作成 */
	address = (unsigned int)((lbus << 16) | (lslot << 11) | (lfunc << 8)
				 | (offset & 0xFC)
				 | ((unsigned int)0x80000000));

	/* アドレスの書き出し*/
	io_write32(0xCF8, address);
	/* データの読み込み */
	tmp = (unsigned short)(
		(io_read32(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
	return(tmp);
}

unsigned short PCICheckVendor(unsigned char bus, unsigned char slot)
{
	unsigned short vendor;

	vendor = PCIConfigReadWord(bus, slot, 0, 0);

	return vendor;
}

unsigned short PCICheckDevice(unsigned char bus, unsigned char slot)
{
	unsigned short vendor, device = 0xffff;
	/* 最初のコンフィギュレーションを読み込むテスト */
	/* ベンダーなし(0xFFFF)の場合、デバイスは存在しないことになる */
	if((vendor = PCIConfigReadWord(bus, slot, 0, 0)) != 0xFFFF)
	{
		device = PCIConfigReadWord(bus, slot, 0, 2);
	}
	return device;
}
