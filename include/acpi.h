#ifndef _ACPI_H_
#define _ACPI_H_

void acpi_init(void *rsdp);
void dump_xsdt(void);
void *acpi_get_dev_table(char *sig);
void *acpi_check_dev_table(void *contents);

#endif
