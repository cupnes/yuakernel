#include <x86.h>
#include <serial.h>

unsigned short port;

void ser_init(void)
{
	port = COM1_BASE;

	io_write(SER_REG_IER(port), 0x00);

	io_write(SER_REG_FCR(port),
		 SER_FCR_TL16B_1B | SER_FCR_CLR_TX_FIFO | SER_FCR_CLR_RX_FIFO
		 | SER_FCR_EN_FIFO);
}

char ser_getc_poll(void)
{
	while (!(io_read(SER_REG_LSR(port)) & SER_LSR_DAT_RDY));
	return io_read(SER_REG_RBR(port));
}

void ser_putc_poll(char c)
{
	while (!(io_read(SER_REG_LSR(port)) & SER_LSR_EMP_TXR));
	io_write(SER_REG_THR(port), c);
}

void ser_debug_echoback(void)
{
	while (1) {
		char c = ser_getc_poll();
		if (c == '\r')
			ser_putc_poll('\n');
		ser_putc_poll(c);
	}
}
