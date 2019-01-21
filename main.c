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
		unsigned int _ro_zero:2;
		unsigned int reg_addr:6;
		unsigned int func_num:3;
		unsigned int dev_num:5;
		unsigned int bus_num:8;
		unsigned int _reserved:7;
		unsigned int enable_bit:1;
	};
};
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
	union pci_config_address addr;
	addr.raw = 0;
	addr.enable_bit = 1;
	addr.dev_num = I218V_DEV_NUM;

	unsigned char raddr;
	unsigned int config_data;
	unsigned int bar0;
	unsigned long long bar;
	unsigned short command;
	unsigned long long tmp;


	addr.reg_addr = 0x04;
	io_write32(PCI_IO_CONFIG_ADDR, addr.raw);

	puth(addr.raw, 8);
	puts("\r\n");

	config_data = io_read32(PCI_IO_CONFIG_DATA);

	puth(config_data, 8);
	puts("\r\n");

	config_data |= 0x00000007;
	puth(config_data, 8);
	puts("\r\n");

	io_write32(PCI_IO_CONFIG_ADDR, addr.raw);

	io_write32(PCI_IO_CONFIG_DATA, config_data);


	addr.reg_addr = 0x10;
	io_write32(PCI_IO_CONFIG_ADDR, addr.raw);

	config_data = 0xf1300000;
	io_write32(PCI_IO_CONFIG_DATA, config_data);

	puth(config_data, 8);
	puts("\r\n");


	/* dump config registers */
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
			tmp = config_data;
			bar += tmp << 32;
		}

		puth(config_data, 8);
		if ((raddr + 4) % 8 == 0)
			puts("\r\n");
		else
			putc(' ');
	}
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
