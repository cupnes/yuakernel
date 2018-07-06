#include <hpet.h>
#include <pic.h>

#include <fb.h>
#include <font.h>
#include <fbcon.h>

#define SCHED_PERIOD	(5 * MS_TO_US)

unsigned long long task_sp[2];
volatile unsigned int current_task = 0;

#define NUM_TASKS		2
#define TASK_B_STASK_BYTES	4096
unsigned char taskB_stack[TASK_B_STASK_BYTES];

struct task_fb {
	unsigned int x;
	unsigned int y;
	unsigned int w;
	unsigned int h;
} tfb[NUM_TASKS];

void do_taskB(void)
{
	unsigned char r = 0, g = 0, b = 0;
	while (1) {
		unsigned int x, y;
		for (y = 0; y < tfb[1].h; y++) {
			for (x = 0; x < tfb[1].w; x++) {
				draw_px(x, y, r++, g, b);
			}
			g++;
		}
		b++;
	}
}

void schedule(unsigned long long current_rsp)
{
	task_sp[current_task] = current_rsp;
	current_task = (current_task + 1) % NUM_TASKS;
	fb.hr = tfb[current_task].w;
	fb.vr = tfb[current_task].h;
	set_start(tfb[current_task].x, tfb[current_task].y);

	set_pic_eoi(HPET_INTR_NO);
	asm volatile ("mov %[sp], %%rsp"
		      :: [sp]"a"(task_sp[current_task]));
	asm volatile (
		/*
		"pop	%r15\n"
		"pop	%r14\n"
		"pop	%r13\n"
		"pop	%r12\n"
		"pop	%r11\n"
		"pop	%r10\n"
		"pop	%r9\n"
		"pop	%r8\n"
		*/
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
	/* 5ms周期の周期タイマー設定 */
	ptimer_setup(SCHED_PERIOD, schedule);

	tfb[0].x = fb_real.hr / 2;
	tfb[0].y = 0;
	tfb[0].w = fb_real.hr / 2;
	tfb[0].h = fb_real.vr;

	tfb[1].x = 0;
	tfb[1].y = 0;
	tfb[1].w = fb_real.hr / 2;
	tfb[1].h = fb_real.vr;

	fb.hr = tfb[0].w;
	fb.vr = tfb[0].h;
	set_start(tfb[0].x, tfb[0].y);

	/* TODO: 予めTaskBのスタックを適当に積んでおき、スタックポインタを揃える */
	unsigned long long *sp =
		(unsigned long long *)((unsigned char *)taskB_stack + TASK_B_STASK_BYTES);
	unsigned long long old_sp = (unsigned long long)sp;

	/* push RIP */
	--sp;
	*sp = (unsigned long long)do_taskB;

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
	*sp = (unsigned long long)do_taskB;

	/* push GR */
	unsigned char i;
	for (i = 0; i < 7; i++) {
		--sp;
		*sp = 0;
	}

	task_sp[1] = (unsigned long long)sp;
}

void sched_start(void)
{
	/* 周期タイマースタート */
	ptimer_start();
}
