#include <x86.h>
#include <pci.h>
#include <fbcon.h>

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
