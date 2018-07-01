#include <intr.h>
#include <x86.h>
#include <fb.h>
#include <fbcon.h>

#define DESC_TYPE_INTR	14

struct interrupt_descriptor idt[MAX_INTR_NO];
unsigned long long idtr[2];

void default_handler(void);
void divide_error_exception(void);
void debug_exception(void);
void nmi_interrupt(void);
void breakpoint_exception(void);
void overflow_exception(void);
void bound_range_exceeded_exception(void);
void invalid_opcode_exception(void);
void device_not_available_exception(void);
void double_fault_exception(void);
void coprocessor_segment_overrun(void);
void invalid_tss_exception(void);
void segment_not_present(void);
void stack_fault_exception(void);
void general_protection_exception(void);
void page_fault_exception(void);
void x87_fpu_floating_point_error(void);
void alignment_check_exception(void);
void machine_check_exception(void);
void simd_floating_point_exception(void);
void virtualization_exception(void);

void set_intr_desc(unsigned char intr_no, void *handler)
{
	idt[intr_no].offset_00_15 = (unsigned long long)handler;
	idt[intr_no].segment_selector = SS_KERNEL_CODE;
	idt[intr_no].type = DESC_TYPE_INTR;
	idt[intr_no].p = 1;
	idt[intr_no].offset_31_16 = (unsigned long long)handler >> 16;
	idt[intr_no].offset_63_32 = (unsigned long long)handler >> 32;
}

void intr_init(void)
{
	set_intr_desc(0, divide_error_exception);
	set_intr_desc(1, debug_exception);
	set_intr_desc(2, nmi_interrupt);
	set_intr_desc(3, breakpoint_exception);
	set_intr_desc(4, overflow_exception);
	set_intr_desc(5, bound_range_exceeded_exception);
	set_intr_desc(6, invalid_opcode_exception);
	set_intr_desc(7, device_not_available_exception);
	set_intr_desc(8, double_fault_exception);
	set_intr_desc(9, coprocessor_segment_overrun);
	set_intr_desc(10, invalid_tss_exception);
	set_intr_desc(11, segment_not_present);
	set_intr_desc(12, stack_fault_exception);
	set_intr_desc(13, general_protection_exception);
	set_intr_desc(14, page_fault_exception);
	set_intr_desc(16, x87_fpu_floating_point_error);
	set_intr_desc(17, alignment_check_exception);
	set_intr_desc(18, machine_check_exception);
	set_intr_desc(19, simd_floating_point_exception);
	set_intr_desc(20, virtualization_exception);

	int i;
	for (i = 21; i < MAX_INTR_NO; i++)
		set_intr_desc(i, default_handler);

	idtr[0] = ((unsigned long long)idt << 16) | (sizeof(idt) - 1);
	idtr[1] = ((unsigned long long)idt >> 48);
	__asm__ ("lidt idtr");
}

void do_default_interrupt(void)
{
	clear_screen();
	puts("DO DEFAULT INTERRUPT\r\n");
	while (1);
}

void do_divide_error_exception(void)
{
	clear_screen();
	puts("DIVIDE ERROR EXCEPTION\r\n");
	while (1);
}

void do_debug_exception(void)
{
	clear_screen();
	puts("DEBUG EXCEPTION\r\n");
	while (1);
}

void do_nmi_interrupt(void)
{
	clear_screen();
	puts("NMI INTERRUPT\r\n");
	while (1);
}

void do_breakpoint_exception(void)
{
	clear_screen();
	puts("BREAKPOINT EXCEPTION\r\n");
	while (1);
}

void do_overflow_exception(void)
{
	clear_screen();
	puts("OVERFLOW EXCEPTION\r\n");
	while (1);
}

void do_bound_range_exceeded_exception(void)
{
	clear_screen();
	puts("BOUND RANGE EXCEEDED EXCEPTION\r\n");
	while (1);
}

void do_invalid_opcode_exception(void)
{
	clear_screen();
	puts("INVALID OPCODE EXCEPTION\r\n");
	while (1);
}

void do_device_not_available_exception(void)
{
	clear_screen();
	puts("DEVICE NOT AVAILABLE EXCEPTION\r\n");
	while (1);
}

void do_double_fault_exception(void)
{
	clear_screen();
	puts("DOUBLE FAULT EXCEPTION\r\n");
	while (1);
}

void do_coprocessor_segment_overrun(void)
{
	clear_screen();
	puts("COPROCESSOR SEGMENT OVERRUN\r\n");
	while (1);
}

void do_invalid_tss_exception(void)
{
	clear_screen();
	puts("INVALID TSS EXCEPTION\r\n");
	while (1);
}

void do_segment_not_present(void)
{
	clear_screen();
	puts("SEGMENT NOT PRESENT\r\n");
	while (1);
}

void do_stack_fault_exception(void)
{
	clear_screen();
	puts("STACK FAULT EXCEPTION\r\n");
	while (1);
}

void do_general_protection_exception(void)
{
	clear_screen();
	puts("GENERAL PROTECTION EXCEPTION\r\n");
	while (1);
}

void do_page_fault_exception(void)
{
	clear_screen();
	puts("PAGE FAULT EXCEPTION\r\n");
	while (1);
}

void do_x87_fpu_floating_point_error(void)
{
	clear_screen();
	puts("X87 FPU FLOATING POINT ERROR\r\n");
	while (1);
}

void do_alignment_check_exception(void)
{
	clear_screen();
	puts("ALIGNMENT CHECK EXCEPTION\r\n");
	while (1);
}

void do_machine_check_exception(void)
{
	clear_screen();
	puts("MACHINE CHECK EXCEPTION\r\n");
	while (1);
}

void do_simd_floating_point_exception(void)
{
	clear_screen();
	puts("SIMD FLOATING POINT EXCEPTION\r\n");
	while (1);
}

void do_virtualization_exception(void)
{
	clear_screen();
	puts("VIRTUALIZATION EXCEPTION\r\n");
	while (1);
}
