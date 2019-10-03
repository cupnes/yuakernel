#pragma once

#define X280_HDA_BUS_NUM	0x00
#define X280_HDA_DEV_NUM	0x1f
#define X280_HDA_FUNC_NUM	0x03

void hda_init(
	unsigned char bus_num, unsigned char dev_num, unsigned char func_num);
