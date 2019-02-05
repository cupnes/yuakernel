#pragma once

#define PCI_MAX_DEV_NUM		32

#define PCI_IO_CONFIG_ADDR	0x0cf8
#define PCI_IO_CONFIG_DATA	0x0cfc

#define CSR_HW_IF_CONFIG_REG_NIC_READY	(0x00400000) /* PCI_OWN_SEM */
#define CSR_HW_IF_CONFIG_REG_PREPARE	(0x08000000) /* WAKE_ME */

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
