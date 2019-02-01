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
#include <network.h>

void debug_dump_address_translation(unsigned long long linear_address);

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
#define I218V_NUM_TX_DESC	8

#define REG_CTRL        0x0000
#define REG_STATUS      0x0008
#define REG_EEPROM      0x0014
#define REG_CTRL_EXT    0x0018
#define REG_IMASK       0x00D0
#define REG_RCTRL       0x0100
#define REG_RXDESCLO    0x2800
#define REG_RXDESCHI    0x2804
#define REG_RXDESCLEN   0x2808
#define REG_RXDESCHEAD  0x2810
#define REG_RXDESCTAIL  0x2818

#define I218V_REG_CTRL	0x0000
#define I218V_REG_STATUS	0x0008
#define I218V_REG_EEPROM	0x0014
#define I218V_REG_CTRL_EXT	0x0018
#define I218V_REG_IMASK	0x00D0
#define I218V_REG_RCTRL	0x0100
#define I218V_REG_RXDESCLO	0x2800
#define I218V_REG_RXDESCHI	0x2804
#define I218V_REG_RXDESCLEN	0x2808
#define I218V_REG_RXDESCHEAD	0x2810
#define I218V_REG_RXDESCTAIL	0x2818
#define I218V_REG_TCTRL	0x0400
#define I218V_REG_TXDESCLO	0x3800
#define I218V_REG_TXDESCHI	0x3804
#define I218V_REG_TXDESCLEN	0x3808
#define I218V_REG_TXDESCHEAD	0x3810
#define I218V_REG_TXDESCTAIL	0x3818
#define I218V_REG_IP	0x5840
#define I218V_REG_RDTR	0x2820 // RX Delay Timer Register
#define I218V_REG_RXDCTL	0x3828 // RX Descriptor Control
#define I218V_REG_RADV	0x282C // RX Int. Absolute Delay Timer
#define I218V_REG_RSRPD	0x2C00 // RX Small Packet Detect Interrupt
#define I218V_REG_TIPG	0x0410      // Transmit Inter Packet Gap
#define I218V_ECTRL_SLU	0x40        //set link up

#define RCTL_EN                         (1 << 1)    // Receiver Enable
#define RCTL_SBP                        (1 << 2)    // Store Bad Packets
#define RCTL_UPE                        (1 << 3)    // Unicast Promiscuous Enabled
#define RCTL_MPE                        (1 << 4)    // Multicast Promiscuous Enabled
#define RCTL_LPE                        (1 << 5)    // Long Packet Reception Enable
#define RCTL_LBM_NONE                   (0 << 6)    // No Loopback
#define RCTL_LBM_PHY                    (3 << 6)    // PHY or external SerDesc loopback
#define RTCL_RDMTS_HALF                 (0 << 8)    // Free Buffer Threshold is 1/2 of RDLEN
#define RTCL_RDMTS_QUARTER              (1 << 8)    // Free Buffer Threshold is 1/4 of RDLEN
#define RTCL_RDMTS_EIGHTH               (2 << 8)    // Free Buffer Threshold is 1/8 of RDLEN
#define RCTL_MO_36                      (0 << 12)   // Multicast Offset - bits 47:36
#define RCTL_MO_35                      (1 << 12)   // Multicast Offset - bits 46:35
#define RCTL_MO_34                      (2 << 12)   // Multicast Offset - bits 45:34
#define RCTL_MO_32                      (3 << 12)   // Multicast Offset - bits 43:32
#define RCTL_BAM                        (1 << 15)   // Broadcast Accept Mode
#define RCTL_VFE                        (1 << 18)   // VLAN Filter Enable
#define RCTL_CFIEN                      (1 << 19)   // Canonical Form Indicator Enable
#define RCTL_CFI                        (1 << 20)   // Canonical Form Indicator Bit Value
#define RCTL_DPF                        (1 << 22)   // Discard Pause Frames
#define RCTL_PMCF                       (1 << 23)   // Pass MAC Control Frames
#define RCTL_SECRC                      (1 << 26)   // Strip Ethernet CRC

