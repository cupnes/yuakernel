#include <x86.h>
#include <cmos.h>

#define CMOS_ADDR	0x70
#define CMOS_DATA	0x71

unsigned char cmos_read_reg(unsigned char reg)
{
	io_write(CMOS_ADDR, reg);
	return io_read(CMOS_DATA);
}

unsigned char unbcd(unsigned char bcd)
{
	return ((bcd >> 4) * 10) + (bcd & 0x0f);
}

struct datetime *conv_to_jst(struct datetime *dt)
{
	dt->hour += 9;
	if (dt->hour > 24)
		dt->hour -= 24;

	/* FIXME: convert date to JST. */

	return dt;
}

struct datetime *get_datetime(struct datetime *dt)
{
	dt->year = 2000 + unbcd(cmos_read_reg(CMOS_YEAR));
	dt->mon = unbcd(cmos_read_reg(CMOS_MON));
	dt->day = unbcd(cmos_read_reg(CMOS_DAY));
	dt->week = cmos_read_reg(CMOS_WEEK);
	dt->hour = unbcd(cmos_read_reg(CMOS_HOUR));
	dt->min = unbcd(cmos_read_reg(CMOS_MIN));
	dt->sec = unbcd(cmos_read_reg(CMOS_SEC));
	return dt;
}
