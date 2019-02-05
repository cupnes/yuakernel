#include <x86.h>
#include <pci.h>
#include <network.h>
#include <nic.h>
#include <fb.h>
#include <fbcon.h>
#include <common.h>

unsigned long long i218v_reg_base;

unsigned char i218v_rx_desc_arr[sizeof(struct i218v_rx_desc) * I218V_NUM_RX_DESC + 16];
unsigned char i218v_tx_desc_arr [sizeof(struct i218v_tx_desc)*I218V_NUM_TX_DESC+16];
struct i218v_rx_desc *rx_descs[I218V_NUM_RX_DESC];
struct i218v_tx_desc *tx_descs[I218V_NUM_TX_DESC];
unsigned char i218v_rx_temp[8192+16];
unsigned short rx_cur;
unsigned short tx_cur;
unsigned char package_buf[1024];
unsigned int package_len = 0;
volatile unsigned int SEND_DHCP = 0;
unsigned int ip[4] = {0};
struct dhcp c;

void i218v_init(void)
{
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

		/* for (i = 0; i < 6; i++) { */
		/* 	puth(mac[i], 2); */
		/* 	putc(' '); */
		/* } */
		/* puts("\r\n"); */
	}

	for (i = 0; i < 0x80; i++)
		i218v_write_reg(0x5200 + i*4, 0);

	rxinit();
	txinit();
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

	while (rx_descs[rx_cur]->status & 0x1) {
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

void sendPacket(const void *p_data, unsigned short p_len)
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
}

void receive_packet(void *p_data, unsigned short *p_len)
{
    *p_len = 0;

	if (rx_descs[rx_cur]->status & 0x1) {
		unsigned char *buf = (unsigned char *)rx_descs[rx_cur]->addr;
		unsigned short len = rx_descs[rx_cur]->length;
        unsigned short old_cur;

		*p_len = len;
		memcpy(p_data, buf, len);

		rx_descs[rx_cur]->status = 0;
		old_cur = rx_cur;
		rx_cur = (rx_cur + 1) % I218V_NUM_RX_DESC;
		i218v_write_reg(REG_RXDESCTAIL, old_cur);
	}
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

void i218v_test(void)
{
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
}
