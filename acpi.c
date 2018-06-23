#include <fbcon.h>
#include <common.h>

struct __attribute__((packed)) RSDP {
	char Signature[8];
	unsigned char Checksum;
	char OEMID[6];
	unsigned char Revision;
	unsigned int RsdtAddress;
	unsigned int Length;
	unsigned long long XsdtAddress;
	unsigned char Extended_Checksum;
	unsigned char Reserved[3];
};

struct __attribute__((packed)) SDTH {
	char Signature[4];
	unsigned int Length;
	unsigned char Revision;
	unsigned char Checksum;
	char OEMID[6];
	char OEM_Table_ID[8];
	unsigned int OEM_Revision;
	unsigned int Creator_ID;
	unsigned int Creator_Revision;
};

struct __attribute__((packed)) XSDT {
	struct SDTH Header;
	struct SDTH *Entry[0];
};

struct XSDT *xsdt;
unsigned long long num_sdts;

void acpi_init(void *rsdp)
{
	xsdt = (struct XSDT *)((struct RSDP *)rsdp)->XsdtAddress;

	num_sdts = (xsdt->Header.Length - sizeof(struct SDTH))
		/ sizeof(struct SDTH *);
}

void dump_sdth_sig(struct SDTH *h)
{
	unsigned char i;
	for (i = 0; i < 4; i++)
		putc(h->Signature[i]);
}

void dump_xsdt(void)
{
	dump_sdth_sig(&xsdt->Header);
	puts("\r\n");

	puts("NUM SDTS ");
	putd(num_sdts, 2);
	puts("\r\n");

	unsigned long long i;
	for (i = 0; i < num_sdts; i++) {
		dump_sdth_sig(xsdt->Entry[i]);
		putc(' ');
	}
	puts("\r\n");
}

void *acpi_get_dev_table(char *sig)
{
	unsigned long long i;
	for (i = 0; i < num_sdts; i++) {
		if (!strncmp(sig, xsdt->Entry[i]->Signature, 4))
			return (unsigned char *)xsdt->Entry[i]
				+ sizeof(struct SDTH);
	}

	return NULL;
}

void acpi_check_dev_table(void *contents)
{
	struct SDTH *sdth =
		(struct SDTH *)((unsigned char *)contents
				- sizeof(struct SDTH));
	dump_sdth_sig(sdth);
}
