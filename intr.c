#include <intr.h>
#include <x86.h>
#include <fb.h>
#include <fbcon.h>
#include <default_intr.h>

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

#ifdef DEBUG_INTR_EACH_HANDLER
	set_intr_desc(21, default_handler_21);
	set_intr_desc(22, default_handler_22);
	set_intr_desc(23, default_handler_23);
	set_intr_desc(24, default_handler_24);
	set_intr_desc(25, default_handler_25);
	set_intr_desc(26, default_handler_26);
	set_intr_desc(27, default_handler_27);
	set_intr_desc(28, default_handler_28);
	set_intr_desc(29, default_handler_29);
	set_intr_desc(30, default_handler_30);
	set_intr_desc(31, default_handler_31);
	set_intr_desc(32, default_handler_32);
	set_intr_desc(33, default_handler_33);
	set_intr_desc(34, default_handler_34);
	set_intr_desc(35, default_handler_35);
	set_intr_desc(36, default_handler_36);
	set_intr_desc(37, default_handler_37);
	set_intr_desc(38, default_handler_38);
	set_intr_desc(39, default_handler_39);
	set_intr_desc(40, default_handler_40);
	set_intr_desc(41, default_handler_41);
	set_intr_desc(42, default_handler_42);
	set_intr_desc(43, default_handler_43);
	set_intr_desc(44, default_handler_44);
	set_intr_desc(45, default_handler_45);
	set_intr_desc(46, default_handler_46);
	set_intr_desc(47, default_handler_47);
	set_intr_desc(48, default_handler_48);
	set_intr_desc(49, default_handler_49);
	set_intr_desc(50, default_handler_50);
	set_intr_desc(51, default_handler_51);
	set_intr_desc(52, default_handler_52);
	set_intr_desc(53, default_handler_53);
	set_intr_desc(54, default_handler_54);
	set_intr_desc(55, default_handler_55);
	set_intr_desc(56, default_handler_56);
	set_intr_desc(57, default_handler_57);
	set_intr_desc(58, default_handler_58);
	set_intr_desc(59, default_handler_59);
	set_intr_desc(60, default_handler_60);
	set_intr_desc(61, default_handler_61);
	set_intr_desc(62, default_handler_62);
	set_intr_desc(63, default_handler_63);
	set_intr_desc(64, default_handler_64);
	set_intr_desc(65, default_handler_65);
	set_intr_desc(66, default_handler_66);
	set_intr_desc(67, default_handler_67);
	set_intr_desc(68, default_handler_68);
	set_intr_desc(69, default_handler_69);
	set_intr_desc(70, default_handler_70);
	set_intr_desc(71, default_handler_71);
	set_intr_desc(72, default_handler_72);
	set_intr_desc(73, default_handler_73);
	set_intr_desc(74, default_handler_74);
	set_intr_desc(75, default_handler_75);
	set_intr_desc(76, default_handler_76);
	set_intr_desc(77, default_handler_77);
	set_intr_desc(78, default_handler_78);
	set_intr_desc(79, default_handler_79);
	set_intr_desc(80, default_handler_80);
	set_intr_desc(81, default_handler_81);
	set_intr_desc(82, default_handler_82);
	set_intr_desc(83, default_handler_83);
	set_intr_desc(84, default_handler_84);
	set_intr_desc(85, default_handler_85);
	set_intr_desc(86, default_handler_86);
	set_intr_desc(87, default_handler_87);
	set_intr_desc(88, default_handler_88);
	set_intr_desc(89, default_handler_89);
	set_intr_desc(90, default_handler_90);
	set_intr_desc(91, default_handler_91);
	set_intr_desc(92, default_handler_92);
	set_intr_desc(93, default_handler_93);
	set_intr_desc(94, default_handler_94);
	set_intr_desc(95, default_handler_95);
	set_intr_desc(96, default_handler_96);
	set_intr_desc(97, default_handler_97);
	set_intr_desc(98, default_handler_98);
	set_intr_desc(99, default_handler_99);
	set_intr_desc(100, default_handler_100);
	set_intr_desc(101, default_handler_101);
	set_intr_desc(102, default_handler_102);
	set_intr_desc(103, default_handler_103);
	set_intr_desc(104, default_handler_104);
	set_intr_desc(105, default_handler_105);
	set_intr_desc(106, default_handler_106);
	set_intr_desc(107, default_handler_107);
	set_intr_desc(108, default_handler_108);
	set_intr_desc(109, default_handler_109);
	set_intr_desc(110, default_handler_110);
	set_intr_desc(111, default_handler_111);
	set_intr_desc(112, default_handler_112);
	set_intr_desc(113, default_handler_113);
	set_intr_desc(114, default_handler_114);
	set_intr_desc(115, default_handler_115);
	set_intr_desc(116, default_handler_116);
	set_intr_desc(117, default_handler_117);
	set_intr_desc(118, default_handler_118);
	set_intr_desc(119, default_handler_119);
	set_intr_desc(120, default_handler_120);
	set_intr_desc(121, default_handler_121);
	set_intr_desc(122, default_handler_122);
	set_intr_desc(123, default_handler_123);
	set_intr_desc(124, default_handler_124);
	set_intr_desc(125, default_handler_125);
	set_intr_desc(126, default_handler_126);
	set_intr_desc(127, default_handler_127);
	set_intr_desc(128, default_handler_128);
	set_intr_desc(129, default_handler_129);
	set_intr_desc(130, default_handler_130);
	set_intr_desc(131, default_handler_131);
	set_intr_desc(132, default_handler_132);
	set_intr_desc(133, default_handler_133);
	set_intr_desc(134, default_handler_134);
	set_intr_desc(135, default_handler_135);
	set_intr_desc(136, default_handler_136);
	set_intr_desc(137, default_handler_137);
	set_intr_desc(138, default_handler_138);
	set_intr_desc(139, default_handler_139);
	set_intr_desc(140, default_handler_140);
	set_intr_desc(141, default_handler_141);
	set_intr_desc(142, default_handler_142);
	set_intr_desc(143, default_handler_143);
	set_intr_desc(144, default_handler_144);
	set_intr_desc(145, default_handler_145);
	set_intr_desc(146, default_handler_146);
	set_intr_desc(147, default_handler_147);
	set_intr_desc(148, default_handler_148);
	set_intr_desc(149, default_handler_149);
	set_intr_desc(150, default_handler_150);
	set_intr_desc(151, default_handler_151);
	set_intr_desc(152, default_handler_152);
	set_intr_desc(153, default_handler_153);
	set_intr_desc(154, default_handler_154);
	set_intr_desc(155, default_handler_155);
	set_intr_desc(156, default_handler_156);
	set_intr_desc(157, default_handler_157);
	set_intr_desc(158, default_handler_158);
	set_intr_desc(159, default_handler_159);
	set_intr_desc(160, default_handler_160);
	set_intr_desc(161, default_handler_161);
	set_intr_desc(162, default_handler_162);
	set_intr_desc(163, default_handler_163);
	set_intr_desc(164, default_handler_164);
	set_intr_desc(165, default_handler_165);
	set_intr_desc(166, default_handler_166);
	set_intr_desc(167, default_handler_167);
	set_intr_desc(168, default_handler_168);
	set_intr_desc(169, default_handler_169);
	set_intr_desc(170, default_handler_170);
	set_intr_desc(171, default_handler_171);
	set_intr_desc(172, default_handler_172);
	set_intr_desc(173, default_handler_173);
	set_intr_desc(174, default_handler_174);
	set_intr_desc(175, default_handler_175);
	set_intr_desc(176, default_handler_176);
	set_intr_desc(177, default_handler_177);
	set_intr_desc(178, default_handler_178);
	set_intr_desc(179, default_handler_179);
	set_intr_desc(180, default_handler_180);
	set_intr_desc(181, default_handler_181);
	set_intr_desc(182, default_handler_182);
	set_intr_desc(183, default_handler_183);
	set_intr_desc(184, default_handler_184);
	set_intr_desc(185, default_handler_185);
	set_intr_desc(186, default_handler_186);
	set_intr_desc(187, default_handler_187);
	set_intr_desc(188, default_handler_188);
	set_intr_desc(189, default_handler_189);
	set_intr_desc(190, default_handler_190);
	set_intr_desc(191, default_handler_191);
	set_intr_desc(192, default_handler_192);
	set_intr_desc(193, default_handler_193);
	set_intr_desc(194, default_handler_194);
	set_intr_desc(195, default_handler_195);
	set_intr_desc(196, default_handler_196);
	set_intr_desc(197, default_handler_197);
	set_intr_desc(198, default_handler_198);
	set_intr_desc(199, default_handler_199);
	set_intr_desc(200, default_handler_200);
	set_intr_desc(201, default_handler_201);
	set_intr_desc(202, default_handler_202);
	set_intr_desc(203, default_handler_203);
	set_intr_desc(204, default_handler_204);
	set_intr_desc(205, default_handler_205);
	set_intr_desc(206, default_handler_206);
	set_intr_desc(207, default_handler_207);
	set_intr_desc(208, default_handler_208);
	set_intr_desc(209, default_handler_209);
	set_intr_desc(210, default_handler_210);
	set_intr_desc(211, default_handler_211);
	set_intr_desc(212, default_handler_212);
	set_intr_desc(213, default_handler_213);
	set_intr_desc(214, default_handler_214);
	set_intr_desc(215, default_handler_215);
	set_intr_desc(216, default_handler_216);
	set_intr_desc(217, default_handler_217);
	set_intr_desc(218, default_handler_218);
	set_intr_desc(219, default_handler_219);
	set_intr_desc(220, default_handler_220);
	set_intr_desc(221, default_handler_221);
	set_intr_desc(222, default_handler_222);
	set_intr_desc(223, default_handler_223);
	set_intr_desc(224, default_handler_224);
	set_intr_desc(225, default_handler_225);
	set_intr_desc(226, default_handler_226);
	set_intr_desc(227, default_handler_227);
	set_intr_desc(228, default_handler_228);
	set_intr_desc(229, default_handler_229);
	set_intr_desc(230, default_handler_230);
	set_intr_desc(231, default_handler_231);
	set_intr_desc(232, default_handler_232);
	set_intr_desc(233, default_handler_233);
	set_intr_desc(234, default_handler_234);
	set_intr_desc(235, default_handler_235);
	set_intr_desc(236, default_handler_236);
	set_intr_desc(237, default_handler_237);
	set_intr_desc(238, default_handler_238);
	set_intr_desc(239, default_handler_239);
	set_intr_desc(240, default_handler_240);
	set_intr_desc(241, default_handler_241);
	set_intr_desc(242, default_handler_242);
	set_intr_desc(243, default_handler_243);
	set_intr_desc(244, default_handler_244);
	set_intr_desc(245, default_handler_245);
	set_intr_desc(246, default_handler_246);
	set_intr_desc(247, default_handler_247);
	set_intr_desc(248, default_handler_248);
	set_intr_desc(249, default_handler_249);
	set_intr_desc(250, default_handler_250);
	set_intr_desc(251, default_handler_251);
	set_intr_desc(252, default_handler_252);
	set_intr_desc(253, default_handler_253);
	set_intr_desc(254, default_handler_254);
	set_intr_desc(255, default_handler_255);
#else
	unsigned short i;
	for (i = 21; i <= 255; i++)
		set_intr_desc(i, default_handler);
#endif

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
