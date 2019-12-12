#include <intr.h>
#include <pic.h>
#include <apic.h>
#include <serial.h>
#include <fb.h>
#include <fbcon.h>
#include <kbc.h>
#include <sched.h>
#include <nic.h>
#include <cmos.h>
#include <ap.h>

#include <common.h>

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
	SYSCALL_RCV_PKT,
	SYSCALL_SND_PKT,
	SYSCALL_MOV_CUR,
	SYSCALL_GET_CUR_Y,
	SYSCALL_DRAW_BG,
	SYSCALL_DRAW_FG,
	SYSCALL_DRAW_IMG,
	SYSCALL_GET_DATETIME,
	SYSCALL_SLEEP,
	SYSCALL_GET_PX,
	SYSCALL_FINISH_TASK,
	SYSCALL_GET_MAC,
	SYSCALL_DRAW_PX_FG,
	SYSCALL_DRAW_PX_BG,
	SYSCALL_NIC_RX_EN,
	SYSCALL_SER_GETC,
	SYSCALL_SER_PUTC,
	SYSCALL_IS_ALIVE,
	SYSCALL_FINISH_CURRENT_TASK,
	SYSCALL_EXEC_AP,
	SYSCALL_FILL_RECT,
	SYSCALL_DRAW_PX,
	SYSCALL_GET_PNUM,
	MAX_SYSCALL_NUM
};

unsigned long long do_syscall_interrupt(
	unsigned long long syscall_id, unsigned long long arg1,
	unsigned long long arg2, unsigned long long arg3,
	unsigned long long current_rsp)
{
	unsigned long long ret_val = 0;
	unsigned int task_status;
	struct rect *r;
	struct pixelformat *px;

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
		move_cursor(0, 0);
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
		ret_val = get_files((struct file **)arg1, (char)arg2);
		break;

	case SYSCALL_EXEC:
		exec((struct file *)arg1);
		break;

	case SYSCALL_ENQ_TASK:
		ret_val = (unsigned long long)enq_task((struct file *)arg1);
		break;

	case SYSCALL_RCV_PKT:
		ret_val = receive_frame((void *)arg1);
		break;

	case SYSCALL_SND_PKT:
		send_frame((void *)arg1, arg2);
		break;

	case SYSCALL_MOV_CUR:
		move_cursor(arg1, arg2);
		break;

	case SYSCALL_GET_CUR_Y:
		ret_val = get_cursor_y();
		break;

	case SYSCALL_DRAW_BG:
		draw_bg((struct file *)arg1);
		break;

	case SYSCALL_DRAW_FG:
		draw_fg((struct file *)arg1);
		break;

	case SYSCALL_DRAW_IMG:
		draw_image((struct image *)arg1, arg2, arg3);
		break;

	case SYSCALL_GET_DATETIME:
		get_datetime((struct datetime *)arg1);
		break;

	case SYSCALL_SLEEP:
		sleep_currnet_task(arg1);
		schedule(current_rsp);
		break;

	case SYSCALL_GET_PX:
		get_px(arg1, arg2, (struct pixelformat *)arg3);
		break;

	case SYSCALL_FINISH_TASK:
		finish_task(arg1);
		schedule(current_rsp);
		break;

	case SYSCALL_GET_MAC:
		memcpy((void *)arg1, nic_mac_addr, 6);
		break;

	case SYSCALL_DRAW_PX_FG:
		draw_px_fg(arg1, arg2);
		break;

	case SYSCALL_DRAW_PX_BG:
		draw_px_bg(arg1, arg2);
		break;

	case SYSCALL_NIC_RX_EN:
		nic_rx_enable();
		break;

	case SYSCALL_SER_GETC:
		ret_val = ser_getc_poll();
		break;

	case SYSCALL_SER_PUTC:
		ser_putc_poll(arg1);
		break;

	case SYSCALL_IS_ALIVE:
		task_status = get_task_status(arg1);
		if (task_status != TS_FREE)
			ret_val = 1;
		else
			ret_val = 0;
		break;

	case SYSCALL_FINISH_CURRENT_TASK:
		finish_task(get_current_task_id());
		schedule(current_rsp);
		break;

	case SYSCALL_EXEC_AP:
		ret_val = ap_enq_task((struct file *)arg1, arg2);
		break;

	case SYSCALL_FILL_RECT:
		r = (struct rect *)arg1;
		px = (struct pixelformat *)arg2;
		fill_rect(r->x, r->y, r->w, r->h, px->r, px->g, px->b);
		break;

	case SYSCALL_DRAW_PX:
		px = (struct pixelformat *)arg3;
		draw_px(arg1, arg2, px->r, px->g, px->b);
		break;

	case SYSCALL_GET_PNUM:
		ret_val = get_pnum();
		break;
	}

	/* PICへ割り込み処理終了を通知(EOI) */
	set_pic_eoi(SYSCALL_INTR_NO);

	return ret_val;
}

void syscall_handler(void);
void syscall_init(void)
{
	void *addr;
	asm volatile ("lea syscall_handler, %[addr]"
		      : [addr]"=r"(addr));
	set_intr_desc(SYSCALL_INTR_NO, addr);
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
