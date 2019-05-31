#include <x86.h>
#include <mp.h>
#include <fbcon.h>
#include <common.h>

#define DEBUG

#define	MP_PROCESSOR	0
#define	MP_BUS		1
#define	MP_IOAPIC	2
#define	MP_INTSRC	3
#define	MP_LINTSRC	4

unsigned long long mp_lapic_addr;	// address of Local APIC of BSP

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

int smp_read_mpc(struct mp_config_table *mpc)
{
	// count processing data
	unsigned long long count = sizeof(*mpc);

	// pointer to entry type stored in MP configuration table entry
	// The first byte of entry indicates entry type.
	unsigned char *mpt = ((unsigned char *)mpc) + count;

	// read signature "PCMP"
	if (strncmp(mpc->mpc_signature, MPC_SIGNATURE, 4) != 0) {
		puts("MPCTable: Bad signature[");
		putc(mpc->mpc_signature[0]);
		putc(mpc->mpc_signature[1]);
		putc(mpc->mpc_signature[2]);
		putc(mpc->mpc_signature[3]);
		puts("]\r\n");
		return 0;
	}

	// MP specification version 1.1 or 1.4
	if (mpc->mpc_spec != 0x01 && mpc->mpc_spec != 0x04) {
		puts("MPCTable: bad table version (");
		putd(mpc->mpc_spec, 3);
		puts(")\r\n");
		return 0;
	}

	// Local APIC address must exist
	if (mpc->mpc_lapic == 0) {
		puts("MPCTable: null local APIC address\r\n");
		return 0;
	}

	// Local APIC
	mp_lapic_addr = mpc->mpc_lapic;

#ifdef DEBUG
	do {
		char str[16];
		memcpy(str, mpc->mpc_oem, 8);
		str[8] = 0;

		memcpy(str, mpc->mpc_productid, 12);
		str[12] = 0;

		puts("MPCTable: Product ID: ");
		puts(str);
		puts("\r\n");
		puts("MPCTable: OEM ID: ");
		puts(str);
		puts("\r\n");
		puts("MPCTable: Local APIC at: 0x");
		puth(mpc->mpc_lapic, 8);
		puts("\r\n");
		puts("MPCTable: Base table length: ");
		putd(mpc->mpc_length, 5);
		puts("\r\n");
	} while (0);
#endif

	//
	// read entries in the table
	//
	mpt = ((unsigned char *)mpc) + count; /* start point of tables */
	while (count < mpc->mpc_length) {
		switch (*mpt) { // read type information, 
			// one entry per processor
		case MP_PROCESSOR:
		{
			puts("MP_PROCESSOR\r\n");
			/* struct mpc_config_processor *m = (struct mpc_config_processor *)mpt; */

			/* MP_processor_info(m); */
			/* mpt += sizeof(*m); */
			/* count += sizeof(*m); */
			break;
		}
		// one entry per bus
		case MP_BUS:
		{
			puts("MP_BUS");
			/* struct mpc_config_bus *m = (struct mpc_config_bus *)mpt; */

			/* MP_bus_info(m); */
			/* mpt += sizeof(*m); */
			/* count += sizeof(*m); */
			break;
		}
		// one entry per IO APIC
		case MP_IOAPIC:
		{
			puts("MP_IOAPIC");
			/* struct mpc_config_ioapic *m = (struct mpc_config_ioapic *)mpt; */

			/* MP_ioapic_info(m); */
			/* mpt += sizeof(*m); */
			/* count += sizeof(*m); */
			break;
		}
		// one entry per bus interrupt source
		case MP_INTSRC:
		{
			puts("MP_INTSRC\r\n");
			/* struct mpc_config_intsrc *m = (struct mpc_config_intsrc *)mpt; */

			/* MP_intsrc_info(m); */
			/* mpt += sizeof(*m); */
			/* count += sizeof(*m); */
			break;
		}
		// one entry per system interrupt source
		case MP_LINTSRC:
		{
			puts("MP_LINTSRC\r\n");
			/* struct mpc_config_lintsrc *m = (struct mpc_config_lintsrc *)mpt; */

			/* MP_lintsrc_info(m); */
			/* mpt += sizeof(*m); */
			/* count += sizeof(*m); */
			break;
		}
		}
	}


#ifdef DEBUG
	puts("MPCTable: success to read tables!\r\n");
#endif

	return 1;
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
