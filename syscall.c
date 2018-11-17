#include <intr.h>
#include <pic.h>

#include <fb.h>
#include <fbcon.h>
#include <kbc.h>
#include <proc.h>
#include <sched.h>

#define SYSCALL_INTR_NO	0x80

enum SYSCCALL_NO {
	SYSCALL_PUTC,
	SYSCALL_GETC,
	SYSCALL_VPUTC,
	SYSCALL_FG,
	SYSCALL_BG,
	SYSCALL_CLS,
	SYSCALL_VCUR_RST,
	SYSCALL_KBC_HDLR,
	SYSCALL_OPEN,
	SYSCALL_GET_FILES,
	SYSCALL_EXEC,
	SYSCALL_ENQ_TASK,
	MAX_SYSCALL_NUM
};

unsigned long long do_syscall_interrupt(
	unsigned long long syscall_id, unsigned long long arg1,
	unsigned long long arg2, unsigned long long arg3)
{
	unsigned long long ret_val = 0;

	switch (syscall_id) {
	case SYSCALL_PUTC:
		putc((char)arg1);
		break;

	case SYSCALL_GETC:
		ret_val = getc();
		break;

	case SYSCALL_VPUTC:
		vputc(arg1);
		break;

	case SYSCALL_FG:
		set_fg(arg1, arg2, arg3);
		break;

	case SYSCALL_BG:
		set_bg(arg1, arg2, arg3);
		break;

	case SYSCALL_CLS:
		clear_screen();
		break;

	case SYSCALL_VCUR_RST:
		vcursor_reset();
		break;

	case SYSCALL_KBC_HDLR:
		kbc_set_handler((void *)arg1);
		break;

	case SYSCALL_OPEN:
		ret_val = (unsigned long long)open((char *)arg1);
		break;

	case SYSCALL_GET_FILES:
		ret_val = get_files((struct file **)arg1);
		break;

	case SYSCALL_EXEC:
		exec((struct file *)arg1);
		break;

	case SYSCALL_ENQ_TASK:
		enq_task((struct file *)arg1);
		break;
	}

	/* PICへ割り込み処理終了を通知(EOI) */
	set_pic_eoi(SYSCALL_INTR_NO);

	return ret_val;
}

void syscall_handler(void);
void syscall_init(void)
{
	set_intr_desc(SYSCALL_INTR_NO, syscall_handler);
	enable_pic_intr(SYSCALL_INTR_NO);
}

unsigned long long syscall(
	unsigned long long syscall_id __attribute__((unused)),
	unsigned long long arg1 __attribute__((unused)),
	unsigned long long arg2 __attribute__((unused)),
	unsigned long long arg3 __attribute__((unused)))
{
	unsigned long long ret_val;

	asm volatile ("int $0x80\n"
		      "movq %%rax, %[ret_val]"
		      : [ret_val]"=r"(ret_val) :);

	return ret_val;
}
