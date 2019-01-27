#include <x86.h>

/* GDTの定義 */
const unsigned long long gdt[] = {
	0x0000000000000000,	/* NULL descriptor */
	0x00af9a000000ffff,	/* base=0, limit=4GB, mode=code(r-x),kernel */
	0x00cf93000000ffff	/* base=0, limit=4GB, mode=data(rw-),kernel */
};
unsigned long long gdtr[2];

inline void enable_cpu_intr(void)
{
	asm volatile ("sti");
}

inline void cpu_halt(void)
{
	asm volatile ("hlt");
}

__inline unsigned long long read_cr0(void)
{
	unsigned long long value;
	asm volatile ("mov %%cr0, %[value]": [value]"=a"(value):);
	return value;
}

__inline unsigned long long read_cr4(void)
{
	unsigned long long value;
	asm volatile ("mov %%cr4, %[value]": [value]"=a"(value):);
	return value;
}

__inline unsigned long long read_rflags(void)
{
	unsigned long long value;
	asm volatile ("pushfq\n"
		      "pop %[value]\n": [value]"=a"(value):);
	return value;
}

__inline unsigned long long read_msr(unsigned int msr)
{
	unsigned int low, high;
	asm volatile ("rdmsr": "=a"(low), "=d"(high): "c"(msr));
	return ((unsigned long long)high << 32) | low;
}

inline unsigned char io_read(unsigned short addr)
{
	unsigned char value;
	asm volatile ("inb %[addr], %[value]"
		      : [value]"=a"(value) : [addr]"d"(addr));
	return value;
}

inline void io_write(unsigned short addr, unsigned char value)
{
	asm volatile ("outb %[value], %[addr]"
		      :: [value]"a"(value), [addr]"d"(addr));
}

inline unsigned short io_read16(unsigned short addr)
{
	unsigned short value;
	asm volatile ("inw %[addr], %[value]"
		      : [value]"=a"(value) : [addr]"d"(addr));
	return value;
}

inline void io_write16(unsigned short addr, unsigned short value)
{
	asm volatile ("outw %[value], %[addr]"
		      :: [value]"a"(value), [addr]"d"(addr));
}

inline unsigned int io_read32(unsigned short addr)
{
	unsigned int value;
	asm volatile ("inl %[addr], %[value]"
		      : [value]"=a"(value) : [addr]"d"(addr));
	return value;
}

inline void io_write32(unsigned short addr, unsigned int value)
{
	asm volatile ("outl %[value], %[addr]"
		      :: [value]"a"(value), [addr]"d"(addr));
}

void gdt_init(void)
{
	/* GDTRの設定 */
	gdtr[0] = ((unsigned long long)gdt << 16) | (sizeof(gdt) - 1);
	gdtr[1] = ((unsigned long long)gdt >> 48);
	asm volatile ("lgdt gdtr");

	/* DS・SSの設定 */
	asm volatile ("movw $2*8, %ax\n"
		      "movw %ax, %ds\n"
		      "movw %ax, %ss\n");

	/* CSの設定 */
	unsigned short selector = SS_KERNEL_CODE;
	unsigned long long dummy;
	asm volatile ("pushq %[selector];"
		      "leaq ret_label(%%rip), %[dummy];"
		      "pushq %[dummy];"
		      "lretq;"
		      "ret_label:"
		      : [dummy]"=r"(dummy)
		      : [selector]"m"(selector));
}
