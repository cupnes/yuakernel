#include <x86.h>
#include <mp.h>
#include <fbcon.h>
#include <common.h>

struct mp_floating *mp_find_config(
	unsigned long long base, unsigned long long length)
{
	unsigned int *basep = (unsigned int *)base;
	struct mp_floating *mpf;

#ifdef DEBUG
	puts("find MP pointer table: ");
	puth((unsigned long long)basep, 16);
	putc('-');
	puth((unsigned long long)basep + length, 16);
	puts("\r\n");
#endif

	while (length > 0) {
		mpf = (struct mp_floating *)basep;

		if ((*basep == SMP_MAGIC_IDENT) &&
		    (mpf->mpf_length == 1) && // this should be 1
		    ((mpf->mpf_specification == 1) || (mpf->mpf_specification == 4))) {
#ifdef DEBUG
			puts("Reserve MP configuration table ");
			puth(mpf->mpf_physptr, 8);
			puts("\r\n");
#endif

			return mpf;

		}

		basep += 4;
		length -= 16;
	}

#ifdef DEBUG
	puts("MP configuration table not found.\r\n");
#endif

	return NULL;
}

void mp_init(void)
{
	struct mp_floating *mpf;
	if ((mpf = mp_find_config(0, 0x400))) {
		puts("found at 0-0x400\r\n");
	} else if ((mpf = mp_find_config(639*0x400, 0x400))) {
		puts("found at 639*0x400-0x400\r\n");
	} else if ((mpf = mp_find_config(0xf0000, 0x10000))) {
		/* seven実機ではここで見つかった */
		puts("found at 0xf0000, 0x10000\r\n");
	} else {
		puts("mpf was not found...\r\n");
	}

	if (mpf->mpf_feature1 == 0 && mpf->mpf_physptr != 0) {
		puts("read MP configure at 0x");
		puth(mpf->mpf_physptr, 8);
		puts("\r\n");

		/* if (smp_read_mpc(mpf->mpf_physptr) == 0) { */
		/* 	puts("BIOS bus, MP table error detected!!!\n"); */
		/* } */
	} else if (mpf->mpf_feature1 != 0) {
		puts("Default MP configurationg processing is");
		puts(" not implemented yet!!!\r\n");
	} else {
		puts("SMP structure ERROR\r\n");
	}

	/* haltして待つ */
	while (1)
		cpu_halt();
}
