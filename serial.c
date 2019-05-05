#include <x86.h>
#include <serial.h>

/* 64bit unsignedの最大値0xffffffffffffffffは
 * 10進で18446744073709551615(20桁)なので'\0'含め21文字分のバッファで足りる */
#define MAX_STR_BUF	21

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

void ser_puts(char *s)
{
	while (*s != '\0')
		ser_putc_poll(*s++);
}

void ser_puth(unsigned long long val, unsigned char num_digits)
{
	char str[MAX_STR_BUF];

	int i;
	for (i = num_digits - 1; i >= 0; i--) {
		unsigned char v = (unsigned char)(val & 0x0f);
		if (v < 0xa)
			str[i] = '0' + v;
		else
			str[i] = 'A' + (v - 0xa);
		val >>= 4;
	}
	str[num_digits] = '\0';

	ser_puts(str);
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