// Buffer Sizes
#define RCTL_BSIZE_256                  (3 << 16)
#define RCTL_BSIZE_512                  (2 << 16)
#define RCTL_BSIZE_1024                 (1 << 16)
#define RCTL_BSIZE_2048                 (0 << 16)
#define RCTL_BSIZE_4096                 ((3 << 16) | (1 << 25))
#define RCTL_BSIZE_8192                 ((2 << 16) | (1 << 25))
#define RCTL_BSIZE_16384                ((1 << 16) | (1 << 25))

#define TSTA_DD                         (1 << 0)    // Descriptor Done
#define TSTA_EC                         (1 << 1)    // Excess Collisions
#define TSTA_LC                         (1 << 2)    // Late Collision
#define LSTA_TU                         (1 << 3)    // Transmit Underrun

	struct __attribute__((packed)) i218v_rx_desc {
		unsigned long long addr;
		unsigned short length;
		unsigned short checksum;
		unsigned char status;
		unsigned char errors;
		unsigned short special;
	};

struct __attribute__((packed)) i218v_tx_desc {
	unsigned long long addr;
	unsigned short length;
	unsigned char cso;
	unsigned char cmd;
	unsigned char status;
	unsigned char css;
	unsigned short special;
};

struct __attribute__((packed)) dhcp {
	unsigned int a[85];
	unsigned short b;
};

unsigned int i218v_read_reg(unsigned short ofs);
void i218v_write_reg(unsigned short ofs, unsigned int val);
void handleReceive(void);
unsigned int switch_endian32(unsigned int nb);
void send_dhcp_request(void);
void rxinit(void);
void txinit(void);
void send_dhcp_discover(void);
int get_ip(void);
void send_test(void);

