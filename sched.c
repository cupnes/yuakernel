#include <hpet.h>

#include <fbcon.h>

#define SCHED_PERIOD	(5 * MS_TO_US)

unsigned long long task_sp[2];

void schedule(void)
{
	putc('.');
}

void sched_init(void)
{
	/* 5ms周期の周期タイマー設定 */
	ptimer_setup(SCHED_PERIOD, schedule);
}

void sched_start(void)
{
	/* 周期タイマースタート */
	ptimer_start();
}
