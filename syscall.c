#include <intr.h>
#include <pic.h>

#include <fbcon.h>
#include <kbc.h>

#define SYSCALL_INTR_NO		128

enum SYSCCALL_NO {
	SYSCALL_PUTC,
	SYSCALL_GETC,
	MAX_SYSCALL_NUM
};

void syscall_handler(void);

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
	}

	/* PICへ割り込み処理終了を通知(EOI) */
	set_pic_eoi(SYSCALL_INTR_NO);

	return ret_val;
}

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
