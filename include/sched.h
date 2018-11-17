#ifndef _SCHED_H_
#define _SCHED_H_

#include <fs.h>

void sched_init(void);
void sched_start(void);
void enq_task(struct file *f);

#endif
