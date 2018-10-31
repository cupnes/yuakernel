#ifndef _LIB_H_
#define _LIB_H_

enum SYSCCALL_NO {
	SYSCALL_PUTC,
	MAX_SYSCALL_NUM
};

unsigned long long syscall(
	unsigned long long syscall_id __attribute__((unused)),
	unsigned long long arg1 __attribute__((unused)),
	unsigned long long arg2 __attribute__((unused)),
	unsigned long long arg3 __attribute__((unused)));

#endif
