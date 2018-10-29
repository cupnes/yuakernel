#ifndef _SYSCALL_H_
#define _SYSCALL_H_

void syscall_init(void);
unsigned long long syscall(
	unsigned long long syscall_id, unsigned long long arg1,
	unsigned long long arg2, unsigned long long arg3);

#endif
