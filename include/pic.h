#ifndef _PIC_H_
#define _PIC_H_

#define MPIC_IMR_ADDR	0x0021
#define SPIC_IMR_ADDR	0x00a1

void pic_init(void);
void enable_pic_intr(unsigned char intr_no);
void set_pic_eoi(unsigned char intr_no);

#endif
