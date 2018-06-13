#include <hpet.h>
#include <acpi.h>
#include <intr.h>
#include <pic.h>
#include <fb.h>
#include <fbcon.h>

#define HPET_INTR_NO		32

struct HPET_TABLE {
	struct SDTH header;
	unsigned int event_timer_block_id;
	struct ACPI_ADDRESS base_address;
	unsigned char hpet_number;
	unsigned short minimum_tick;
	unsigned char flags;
} *hpet_table;

struct hpet_info {
	unsigned long long reg_base;
	unsigned char num_timers;
	unsigned int counter_clk_period;
} hi;

/* General Capabilities and ID Register */
#define GCIDR_ADDR	(hi.reg_base)
#define GCIDR	(*(volatile unsigned long long *)GCIDR_ADDR)
union gcidr {
	unsigned long long raw;
	struct {
		unsigned long long rev_id:8;
		unsigned long long num_tim_cap:5;
		unsigned long long count_size_cap:1;
		unsigned long long _reserved:1;
		unsigned long long leg_route_cap:1;
		unsigned long long vendor_id:16;
		unsigned long long counter_clk_period:32;
	};
};

/* General Configuration Register */
#define GCR_ADDR	(hi.reg_base + 0x10)
#define GCR	(*(volatile unsigned long long *)GCR_ADDR)
union gcr {
	unsigned long long raw;
	struct {
		unsigned long long enable_cnf:1;
		unsigned long long leg_rt_cnf:1;
		unsigned long long _reserved:62;
	};
};

/* General Interrupt Status Register */
#define GISR_ADDR	(hi.reg_base + 0x20)
#define GISR	(*(volatile unsigned long long *)GISR_ADDR)
union gisr {
	unsigned long long raw;
	struct {
		unsigned long long t0_int_sts:1;
		unsigned long long t1_t31_int_sts:31;
		unsigned long long _reserved:32;
	};
};

/* Main Counter Register */
#define MCR_ADDR	(hi.reg_base + 0xf0)
#define MCR	(*(volatile unsigned long long *)MCR_ADDR)

/* Timer N Configuration and Capabilities Register */
#define TNCCR_ADDR(n)	(hi.reg_base + (0x20 * (n)) + 0x100)
#define TNCCR(n)	(*(volatile unsigned long long *)(TNCCR_ADDR(n)))
#define TNCCR_INT_TYPE_EDGE	0
#define TNCCR_INT_TYPE_LEVEL	1
#define TNCCR_TYPE_NON_PERIODIC	0
#define TNCCR_TYPE_PERIODIC	1
union tnccr {
	unsigned long long raw;
	struct {
		unsigned long long _reserved1:1;
		unsigned long long int_type_cnf:1;
		unsigned long long int_enb_cnf:1;
		unsigned long long type_cnf:1;
		unsigned long long per_int_cap:1;
		unsigned long long size_cap:1;
		unsigned long long val_set_cnf:1;
		unsigned long long _reserved2:1;
		unsigned long long mode32_cnf:1;
		unsigned long long int_route_cnf:5;
		unsigned long long fsb_en_cnf:1;
		unsigned long long fsb_int_del_cap:1;
		unsigned long long _reserved3:16;
		unsigned long long int_route_cap:32;
	};
};

/* Timer N Comparator Register */
#define TNCR_ADDR(n)	(hi.reg_base + (0x20 * (n)) + 0x108)
#define TNCR(n)	(*(volatile unsigned long long *)(TNCR_ADDR(n)))

#define TIMER_N	0	/* 使用するタイマー番号 */
#define TIMER_N_MASK	((unsigned long long)(1 << TIMER_N))

void hpet_handler(void);
void (*user_handler)(void);
unsigned char is_oneshot = 0;
unsigned long long cmpr_clk_counts;

void hpet_init(void)
{
	/* HPET tableを取得 */
	hpet_table = (struct HPET_TABLE *)acpi_find_sdt("HPET");

	/* struct hpet_info hi のメンバに値を設定 */
	hi.reg_base = hpet_table->base_address.address;
	union gcidr gcidr;
	gcidr.raw = GCIDR;
	hi.num_timers = gcidr.num_tim_cap + 1;
		/* gcidr.num_tim_capは最後のタイマー番号(0始まり)のため、
		 * タイマーの数としては1を足す */
	hi.counter_clk_period = gcidr.counter_clk_period;

	/* タイマー無効化 */
	union gcr gcr;
	gcr.raw = GCR;
	gcr.enable_cnf = 0;
	GCR = gcr.raw;

	/* IDTへループバック関数登録 */
	set_intr_desc(HPET_INTR_NO, hpet_handler);
	enable_pic_intr(HPET_INTR_NO);
}

void sleep(unsigned long long us)
{
	/* 現在のmain counterのカウント値を取得 */
	unsigned long long mc_now = MCR;

	/* usマイクロ秒後のmain counterのカウント値を算出 */
	unsigned long long fs = us * US_TO_FS;
	union gcidr gcidr;
	gcidr.raw = GCIDR;
	unsigned long long mc_duration = fs / gcidr.counter_clk_period;
	unsigned long long mc_after = mc_now + mc_duration;

	/* タイマーが無効であれば有効化する */
	union gcr gcr;
	gcr.raw = GCR;
	if (!gcr.enable_cnf) {
		gcr.enable_cnf = 1;
		GCR = gcr.raw;
	}

	/* usマイクロ秒の経過を待つ */
	while (MCR < mc_after);
}

