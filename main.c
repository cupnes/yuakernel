#include <x86.h>
#include <intr.h>
#include <pic.h>
#include <acpi.h>
#include <syscall.h>
#include <fb.h>
#include <kbc.h>
#include <fbcon.h>
#include <fs.h>
#include <hpet.h>
#include <sched.h>
#include <common.h>
#include <pci.h>
#include <network.h>
#include <nic.h>
#include <cmos.h>

void debug_dump_address_translation(unsigned long long linear_address);

struct __attribute__((packed)) platform_info {
	struct framebuffer fb;
	void *rsdp;
};

#define INIT_APP	"init"

/* begin: serial.h */
#define COM1_BASE	0x03f8
#define COM2_BASE	0x02f8
#define COM3_BASE	0x03e8
#define COM4_BASE	0x02e8

#define SER_REG_THR(base)	((base) + 0) /* Transmitter Holding Buffer */
#define SER_REG_RBR(base)	((base) + 0) /* Receiver Buffer */
#define SER_REG_DLL(base)	((base) + 0) /* Divisor Latch Low Byte */
#define SER_REG_IER(base)	((base) + 1) /* Interrupt Enable Register */
#define SER_REG_DLH(base)	((base) + 1) /* Divisor Latch High Byte */
#define SER_REG_IIR(base)	((base) + 2)
					/* Interrupt Identification Register */
#define SER_REG_FCR(base)	((base) + 2) /* FIFO Control Register */
#define SER_REG_LCR(base)	((base) + 3) /* Line Control Register */
#define SER_REG_MCR(base)	((base) + 4) /* Modem Control Register */
#define SER_REG_LSR(base)	((base) + 5) /* Line Status Register */
#define SER_REG_MSR(base)	((base) + 6) /* Modem Status Register */
#define SER_REG_SR(base)	((base) + 7) /* Scratch Register */

#define SER_FCR_TL16B_1B	(0b00 << 6)
#define SER_FCR_TL16B_4B	(0b01 << 6)
#define SER_FCR_TL16B_8B	(0b10 << 6)
#define SER_FCR_TL16B_14B	(0b11 << 6)
#define SER_FCR_TL64B_1B	(0b00 << 6)
#define SER_FCR_TL64B_16B	(0b01 << 6)
#define SER_FCR_TL64B_32B	(0b10 << 6)
#define SER_FCR_TL64B_56B	(0b11 << 6)
#define SER_FCR_EN_64B_FIFO	(1U << 5)
#define SER_FCR_DMA_MOD_SEL	(1U << 3)
#define SER_FCR_CLR_TX_FIFO	(1U << 2)
#define SER_FCR_CLR_RX_FIFO	(1U << 1)
#define SER_FCR_EN_FIFO	(1U << 0)

#define SER_LSR_ERR_RXF	(1U << 7)
#define SER_LSR_EMP_RXR	(1U << 6)
#define SER_LSR_EMP_TXR	(1U << 5)
#define SER_LSR_BRK_INT	(1U << 4)
#define SER_LSR_FRA_ERR	(1U << 3)
#define SER_LSR_PAR_ERR	(1U << 2)
#define SER_LSR_OVR_ERR	(1U << 1)
#define SER_LSR_DAT_RDY	(1U << 0)
/* end: serial.h */

/* begin: serial.c */
unsigned short port;
/* end: serial.c */

void start_kernel(void *_t __attribute__((unused)), struct platform_info *pi,
		  void *_fs_start)
{
	/* フレームバッファ周りの初期化 */
	fb_init(&pi->fb);
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

	/* ACPIの初期化 */
	acpi_init(pi->rsdp);

	/* CPU周りの初期化 */
	gdt_init();
	intr_init();

	/* 周辺ICの初期化 */
	/* begin: serial.c */
	port = COM1_BASE;
	io_write(SER_REG_IER(port), 0x00);
	io_write(SER_REG_FCR(port),
		 SER_FCR_TL16B_1B | SER_FCR_CLR_TX_FIFO | SER_FCR_CLR_RX_FIFO
		 | SER_FCR_EN_FIFO);
	while (1) {
		while (!(io_read(SER_REG_LSR(port)) & SER_LSR_DAT_RDY));
		unsigned char c = io_read(SER_REG_RBR(port));
		while (!(io_read(SER_REG_LSR(port)) & SER_LSR_EMP_TXR));
		io_write(SER_REG_THR(port), c);
	}
	/* end: serial.c */

	pic_init();
	hpet_init();
	kbc_init();
	i218v_init();

	/* システムコールの初期化 */
	syscall_init();

	/* ファイルシステムの初期化 */
	fs_init(_fs_start);

	/* スケジューラの初期化 */
	sched_init();

	/* CPUの割り込み有効化 */
	enable_cpu_intr();

	/* スケジューラの開始 */
	sched_start();

	/* initアプリ起動 */
	exec(open(INIT_APP));

	/* haltして待つ */
	while (1)
		cpu_halt();
}
