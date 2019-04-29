#pragma once

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

void ser_init(void);
char ser_getc_poll(void);
void ser_putc_poll(char c);
void ser_debug_echoback(void);
