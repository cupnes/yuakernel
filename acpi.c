#include <acpi.h>
#include <common.h>

struct XSDT {
	struct SDTH Header;
	void *Entry[0];
};

struct RSDP {
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

unsigned long long *acpi_sdt_addrs;
unsigned long long num_acpi_sdt_addrs;

void acpi_init(void *_rsdp)
{
	struct RSDP *rsdp = _rsdp;
	struct SDTH *xsdt = (struct SDTH *)rsdp->XsdtAddress;

	num_acpi_sdt_addrs = (xsdt->Length - sizeof(struct SDTH))
		/ sizeof(unsigned long long);
	acpi_sdt_addrs = (unsigned long long *)(xsdt + 1);
}

struct SDTH *acpi_find_sdt(char *signature)
{
	unsigned char i;

	unsigned long long *entry_ptr = acpi_sdt_addrs;
	for (i = 0; i < num_acpi_sdt_addrs; i++, entry_ptr++) {
		struct SDTH *entry = (struct SDTH *)*entry_ptr;
		if (!strncmp(entry->Signature, signature, 4))
			return entry;
	}

	return NULL;
}
