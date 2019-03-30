#pragma once

#define PCI_VID_INTEL		0x8086
#define PCI_DID_IWM7265		0x095b
#define PCI_DID_I218V		0x15a3

#define I218V_DEV_NUM	0x19

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

extern unsigned char nic_mac[6];

void i218v_init(void);
unsigned int i218v_read_reg(unsigned short ofs);
void i218v_write_reg(unsigned short ofs, unsigned int val);
void handleReceive(void);
unsigned int switch_endian32(unsigned int nb);
void send_dhcp_request(void);
void rxinit(void);
void txinit(void);
void sendPacket(const void *p_data, unsigned short p_len);
void receive_packet(void *p_data, unsigned short *p_len);
void send_dhcp_discover(void);
int get_ip(void);
void send_test(void);
void i218v_test(void);
