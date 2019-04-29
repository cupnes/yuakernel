#include <lib.h>

#ifdef RUN_LOCAL
#include <stdio.h>
#include <local_conf.h>
#endif

/* 64bit unsignedの最大値0xffffffffffffffffは
 * 10進で18446744073709551615(20桁)なので'\0'含め21文字分のバッファで足りる */
#define MAX_STR_BUF	21

#ifndef RUN_LOCAL
int strcmp(char *s1, char *s2)
{
	char is_equal = 1;

	for (; (*s1 != '\0') && (*s2 != '\0'); s1++, s2++) {
		if (*s1 != *s2) {
			is_equal = 0;
			break;
		}
	}

	if (is_equal) {
		if (*s1 != '\0') {
			return 1;
		} else if (*s2 != '\0') {
			return -1;
		} else {
			return 0;
		}
	} else {
		return (int)(*s1 - *s2);
	}
}

int strncmp(char *s1, char *s2, unsigned long long n)
{
	char is_equal = 1;

	for (; (*s1 != '\0') && (*s2 != '\0'); s1++, s2++, n--) {
		if (*s1 != *s2 || n == 1) {
			is_equal = 0;
			break;
		}
	}

	if (is_equal) {
		if (*s1 != '\0') {
			return 1;
		} else if (*s2 != '\0') {
			return -1;
		} else {
			return 0;
		}
	} else {
		return (int)(*s1 - *s2);
	}
}

void memcpy(void *dst, void *src, unsigned long long size)
{
	unsigned char *d = (unsigned char *)dst;
	unsigned char *s = (unsigned char *)src;
	for (; size > 0; size--)
		*d++ = *s++;
}
#endif

unsigned long long syscall(
	unsigned long long syscall_id __attribute__((unused)),
	unsigned long long arg1 __attribute__((unused)),
	unsigned long long arg2 __attribute__((unused)),
	unsigned long long arg3 __attribute__((unused)))
{
#ifndef RUN_LOCAL
	unsigned long long ret_val;

	asm volatile ("int $0x80\n"
		      "movq %%rax, %[ret_val]"
		      : [ret_val]"=r"(ret_val) :);

	return ret_val;
#else
	return 0;
#endif
}

#ifndef RUN_LOCAL
void putchar(char c)
{
	syscall(SYSCALL_PUTC, c, 0, 0);
}

void puts(char *s)
{
	while (*s != '\0')
		putchar(*s++);
}
#endif

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

#ifndef RUN_LOCAL
	puts(str);
#else
	printf("%s", str);
#endif
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

#ifndef RUN_LOCAL
	puts(str);
#else
	printf("%s", str);
#endif
}

#ifndef RUN_LOCAL
char getchar(void)
{
	return syscall(SYSCALL_GETC, 0, 0, 0);
}
#endif

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

int exec_bg(struct file *file)
{
	return (int)syscall(SYSCALL_ENQ_TASK, (unsigned long long)file, 0, 0);
}

void receive_packet(void *p_data, unsigned short *p_len)
{
	syscall(SYSCALL_RCV_PKT, (unsigned long long)p_data,
		(unsigned long long)p_len, 0);
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

void get_px(unsigned int x, unsigned int y, struct pixelformat *val)
{
	syscall(SYSCALL_GET_PX, x, y, (unsigned long long)val);
}

void draw_px_fg(unsigned int x, unsigned int y)
{
	syscall(SYSCALL_DRAW_PX_FG, x, y, 0);
}

void draw_px_bg(unsigned int x, unsigned int y)
{
	syscall(SYSCALL_DRAW_PX_BG, x, y, 0);
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

void finish_task(int task_id)
{
	syscall(SYSCALL_FINISH_TASK, task_id, 0, 0);
}

void get_mac(unsigned char *mac)
{
#ifndef RUN_LOCAL
	syscall(SYSCALL_GET_MAC, (unsigned long long)mac, 0, 0);
#else
	mac[0] = OWN_MAC_0;
	mac[1] = OWN_MAC_1;
	mac[2] = OWN_MAC_2;
	mac[3] = OWN_MAC_3;
	mac[4] = OWN_MAC_4;
	mac[5] = OWN_MAC_5;
#endif
}

void nic_rx_enable(void)
{
	syscall(SYSCALL_NIC_RX_EN, 0, 0, 0);
}

#ifndef RUN_LOCAL
#define LGCS_A	5
#define LGCS_B	3
#define LGCS_M	32768

static unsigned short lgcs_x;

void set_seed(unsigned short x_0)
{
	lgcs_x = x_0;
}

unsigned short rand(void)
{
	lgcs_x = ((LGCS_A * lgcs_x) + LGCS_B) & (LGCS_M - 1);
	return lgcs_x;
}
#endif

char ser_getc(void)
{
	return syscall(SYSCALL_SER_GETC, 0, 0, 0);
}

void ser_putc(char c)
{
	syscall(SYSCALL_SER_PUTC, c, 0, 0);
}
