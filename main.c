#include <x86.h>
#include <intr.h>
#include <pic.h>
#include <acpi.h>
#include <syscall.h>
#include <fb.h>
#include <kbc.h>
#include <fbcon.h>
#include <fs.h>
#include <hpet.h>
#include <sched.h>
#include <common.h>
#include <cover.h>
#include <kana_shell.h>
#include <proc.h>

/* >>> pci.h */
#define PCI_MAX_DEV_NUM		32

#define PCI_IO_CONFIG_ADDR	0x0cf8
#define PCI_IO_CONFIG_DATA	0x0cfc

#define PCI_VID_INTEL		0x8086
#define PCI_DID_IWM7265		0x095b
#define PCI_DID_I218V		0x15a3

#define CSR_HW_IF_CONFIG_REG_NIC_READY	(0x00400000) /* PCI_OWN_SEM */
#define CSR_HW_IF_CONFIG_REG_PREPARE	(0x08000000) /* WAKE_ME */

#define I218V_DEV_NUM	0x19

union pci_config_address {
	unsigned int raw;
	struct __attribute__((packed)) {
		unsigned int reg_addr:8;
		unsigned int func_num:3;
		unsigned int dev_num:5;
		unsigned int bus_num:8;
		unsigned int _reserved:7;
		unsigned int enable_bit:1;
	};
};

unsigned int pci_read_config_reg(unsigned char bus, unsigned char dev,
				 unsigned char func, unsigned char reg);
void pci_write_config_reg(unsigned char bus, unsigned char dev,
			  unsigned char func, unsigned char reg,
			  unsigned int val);
void pci_put_ids(unsigned char bus, unsigned char dev, unsigned char func,
		 unsigned short vid, unsigned short did);
void pci_dump_config_reg(unsigned char bus, unsigned char dev, unsigned char func);
void pci_scan_bus(unsigned char bus);
/* <<< pci.h */

struct __attribute__((packed)) platform_info {
	struct framebuffer fb;
	void *rsdp;
};

#define INIT_APP	"ksh"

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

	/* システムコールの初期化 */
	syscall_init();

	/* ファイルシステムの初期化 */
	fs_init(_fs_start);

	/* スケジューラの初期化 */
	sched_init();



	/* PCI test */
	/* pci_scan_bus(0); */
	/* pci_dump_config_reg(0x00, 0x19, 0x00); */
	unsigned int bar = pci_read_config_reg(0x00, 0x19, 0x00, 0x10);
	puth(bar, 8);
	puts("\r\n");

	/* haltして待つ */
	while (1)
		cpu_halt();



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

unsigned int pci_read_config_reg(unsigned char bus, unsigned char dev,
				 unsigned char func, unsigned char reg)
{
	union pci_config_address addr;
	addr.raw = 0;
	addr.enable_bit = 1;
	addr.bus_num = bus;
	addr.dev_num = dev;
	addr.func_num = func;
	addr.reg_addr = reg;
	io_write32(PCI_IO_CONFIG_ADDR, addr.raw);
	return io_read32(PCI_IO_CONFIG_DATA);
	/* unsigned int data = io_read32(PCI_IO_CONFIG_DATA); */
	/* addr.enable_bit = 0; */
	/* io_write32(PCI_IO_CONFIG_ADDR, addr.raw); */
	/* return data; */
}

void pci_write_config_reg(unsigned char bus, unsigned char dev,
			  unsigned char func, unsigned char reg,
			  unsigned int val)
{
	union pci_config_address addr;
	addr.raw = 0;
	addr.enable_bit = 1;
	addr.bus_num = bus;
	addr.dev_num = dev;
	addr.func_num = func;
	addr.reg_addr = reg;
	io_write32(PCI_IO_CONFIG_ADDR, addr.raw);
	io_write32(PCI_IO_CONFIG_DATA, val);
	/* addr.enable_bit = 0; */
	/* io_write32(PCI_IO_CONFIG_ADDR, addr.raw); */
}

void pci_put_ids(unsigned char bus, unsigned char dev, unsigned char func,
                 unsigned short vid, unsigned short did)
{
	puth(bus, 2);
	putc(',');
	puth(dev, 2);
	putc(',');
	puth(func, 1);
	putc(':');
	puth(vid, 4);
	putc(',');
	puth(did, 4);
	puts("\r\n");
}

void pci_dump_config_reg(unsigned char bus, unsigned char dev, unsigned char func)
{
	unsigned char reg;
	for (reg = 0x00; reg < 0x30; reg += 0x04) {
		unsigned int config_data =
			pci_read_config_reg(bus, dev, func, reg);
		puth(config_data, 8);
		if ((reg + 4) % 8 == 0)
			puts("\r\n");
		else
			putc(' ');
	}
	puts("\r\n");
}

void pci_scan_bus(unsigned char bus)
{
	unsigned char dev;
	for (dev = 0x00; dev < PCI_MAX_DEV_NUM; dev++) {
		unsigned int config_data =
			pci_read_config_reg(bus, dev, 0, 0x0c);
		unsigned char tmp = (config_data & 0x00ff0000) >> 16;
		unsigned char header_type = tmp & 0x7f;
		if (header_type > 1)
			continue;

		unsigned char has_multi_func = tmp & 0x80;
		if ((dev == 0x1c) || (dev == 0x1f)) {
			puth(tmp, 2);
			putc(',');
			puth(has_multi_func, 2);
			puts("\r\n");
		}
		unsigned char func;
		for (func = 0; func < ((has_multi_func) ? 8 : 1); func++) {
			config_data = pci_read_config_reg(bus, dev, func, 0);
			unsigned short vendor_id = config_data & 0x0000ffff;
			if (vendor_id == 0xffff)
				continue;

			unsigned short device_id =
				(config_data & 0xffff0000) >> 16;

			/* pci_put_ids(bus, dev, func, vendor_id, device_id); */

			pci_write_config_reg(bus, dev, func, 0x04, 0x00000007);
			pci_dump_config_reg(bus, dev, func);

			return;
		}
	}
}