unsigned char i218v_rx_desc_arr[sizeof(struct i218v_rx_desc) * I218V_NUM_RX_DESC + 16];
unsigned char i218v_tx_desc_arr [sizeof(struct i218v_tx_desc)*I218V_NUM_TX_DESC+16];
struct i218v_rx_desc *rx_descs[I218V_NUM_RX_DESC];
struct i218v_tx_desc *tx_descs[I218V_NUM_TX_DESC];
unsigned char i218v_rx_temp[8192+16];
unsigned short rx_cur;
unsigned short tx_cur;
unsigned long long i218v_reg_base;
unsigned char package_buf[1024];
unsigned int package_len = 0;
volatile unsigned int SEND_DHCP = 0;
unsigned int ip[4] = {0};
struct dhcp c;
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



	/* i218v test */
	i218v_reg_base = pci_read_config_reg(0x00, 0x19, 0x00, 0x10);

	unsigned int status_command = pci_read_config_reg(0x00, 0x19, 0x00, 0x04);
	pci_write_config_reg(0x00, 0x19, 0x00, 0x04, status_command | 0x00000004);

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

		while (1)
			cpu_halt();
	} else {
		puts("eeprom not exists\r\n");

		unsigned long long bar =
			pci_read_config_reg(0x00, 0x19, 0x00, 0x10)
			& 0xfffffff0;
		/* puts("bar:"); */
		/* puth(bar, 8); */
		/* puts("\r\n"); */

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

	for (i = 0; i < 0x80; i++)
		i218v_write_reg(0x5200 + i*4, 0);

	rxinit();
	txinit();

	volatile unsigned int counter = 100000;
	while (counter--);

    send_test();

	get_ip();

	puts("receiving interupt\r\n");
	while (1) {
		unsigned int status = i218v_read_reg(0xc0);
		(void)status;
		/* if (status & 0x04) { */
		/* 	// startLink(); /////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!start */
		/* } else if (status & 0x10) { */
		/* 	// good threshold */
		/* } else if (status & 0x80) { */
		/* 	/\* puts("receive!\r\n"); *\/ */
		/* 	handleReceive(); */
		/* } */
		handleReceive();
	}

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

			/* unsigned short device_id = */
			/* 	(config_data & 0xffff0000) >> 16; */

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

void debug_dump_address_translation(unsigned long long linear_address)
{
	union linear_address_4lv_2mpage la;
	la.raw = linear_address;
	puth(la.raw, 16);
	puts("\r\n");
	puth(la.pml4, 3);
	putc(':');
	puth(la.directory_ptr, 3);
	putc(':');
	puth(la.directory, 3);
	putc(':');
	puth(la.offset, 6);
	puts("\r\n");

	unsigned long long cr3 = read_cr3();
	puts("cr3:");
	puth(cr3, 16);
	puts("\r\n");

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

void handleReceive(void)
{
	/* putc('.'); */

	unsigned short old_cur;
	/* unsigned int got_packet = 0; */

	while(rx_descs[rx_cur]->status & 0x1) {
		/* got_packet = 1; */
		unsigned char *buf = (unsigned char *)rx_descs[rx_cur]->addr;
		unsigned short len = rx_descs[rx_cur]->length;

		package_len = len;
		memcpy(package_buf, buf, len);

		move_cursor(0, 0);
		clear_screen();
		unsigned char i;
		for (i = 0; i < 181; i++) {
			puth(buf[i], 2);
		}

		/* if ((buf[278]==0x63)&&(buf[279]==0x82)&&(SEND_DHCP==0)) { */
		/* 	ip[0]=buf[58]; */
		/* 	ip[1]=buf[59]; */
		/* 	ip[2]=buf[60]; */
		/* 	ip[3]=buf[61]; */

		/* 	unsigned char i; */
		/* 	for (i = 0; i < 4; i++) { */
		/* 		puth(ip[i], 2); */
		/* 		putc('.'); */
		/* 	} */
		/* 	puts("\r\n"); */

		/* 	send_dhcp_request(); */
		/* } */

		rx_descs[rx_cur]->status = 0;
		old_cur = rx_cur;
		rx_cur = (rx_cur + 1) % I218V_NUM_RX_DESC;
		i218v_write_reg(REG_RXDESCTAIL, old_cur);
	}
}

unsigned int switch_endian32(unsigned int nb)
{
	return(((nb>>24)&0xff)|
	       ((nb<<8)&0xff0000)|
	       ((nb>>8)&0xff00)|
	       ((nb<<24)&0xff000000));
}

int sendPacket(const void *p_data, unsigned short p_len)
{
	tx_descs[tx_cur]->addr = (unsigned long long)p_data;
	tx_descs[tx_cur]->length = p_len;
	tx_descs[tx_cur]->cmd = CMD_EOP | CMD_IFCS | CMD_RS | CMD_RPS;
	tx_descs[tx_cur]->status = 0;
	unsigned char old_cur = tx_cur;
	tx_cur = (tx_cur + 1) % E1000_NUM_TX_DESC;
	//printf("%x\n",tx_cur);
	i218v_write_reg(REG_TXDESCTAIL, tx_cur);
	// printf("%x\n",readCommand(REG_TXDESCTAIL));
	while(!(tx_descs[old_cur]->status & 0xff));
	return 0;
}

void send_dhcp_request(void)
{
	c.a[71]=switch_endian32(0x033604c0);
	c.a[72]=switch_endian32(0x115ab432);
	c.a[73]=switch_endian32(0x04000000);
	c.a[74]=switch_endian32(0x00370a01);
	c.a[75]=switch_endian32(0x1c02030f);
	c.a[76]=switch_endian32(0x060c2829);
	c.a[77]=switch_endian32(0x2aff0000);
	c.a[73]=c.a[73]|switch_endian32((ip[0]<<16)&0x00ff0000)|switch_endian32((ip[1]<<8)&0x0000ff00)|switch_endian32(ip[2]&0x000000ff);
	c.a[74]=c.a[74]|switch_endian32(ip[3]<<24);
	c.a[73]=c.a[73]|switch_endian32(0x04000000);
	sendPacket(&c,sizeof(c));
	SEND_DHCP=1;
}

void rxinit(void)
{
	struct i218v_rx_desc *descs;

	descs = (struct i218v_rx_desc *)i218v_rx_desc_arr;
	int i;
	for (i = 0; i < I218V_NUM_RX_DESC; i++) {
		rx_descs[i] = (struct i218v_rx_desc *)((unsigned char *)descs + i*16);
		rx_descs[i]->addr = (unsigned long long)(unsigned char *)i218v_rx_temp;
		rx_descs[i]->status = 0;
	}

	i218v_write_reg(I218V_REG_TXDESCLO,
			(unsigned int)((unsigned long long)i218v_rx_desc_arr >> 32));
	i218v_write_reg(I218V_REG_TXDESCHI,
			(unsigned int)((unsigned long long)i218v_rx_desc_arr & 0xffffffff));

	i218v_write_reg(I218V_REG_RXDESCLO, (unsigned long long)i218v_rx_desc_arr);
	i218v_write_reg(I218V_REG_RXDESCHI, 0);

	i218v_write_reg(I218V_REG_RXDESCLEN, I218V_NUM_RX_DESC * 16);

	i218v_write_reg(I218V_REG_RXDESCHEAD, 0);
	i218v_write_reg(I218V_REG_RXDESCTAIL, I218V_NUM_RX_DESC - 1);
	rx_cur = 0;
	i218v_write_reg(I218V_REG_RCTRL,
			RCTL_EN | RCTL_SBP | RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE
			| RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC  | RCTL_BSIZE_2048);

}

void txinit(void)
{
	int i ;
	struct i218v_tx_desc *descs;
	// Allocate buffer for receive descriptors. For simplicity, in my case khmalloc returns a virtual address that is identical to it physical mapped address.
	// In your case you should handle virtual and physical addresses as the addresses passed to the NIC should be physical ones

	descs = (struct i218v_tx_desc *)i218v_tx_desc_arr;
	for (i = 0; i < I218V_NUM_TX_DESC; i++) {
		tx_descs[i] = (struct i218v_tx_desc *)((unsigned char*)descs + i*16);
		tx_descs[i]->addr = 0;
		tx_descs[i]->cmd = 0;
		tx_descs[i]->status = TSTA_DD;
	}

	i218v_write_reg(REG_TXDESCHI, (unsigned int)((unsigned long long)i218v_tx_desc_arr >> 32));
	i218v_write_reg(REG_TXDESCLO, (unsigned int)((unsigned long long)i218v_tx_desc_arr & 0xFFFFFFFF));


	//now setup total length of descriptors
	i218v_write_reg(REG_TXDESCLEN, I218V_NUM_TX_DESC * 16);


	//setup numbers
	i218v_write_reg( REG_TXDESCHEAD, 0);
	i218v_write_reg( REG_TXDESCTAIL, 0);
	tx_cur = 0;
	i218v_write_reg(REG_TCTRL,  TCTL_EN
			| TCTL_PSP
			| (15 << TCTL_CT_SHIFT)
			| (64 << TCTL_COLD_SHIFT)
			| TCTL_RTLC);

	i218v_write_reg(REG_TCTRL, 0x3003F0FA); //0b0110000000000111111000011111010);
	i218v_write_reg(REG_TIPG,  0x0060200A);
}

void send_dhcp_discover(void)
{
	//printf("something went wrong");
	c.a[0]=switch_endian32(0xffffffff);
	c.a[1]=switch_endian32(0xffff5254);
	c.a[2]=switch_endian32(0x00123456);
	c.a[3]=switch_endian32(0x08004510);
	c.a[4]=switch_endian32(0x01480000);
	c.a[5]=switch_endian32(0x00001011);
	c.a[6]=switch_endian32(0xa9960000);
	c.a[7]=switch_endian32(0x0000ffff);
	c.a[8]=switch_endian32(0xffff0044);
	c.a[9]=switch_endian32(0x00430134);
	c.a[10]=switch_endian32(0x1c6d0101);
	c.a[11]=switch_endian32(0x0600fa5f);
	c.a[12]=switch_endian32(0xc0130008);
	c.a[13]=switch_endian32(0x00000000);
	c.a[14]=switch_endian32(0x00000000);
	c.a[15]=switch_endian32(0x00000000);
	c.a[16]=switch_endian32(0x00000000);
	c.a[17]=switch_endian32(0x00005254);
	c.a[18]=switch_endian32(0x00123456);
	c.a[19]=switch_endian32(0x00000000);
	c.a[20]=switch_endian32(0x00000000);
	c.a[21]=switch_endian32(0x00000000);
	c.a[22]=switch_endian32(0x00000000);
	c.a[23]=switch_endian32(0x00000000);
	c.a[24]=switch_endian32(0x00000000);
	c.a[25]=switch_endian32(0x00000000);
	c.a[26]=switch_endian32(0x00000000);
	c.a[27]=switch_endian32(0x00000000);
	c.a[28]=switch_endian32(0x00000000);
	c.a[29]=switch_endian32(0x00000000);
	c.a[30]=switch_endian32(0x00000000);
	c.a[31]=switch_endian32(0x00000000);
	c.a[32]=switch_endian32(0x00000000);
	c.a[33]=switch_endian32(0x00000000);
	c.a[34]=switch_endian32(0x00000000);
	c.a[35]=switch_endian32(0x00000000);
	c.a[36]=switch_endian32(0x00000000);
	c.a[37]=switch_endian32(0x00000000);
	c.a[38]=switch_endian32(0x00000000);
	c.a[39]=switch_endian32(0x00000000);
	c.a[40]=switch_endian32(0x00000000);
	c.a[41]=switch_endian32(0x00000000);
	c.a[42]=switch_endian32(0x00000000);
	c.a[43]=switch_endian32(0x00000000);
	c.a[44]=switch_endian32(0x00000000);
	c.a[45]=switch_endian32(0x00000000);
	c.a[46]=switch_endian32(0x00000000);
	c.a[47]=switch_endian32(0x00000000);
	c.a[48]=switch_endian32(0x00000000);
	c.a[49]=switch_endian32(0x00000000);
	c.a[50]=switch_endian32(0x00000000);
	c.a[51]=switch_endian32(0x00000000);
	c.a[52]=switch_endian32(0x00000000);
	c.a[53]=switch_endian32(0x00000000);
	c.a[54]=switch_endian32(0x00000000);
	c.a[55]=switch_endian32(0x00000000);
	c.a[56]=switch_endian32(0x00000000);
	c.a[57]=switch_endian32(0x00000000);
	c.a[58]=switch_endian32(0x00000000);
	c.a[59]=switch_endian32(0x00000000);
	c.a[60]=switch_endian32(0x00000000);
	c.a[61]=switch_endian32(0x00000000);
	c.a[62]=switch_endian32(0x00000000);
	c.a[63]=switch_endian32(0x00000000);
	c.a[64]=switch_endian32(0x00000000);
	c.a[65]=switch_endian32(0x00000000);
	c.a[66]=switch_endian32(0x00000000);
	c.a[67]=switch_endian32(0x00000000);
	c.a[68]=switch_endian32(0x00000000);
	c.a[69]=switch_endian32(0x00006382);
	c.a[70]=switch_endian32(0x53633501);
	c.a[71]=switch_endian32(0x0132040a);
	c.a[72]=switch_endian32(0xc1b76337);
	c.a[73]=switch_endian32(0x0a011c02);
	c.a[74]=switch_endian32(0x030f060c);
	c.a[75]=switch_endian32(0x28292aff);
	c.a[76]=switch_endian32(0x00000000);
	c.a[77]=switch_endian32(0x00000000);
	c.a[78]=switch_endian32(0x00000000);
	c.a[79]=switch_endian32(0x00000000);
	c.a[80]=switch_endian32(0x00000000);
	c.a[81]=switch_endian32(0x00000000);
	c.a[82]=switch_endian32(0x00000000);
	c.a[83]=switch_endian32(0x00000000);
	c.a[84]=switch_endian32(0x00000000);
	c.b=0;
	//a[85]=switch_endian32(0x0000);
	sendPacket(&c,sizeof(c));
	//printf("dhcp package send complete\n");
}

int get_ip(void)
{
	SEND_DHCP=0;
	send_dhcp_discover();
	/* sti(); */
	/* uint32_t ms = system_time.count_ms; */
	/* while (SEND_DHCP == 0) { */
	/* if (system_time.count_ms - ms > 1000) */
	/* 	return -1; */
	/* } */
	return 0;
}

#define SEND_TEST_WAIT	1000000
void send_test(void)
{
    unsigned int data = 0xbeefcafe;
    volatile unsigned int c;
    while (1) {
        sendPacket(&data, sizeof(data));
        for (c = 0; c < SEND_TEST_WAIT; c++);
    }
}
