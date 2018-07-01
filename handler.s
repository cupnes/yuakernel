	.global default_handler
default_handler:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_default_interrupt
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global divide_error_exception
divide_error_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_divide_error_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global debug_exception
debug_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_debug_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global nmi_interrupt
nmi_interrupt:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_nmi_interrupt
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global breakpoint_exception
breakpoint_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_breakpoint_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global overflow_exception
overflow_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_overflow_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global bound_range_exceeded_exception
bound_range_exceeded_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_bound_range_exceeded_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global invalid_opcode_exception
invalid_opcode_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_invalid_opcode_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global device_not_available_exception
device_not_available_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_device_not_available_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global double_fault_exception
double_fault_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_double_fault_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global coprocessor_segment_overrun
coprocessor_segment_overrun:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_coprocessor_segment_overrun
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global invalid_tss_exception
invalid_tss_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_invalid_tss_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global segment_not_present
segment_not_present:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_segment_not_present
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global stack_fault_exception
stack_fault_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_stack_fault_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global general_protection_exception
general_protection_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_general_protection_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global page_fault_exception
page_fault_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_page_fault_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global x87_fpu_floating_point_error
x87_fpu_floating_point_error:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_x87_fpu_floating_point_error
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global alignment_check_exception
alignment_check_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_alignment_check_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global machine_check_exception
machine_check_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_machine_check_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global simd_floating_point_exception
simd_floating_point_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_simd_floating_point_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global virtualization_exception
virtualization_exception:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_virtualization_exception
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global	hpet_handler
hpet_handler:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	mov	%rsp, %rdi
	call	do_hpet_interrupt
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq

	.global	kbc_handler
kbc_handler:
	push	%rax
	push	%rcx
	push	%rdx
	push	%rbx
	push	%rbp
	push	%rsi
	push	%rdi
	call	do_kbc_interrupt
	pop	%rdi
	pop	%rsi
	pop	%rbp
	pop	%rbx
	pop	%rdx
	pop	%rcx
	pop	%rax
	iretq
