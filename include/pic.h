#pragma once

void pic_init(void);
void enable_pic_intr(unsigned char intr_no);
void set_pic_eoi(unsigned char intr_no);
