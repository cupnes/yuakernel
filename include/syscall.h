#pragma once

void syscall_init(void);
unsigned long long syscall(
	unsigned long long syscall_id, unsigned long long arg1,
	unsigned long long arg2, unsigned long long arg3);
