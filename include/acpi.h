#ifndef _ACPI_H_
#define _ACPI_H_

struct __attribute__((packed)) ACPI_ADDRESS {
	unsigned char address_space_id;
	unsigned char register_bit_width;
	unsigned char register_bit_offset;
	unsigned char _reserved;
	unsigned long long address;
};

void acpi_init(void *rsdp);
void dump_xsdt(void);
void *acpi_get_dev_table(char *sig);
void *acpi_check_dev_table(void *contents);

#endif
