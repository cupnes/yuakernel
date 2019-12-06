#include <x86.h>
#include <sched.h>
#include <hpet.h>
#include <pic.h>
#include <fbcon.h>
#include <fs.h>

#define SCHED_PERIOD	(5 * MS_TO_US)
#define MAX_TASKS		100
#define TASK_STASK_BYTES	4096

unsigned long long task_sp[MAX_TASKS];
volatile int current_task;
unsigned char task_stack[MAX_TASKS - 1][TASK_STASK_BYTES];
int used_tasks;
unsigned long long sleep_timer[MAX_TASKS] = { 0 };
unsigned long long task_status[MAX_TASKS] = { TS_FREE };
static unsigned char is_idle;

void schedule(unsigned long long current_rsp)
{
	if (!is_idle)
		task_sp[current_task] = current_rsp;

	int i;
	for (i = 0; i < used_tasks; i++) {
		if (task_status[i] == TS_SLEEP) {
			if (sleep_timer[i] >= SCHED_PERIOD) {
				sleep_timer[i] -= SCHED_PERIOD;
				if (sleep_timer[i] == 0)
					task_status[i] = TS_RUNNING;
			} else if (sleep_timer[i] > 0) {
				sleep_timer[i] = 0;
				task_status[i] = TS_RUNNING;
			}
		}
	}

	int begin = current_task;
	while (1) {
		current_task = (current_task + 1) % used_tasks;
		if (task_status[current_task] == TS_RUNNING) {
			is_idle = 0;
			break;
		}
		if (current_task == begin) {
			is_idle = 1;
			break;
		}
	}

	if (is_idle) {
		/* 割り込み有効化 */
		set_pic_eoi(HPET_INTR_NO);
		enable_cpu_intr();

		/* アイドルループ */
		while (1)
			cpu_halt();
	}

	set_pic_eoi(HPET_INTR_NO);
	asm volatile ("mov %[sp], %%rsp"
		      :: [sp]"a"(task_sp[current_task]));
	asm volatile (
		"pop	%rdi\n"
		"pop	%rsi\n"
		"pop	%rbp\n"
		"pop	%rbx\n"
		"pop	%rdx\n"
		"pop	%rcx\n"
		"pop	%rax\n"
		"iretq\n");
}

void sched_init(void)
{
	/* 最初に起動させるアプリ向け設定 */
	current_task = 0;
	used_tasks = 1;
	task_status[current_task] = TS_RUNNING;
	is_idle = 0;

	/* 5ms周期の周期タイマー設定 */
	ptimer_setup(SCHED_PERIOD, schedule);
}

void sched_start(void)
{
	/* 周期タイマースタート */
	ptimer_start();
}

int enq_task(struct file *f)
{
	/* 使用可能なタスク番号を先頭の方から取得 */
	int task_id;
	for (task_id = 0; task_id < MAX_TASKS; task_id++) {
		if (task_status[task_id] == TS_FREE)
			break;
	}
	if (task_id >= MAX_TASKS)
		return -1;

	unsigned long long start_addr = (unsigned long long)f->data;

	/* 予めタスクのスタックを適切に積んでおき、スタックポインタを揃える */
	unsigned long long *sp =
		(unsigned long long *)((unsigned char *)task_stack[task_id]
				       + TASK_STASK_BYTES);
	unsigned long long old_sp = (unsigned long long)sp;

	/* push SS */
	--sp;
	*sp = 0x10;

	/* push old RSP */
	--sp;
	*sp = old_sp;

	/* push RFLAGS */
	--sp;
	*sp = 0x202;

	/* push CS */
	--sp;
	*sp = 8;

	/* push RIP */
	--sp;
	*sp = start_addr;

	/* push GR */
	unsigned char i;
	for (i = 0; i < 7; i++) {
		--sp;
		*sp = 0;
	}

	task_sp[task_id] = (unsigned long long)sp;
	task_status[task_id] = TS_RUNNING;

	if (task_id >= used_tasks)
		used_tasks = task_id + 1;

	return task_id;
}

void sleep_currnet_task(unsigned long long us)
{
	sleep_timer[current_task] = us;
	task_status[current_task] = TS_SLEEP;
}

void finish_task(int task_id)
{
	task_status[task_id] = TS_FREE;
}

int get_current_task_id(void)
{
	return current_task;
}

unsigned int get_task_status(int task_id)
{
	return task_status[task_id];
}

void sched_test(
	unsigned long long a, unsigned long long b, unsigned long long c,
	unsigned long long d, unsigned long long e)
{
	puth(a, 16);
	puts("\r\n");
	puth(b, 16);
	puts("\r\n");
	puth(c, 16);
	puts("\r\n");
	puth(d, 16);
	puts("\r\n");
	puth(e, 16);
	puts("\r\n");
}

void exec(struct file *f)
{
	void *start_addr = (void *)f->data;
	asm volatile ("call *%[start_addr]" :: [start_addr]"r"(start_addr));
}
