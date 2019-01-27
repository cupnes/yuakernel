#ifndef _X86_H_
#define _X86_H_

#define SS_KERNEL_CODE	0x0008
#define SS_KERNEL_DATA	0x0010

#define MAX_INTR_NO	256

#define MSR_IA32_EFER	0xc0000080

#define CR3_FLAGS_MASK	0x0000000000000fff
#define PML4E_FLAGS_MASK	0x0000000000000fff
#define PDPTE_FLAGS_MASK	0x0000000000000fff

struct __attribute__((packed)) interrupt_descriptor {
	unsigned short offset_00_15;
	unsigned short segment_selector;
	unsigned short ist	: 3;
	unsigned short _zero1	: 5;
	unsigned short type	: 4;
	unsigned short _zero2	: 1;
	unsigned short dpl	: 2;
	unsigned short p	: 1;
	unsigned short offset_31_16;
	unsigned int   offset_63_32;
	unsigned int   _reserved;
};

union linear_address_4lv_2mpage {
	unsigned long long raw;
	struct __attribute__((packed)) {
		unsigned long long offset:21;
		unsigned long long directory:9;
		unsigned long long directory_ptr:9;
		unsigned long long pml4:9;
		unsigned long long _ign:16;
	};
};

void enable_cpu_intr(void);
void cpu_halt(void);
unsigned long long read_cr0(void);
unsigned long long read_cr3(void);
unsigned long long read_cr4(void);
unsigned long long read_rflags(void);
unsigned long long read_msr(unsigned int msr);
unsigned char io_read(unsigned short addr);
void io_write(unsigned short addr, unsigned char value);
unsigned short io_read16(unsigned short addr);
void io_write16(unsigned short addr, unsigned short value);
unsigned int io_read32(unsigned short addr);
void io_write32(unsigned short addr, unsigned int value);
void gdt_init(void);

#endif
