#include <intr.h>
#include <pic.h>

#include <fbcon.h>

#define SYSCALL_INTR_NO		128

void syscall_handler(void);

void do_syscall_interrupt(void)
{
	puts("DO SYSCALL INTERRUPT\r\n");

	while (1);

	/* PICへ割り込み処理終了を通知(EOI) */
	set_pic_eoi(SYSCALL_INTR_NO);
}

void syscall_init(void)
{
	set_intr_desc(SYSCALL_INTR_NO, syscall_handler);
	enable_pic_intr(SYSCALL_INTR_NO);
}
