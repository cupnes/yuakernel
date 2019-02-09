#pragma once

#define CMOS_SEC	0x00
#define CMOS_MIN	0x02
#define CMOS_HOUR	0x04
#define CMOS_WEEK	0x06
#define CMOS_DAY	0x07
#define CMOS_MON	0x08
#define CMOS_YEAR	0x09
#define CMOS_CENTURY	0x32
#define CMOS_STATUS_A	0x0a
#define CMOS_STATUS_B	0x0b

struct __attribute__((packed)) datetime {
	unsigned short year;
	unsigned char mon;
	unsigned char day;
	unsigned char week;
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
};

unsigned char cmos_read_reg(unsigned char reg);
struct datetime *conv_to_jst(struct datetime *dt);
struct datetime *get_datetime(struct datetime *dt);
