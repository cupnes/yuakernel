#include <acpi.h>

struct __attribute__((packed)) HPET_TABLE {
	unsigned int event_timer_block_id;
	struct ACPI_ADDRESS base_address;
	unsigned char hpet_number;
	unsigned short minimum_tick;
	unsigned char flags;
};

unsigned long long reg_base;

/* General Capabilities and ID Register */
#define GCIDR_ADDR	(reg_base)
#define GCIDR	(*(volatile unsigned long long *)GCIDR_ADDR)
union gcidr {
	unsigned long long raw;
	struct __attribute__((packed)) {
		unsigned long long rev_id:8;
		unsigned long long num_tim_cap:5;
		unsigned long long count_size_cap:1;
		unsigned long long _reserved:1;
		unsigned long long leg_route_cap:1;
		unsigned long long vendor_id:16;
		unsigned long long counter_clk_period:32;
	};
};

void hpet_init(void)
{
	/* HPET tableを取得 */
	struct HPET_TABLE *hpet_table =
		(struct HPET_TABLE *)acpi_get_dev_table("HPET");

	/* レジスタの先頭アドレスを取得 */
	reg_base = hpet_table->base_address.address;
}
