#pragma once

void set_intr_desc(unsigned char intr_no, void *handler);
void intr_init(void);

void default_handler(void);
