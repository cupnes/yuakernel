#include <x86.h>

#define CMOS_ADDR	0x70
#define CMOS_DATA	0x71

unsigned char cmos_read_reg(unsigned char reg)
{
	io_write(CMOS_ADDR, reg);
	return io_read(CMOS_DATA);
}