void do_hpet_interrupt(void)
{
	if (is_oneshot == 1) {
		/* タイマー無効化 */
		union gcr gcr;
		gcr.raw = GCR;
		gcr.enable_cnf = 0;
		GCR = gcr.raw;

		is_oneshot = 0;
	}

	/* ユーザーハンドラを呼び出す */
	user_handler();

	/* PICへ割り込み処理終了を通知(EOI) */
	set_pic_eoi(HPET_INTR_NO);
}

void alert(unsigned long long us, void *handler)
{
	/* ユーザーハンドラ設定 */
	user_handler = handler;

	/* 割り込み有効化・エッジトリガー設定 */
	union tnccr tnccr;
	tnccr.raw = TNCCR(TIMER_N);
	tnccr.int_enb_cnf = 1;
	tnccr.int_type_cnf = TNCCR_INT_TYPE_EDGE;
	tnccr.type_cnf = TNCCR_TYPE_NON_PERIODIC;
	tnccr._reserved1 = 0;
	tnccr._reserved2 = 0;
	tnccr._reserved3 = 0;
	TNCCR(TIMER_N) = tnccr.raw;

	/* main counterをゼロクリア */
	MCR = (unsigned long long)0;

	/* コンパレータ設定 */
	unsigned long long femt_sec = us * US_TO_FS;
	unsigned long long clk_counts = femt_sec / hi.counter_clk_period;
	TNCR(TIMER_N) = clk_counts;

	is_oneshot = 1;

	/* LegacyReplacement Route有効化・タイマー有効化 */
	union gcr gcr;
	gcr.raw = GCR;
	gcr.leg_rt_cnf = 1;
	gcr.enable_cnf = 1;
	GCR = gcr.raw;
}

void ptimer_setup(unsigned long long us, void *handler)
{
	/* タイマー無効化 */
	union gcr gcr;
	gcr.raw = GCR;
	gcr.enable_cnf = 0;
	GCR = gcr.raw;

	/* ユーザーハンドラ設定 */
	user_handler = handler;

	/* 割り込み有効化・エッジトリガー設定 */
	union tnccr tnccr;
	tnccr.raw = TNCCR(TIMER_N);
	tnccr.int_enb_cnf = 1;
	tnccr.int_type_cnf = TNCCR_INT_TYPE_EDGE;
	tnccr.type_cnf = TNCCR_TYPE_PERIODIC;
	tnccr._reserved1 = 0;
	tnccr._reserved2 = 0;
	tnccr._reserved3 = 0;
	TNCCR(TIMER_N) = tnccr.raw;

	/* コンパレータ設定 */
	unsigned long long femt_sec = us * US_TO_FS;
	cmpr_clk_counts = femt_sec / hi.counter_clk_period;

	/* LegacyReplacement Route有効化 */
	gcr.raw = GCR;
	gcr.leg_rt_cnf = 1;
	GCR = gcr.raw;
}

void ptimer_start(void)
{
	/* コンパレータ初期化 */
	union tnccr tnccr;
	tnccr.raw = TNCCR(TIMER_N);
	tnccr.val_set_cnf = 1;
	TNCCR(TIMER_N) = tnccr.raw;
	TNCR(TIMER_N) = cmpr_clk_counts;

	/* main counter初期化 */
	MCR = (unsigned long long)0;

	/* タイマー有効化 */
	union gcr gcr;
	gcr.raw = GCR;
	gcr.enable_cnf = 1;
	GCR = gcr.raw;
}

void ptimer_stop(void)
{
	/* タイマー無効化 */
	union gcr gcr;
	gcr.raw = GCR;
	gcr.enable_cnf = 0;
	GCR = gcr.raw;
}

void dump_tnccr(void)
{
	puts("TNCCR\r\n");

	unsigned char i;
	for (i = 0; i < hi.num_timers; i++) {
		union tnccr r;
		r.raw = TNCCR(i);

		putd(i, 1);
		putc(' ');
		puth(r.int_route_cap, 8);
		putc(' ');
		putd(r.fsb_int_del_cap, 1);
		putc(' ');
		putd(r.fsb_en_cnf, 1);
		putc(' ');
		puth(r.int_route_cnf, 2);
		putc(' ');
		putd(r.mode32_cnf, 1);
		putc(' ');
		putd(r.val_set_cnf, 1);
		putc(' ');
		putd(r.size_cap, 1);
		putc(' ');
		putd(r.per_int_cap, 1);
		putc(' ');
		putd(r.type_cnf, 1);
		putc(' ');
		putd(r.int_enb_cnf, 1);
		putc(' ');
		putd(r.int_type_cnf, 1);

		puts("\r\n");
	}
}

void show_timer_info(void)
{
	puts("GCR\r\n");
	union gcr gcr;
	gcr.raw = GCR;
	putd(gcr.leg_rt_cnf, 1);
	putc(' ');
	putd(gcr.enable_cnf, 1);
	puts("\r\n\r\n");

	puts("MCR\r\n");
	puth(MCR, 16);
	puts("\r\n");
	puts("WAIT\r\n");
	volatile unsigned long long wait = 100000000;
	while (wait--);
	puth(MCR, 16);
	puts("\r\n\r\n");

	dump_tnccr();
}
