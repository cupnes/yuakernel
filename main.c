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

/* >>> i218v.h */
#define I218V_NUM_RX_DESC	32

struct __attribute__((packed)) i218v_rx_desc {
	unsigned long long addr;
	unsigned short length;
	unsigned short checksum;
	unsigned char status;
	unsigned char errors;
	unsigned short special;
};

unsigned int i218v_read_reg(unsigned short ofs);
void i218v_write_reg(unsigned short ofs, unsigned int val);

static unsigned char i218v_rx_desc_arr[sizeof(struct i218v_rx_desc) * I218V_NUM_RX_DESC + 16];
struct i218v_rx_desc *rx_descs[I218V_NUM_RX_DESC];
static unsigned char i218v_rx_temp[8192+16];
unsigned short rx_cur;
unsigned long long i218v_reg_base;
/* <<< i218v.h */

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

	/* i218v_reg_base = pci_read_config_reg(0x00, 0x19, 0x00, 0x10); */

	/* i218v_write_reg(0x00D8, 0xffffffff); */
	/* i218v_write_reg(0x0100, 0); */
	/* i218v_write_reg(0x0400, 0); */
	/* i218v_write_reg(0x0000, i218v_read_reg(0x0000) | (1 << 26)); */

	unsigned int status_command = pci_read_config_reg(0x00, 0x19, 0x00, 0x04);
	pci_write_config_reg(0x00, 0x19, 0x00, 0x04, status_command | 0x00000004);

	/* i218v_write_reg(0x0014, 1); */
	unsigned int tmp_reg;

	i218v_write_reg(0x0014, 1);
	unsigned int eeprom_exists = 0;
	unsigned int i;
	for (i = 0; i < 1000; i++) {
		tmp_reg = i218v_read_reg(0x0014);
		eeprom_exists = tmp_reg & 0x10;
		if (eeprom_exists)
			break;
	}
	if (eeprom_exists) {
		puts("eeprom exists\r\n");

		/* haltして待つ */
		while (1)
			cpu_halt();
	} else {
		puts("eeprom not exists\r\n");

		unsigned long long bar =
			pci_read_config_reg(0x00, 0x19, 0x00, 0x10)
			& 0xfffffff0;
		puts("bar:");
		puth(bar, 8);
		puts("\r\n");

		unsigned char *mem_base_mac_8 = (unsigned char *)(bar + 0x5400);
		unsigned int *mem_base_mac_32 = (unsigned int *)(bar + 0x5400);
		unsigned char mac[6];
		if (mem_base_mac_32[0] != 0) {
			for (i = 0; i < 6; i++) {
				mac[i] = mem_base_mac_8[i];
			}
		}

		for (i = 0; i < 6; i++) {
			puth(mac[i], 2);
			putc(' ');
		}
		puts("\r\n");
	}

	struct i218v_rx_desc *descs;

	descs = (struct i218v_rx_desc *)i218v_rx_desc_arr;
	for (i = 0; i < I218V_NUM_RX_DESC; i++) {
		rx_descs[i] = (struct i218v_rx_desc *)((unsigned char *)descs + i*16);
		rx_descs[i]->addr = (unsigned long long)(unsigned char *)i218v_rx_temp;
		rx_descs[i]->status = 0;
	}

	writeCommand(REG_TXDESCLO, (unsigned int)((unsigned long long)i218v_rx_desc_arr >> 32) );
	writeCommand(REG_TXDESCHI, (unsigned int)((unsigned long long)i218v_rx_desc_arr & 0xFFFFFFFF));

	writeCommand(REG_RXDESCLO, (unsigned long long)i218v_rx_desc_arr);
	writeCommand(REG_RXDESCHI, 0);

	writeCommand(REG_RXDESCLEN, I218V_NUM_RX_DESC * 16);

	writeCommand(REG_RXDESCHEAD, 0);
	writeCommand(REG_RXDESCTAIL, I218V_NUM_RX_DESC-1);
	rx_cur = 0;
	writeCommand(REG_RCTRL, RCTL_EN| RCTL_SBP| RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_2048);

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

unsigned int i218v_read_reg(unsigned short ofs)
{
	unsigned long long addr = i218v_reg_base + ofs;
	/* putc('R'); */
	/* puth(addr, 16); */
	/* puts("\r\n"); */
	return *(volatile unsigned int *)addr;
}

void i218v_write_reg(unsigned short ofs, unsigned int val)
{
	unsigned long long addr = i218v_reg_base + ofs;
	/* putc('W'); */
	/* puth(addr, 16); */
	/* puts("\r\n"); */
	*(volatile unsigned int *)addr = val;
}

void debug_dump_regs(void)
{
	puts("cr0:");
	puth(read_cr0(), 16);
	puts("\r\n");
	puts("cr4:");
	puth(read_cr4(), 16);
	puts("\r\n");
	puts("rflags:");
	puth(read_rflags(), 16);
	puts("\r\n");
	puts("efer:");
	puth(read_msr(MSR_IA32_EFER), 16);
	puts("\r\n");
}

void debug_dump_address_translation(void)
{
	union linear_address_4lv_2mpage la;
	la.raw = 0x00000000f1300000;
	/* puth(la.raw, 16); */
	/* puts("\r\n"); */
	/* puth(la.pml4, 3); */
	/* putc(':'); */
	/* puth(la.directory_ptr, 3); */
	/* putc(':'); */
	/* puth(la.directory, 3); */
	/* putc(':'); */
	/* puth(la.offset, 6); */
	/* puts("\r\n"); */

	unsigned long long cr3 = read_cr3();
	/* puts("cr3:"); */
	/* puth(cr3, 16); */
	/* puts("\r\n"); */

	unsigned long long pml4 = cr3 & ~CR3_FLAGS_MASK;
	puts("pml4:");
	puth(pml4, 16);
	puts("\r\n");

	unsigned long long pml4e_addr = pml4 + (la.pml4 * 8);
	puts("pml4ea:");
	puth(pml4e_addr, 16);
	puts("\r\n");

	unsigned long long pml4e = *(volatile unsigned long long *)pml4e_addr;
	puts("pml4e:");
	puth(pml4e, 16);
	puts("\r\n");

	unsigned long long pdpt = pml4e & ~PML4E_FLAGS_MASK;
	puts("pdpt:");
	puth(pdpt, 16);
	puts("\r\n");

	unsigned long long pdpte_addr = pdpt + (la.directory_ptr * 8);
	puts("pdptea:");
	puth(pdpte_addr, 16);
	puts("\r\n");

	unsigned long long pdpte = *(volatile unsigned long long *)pdpte_addr;
	puts("pdpte:");
	puth(pdpte, 16);
	puts("\r\n");

	unsigned long long pd = pdpte & ~PDPTE_FLAGS_MASK;
	puts("pd:");
	puth(pd, 16);
	puts("\r\n");

	unsigned long long pde_addr = pd + (la.directory * 8);
	puts("pdea:");
	puth(pde_addr, 16);
	puts("\r\n");

	unsigned long long pde = *(volatile unsigned long long *)pde_addr;
	puts("pde:");
	puth(pde, 16);
	puts("\r\n");
}
