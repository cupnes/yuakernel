#include <lib.h>

/* 64bit unsignedの最大値0xffffffffffffffffは
 * 10進で18446744073709551615(20桁)なので'\0'含め21文字分のバッファで足りる */
#define MAX_STR_BUF	21

void memcpy(void *dst, void *src, unsigned long long size)
{
	unsigned char *d = (unsigned char *)dst;
	unsigned char *s = (unsigned char *)src;
	for (; size > 0; size--)
		*d++ = *s++;
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

void putc(char c)
{
	syscall(SYSCALL_PUTC, c, 0, 0);
}

void puts(char *s)
{
	while (*s != '\0')
		putc(*s++);
}

void putd(unsigned long long val, unsigned char num_digits)
{
	char str[MAX_STR_BUF];

	int i;
	for (i = num_digits - 1; i >= 0; i--) {
		int digit = val % 10;
		str[i] = '0' + digit;
		val /= 10;
	}
	str[num_digits] = '\0';

	puts(str);
}

void puth(unsigned long long val, unsigned char num_digits)
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

	puts(str);
}

char getc(void)
{
	return syscall(SYSCALL_GETC, 0, 0, 0);
}

void vputc(unsigned char c)
{
	syscall(SYSCALL_VPUTC, c, 0, 0);
}

void vputs(unsigned char *s)
{
	while (*s != '\0')
		vputc(*s++);
}

void vputh(unsigned long long val, unsigned char num_digits)
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

	vputs((unsigned char *)str);
}

void set_fg(unsigned char r, unsigned char g, unsigned char b)
{
	syscall(SYSCALL_FG, r, g, b);
}

void set_bg(unsigned char r, unsigned char g, unsigned char b)
{
	syscall(SYSCALL_BG, r, g, b);
}

void clear_screen(void)
{
	syscall(SYSCALL_CLS, 0, 0, 0);
}

void vcursor_reset(void)
{
	syscall(SYSCALL_VCUR_RST, 0, 0, 0);
}

void set_kbc_handler(void *handler)
{
	syscall(SYSCALL_KBC_HDLR, (unsigned long long)handler, 0, 0);
}

struct file *open(char *file_name)
{
	return (struct file *)syscall(
		SYSCALL_OPEN, (unsigned long long)file_name, 0, 0);
}

unsigned long long get_files(struct file *files[])
{
	return syscall(SYSCALL_GET_FILES, (unsigned long long)files, 0, 0);
}

void exec(struct file *file)
{
	syscall(SYSCALL_EXEC, (unsigned long long)file, 0, 0);
}

void exec_bg(struct file *file)
{
	syscall(SYSCALL_ENQ_TASK, (unsigned long long)file, 0, 0);
}

void send_packet(void *p_data, unsigned short p_len)
{
	syscall(SYSCALL_SND_PKT, (unsigned long long)p_data, p_len, 0);
}

void move_cursor(unsigned int x, unsigned int y)
{
	syscall(SYSCALL_MOV_CUR, x, y, 0);
}

unsigned int get_cursor_y(void)
{
	return syscall(SYSCALL_GET_CUR_Y, 0, 0, 0);
}

void draw_bg(struct file *img)
{
	syscall(SYSCALL_DRAW_BG, (unsigned long long)img, 0, 0);
}

void draw_fg(struct file *img)
{
	syscall(SYSCALL_DRAW_FG, (unsigned long long)img, 0, 0);
}

void draw_image(struct image *img, unsigned int px, unsigned int py)
{
	syscall(SYSCALL_DRAW_IMG, (unsigned long long)img, px, py);
}

void get_datetime(struct datetime *dt)
{
	syscall(SYSCALL_GET_DATETIME, (unsigned long long)dt, 0, 0);
}

void sleep(unsigned long long us)
{
	syscall(SYSCALL_SLEEP, us, 0, 0);
}
