#ifndef _HPET_H_
#define _HPET_H_

#define US_TO_FS	1000000000
#define SEC_TO_US	1000000

void hpet_init(void);
void sleep(unsigned long long us);
void alert(unsigned long long us, void *handler);
void ptimer_setup(unsigned long long us, void *handler);
void ptimer_start(void);
void ptimer_stop(void);
void dump_tnccr(void);
void show_timer_info(void);

#endif
