#ifndef _SCHED_H_
#define _SCHED_H_

#include <fs.h>

void sched_init(void);
void sched_start(void);
void enq_task(struct file *f);
void sleep_currnet_task(unsigned long long us);
void sched_test(
	unsigned long long a, unsigned long long b, unsigned long long c,
	unsigned long long d, unsigned long long e);

#endif
