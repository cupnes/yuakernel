#pragma once

#include <fs.h>

enum TASK_STATUS {
	TS_FREE,
	TS_RUNNING,
	TS_SLEEP
};

void schedule(unsigned long long current_rsp);
void sched_init(void);
void sched_start(void);
int enq_task(struct file *f);
void sleep_currnet_task(unsigned long long us);
void finish_task(int task_id);
unsigned int get_task_status(int task_id);
void sched_test(
	unsigned long long a, unsigned long long b, unsigned long long c,
	unsigned long long d, unsigned long long e);
void exec(struct file *f);
