#include <lib.h>

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

void putc(char c)
{
	syscall(SYSCALL_PUTC, c, 0, 0);
}

char getc(void)
{
	return syscall(SYSCALL_GETC, 0, 0, 0);
}

void vputc(unsigned char c)
{
	syscall(SYSCALL_VPUTC, c, 0, 0);
}

void set_fg(unsigned char r, unsigned char g, unsigned char b)
{
	syscall(SYSCALL_FG, r, g, b);
}

void set_bg(unsigned char r, unsigned char g, unsigned char b)
{
	syscall(SYSCALL_BG, r, g, b);
}

void clear_screen(void)
{
	syscall(SYSCALL_CLS, 0, 0, 0);
}

void vcursor_reset(void)
{
	syscall(SYSCALL_VCUR_RST, 0, 0, 0);
}

void set_kbc_handler(void *handler)
{
	syscall(SYSCALL_KBC_HDLR, (unsigned long long)handler, 0, 0);
}
