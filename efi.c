#include <efi.h>

struct EFI_SYSTEM_TABLE *efi_st;

void efi_init(struct EFI_SYSTEM_TABLE *st)
{
	efi_st = st;
}
