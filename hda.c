#include <x86.h>
#include <fbcon.h>
#include <pci.h>

unsigned long long hda_base;

#define GCAP	(*(volatile unsigned short *)hda_base)

void hda_init(
	unsigned char bus_num, unsigned char dev_num, unsigned char func_num)
{
	puts("## B: hda_init()\r\n");

	puth(bus_num, 2);
	putc(':');
	puth(dev_num, 2);
	putc('.');
	puth(func_num, 2);
	puts("\r\n");

	/* PCIコンフィグレーション空間からBARを取得 */
	unsigned int bar = get_pci_conf_reg(
		bus_num, dev_num, func_num, PCI_CONF_BAR);
	puts("bar=");
	puth(bar, 8);
	puts("\r\n");

	/* メモリ空間用ベースアドレス(32ビット)を返す */
	hda_base = bar & PCI_BAR_MASK_MEM_ADDR;
	puts("base=");
	puth(hda_base, 16);
	puts("\r\n");

	puts("GCAP=");
	puth(GCAP, 4);
	puts("\r\n");

	puts("## E: hda_init()\r\n");
}
