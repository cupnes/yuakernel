#include <lib.h>

#define NU	FONT_hira_nu
#define FU	FONT_hira_fu
#define A	FONT_hira_a
#define U	FONT_hira_u
#define E	FONT_hira_e
#define O	FONT_hira_o
#define YA	FONT_hira_ya
#define YU	FONT_hira_yu
#define YO	FONT_hira_yo
#define WA	FONT_hira_wa
#define HO	FONT_hira_ho
#define HE	FONT_hira_he
#define TA	FONT_hira_ta
#define TE	FONT_hira_te
#define I	FONT_hira_i
#define SU	FONT_hira_su
#define KA	FONT_hira_ka
#define N	FONT_hira_n
#define NA	FONT_hira_na
#define NI	FONT_hira_ni
#define RA	FONT_hira_ra
#define SE	FONT_hira_se
#define CHI	FONT_hira_chi
#define TO	FONT_hira_to
#define SHI	FONT_hira_shi
#define HA	FONT_hira_ha
#define KI	FONT_hira_ki
#define KU	FONT_hira_ku
#define MA	FONT_hira_ma
#define NO	FONT_hira_no
#define RI	FONT_hira_ri
#define RE	FONT_hira_re
#define KE	FONT_hira_ke
#define MU	FONT_hira_mu
#define TSU	FONT_hira_tsu
#define SA	FONT_hira_sa
#define SO	FONT_hira_so
#define HI	FONT_hira_hi
#define KO	FONT_hira_ko
#define MI	FONT_hira_mi
#define MO	FONT_hira_mo
#define NE	FONT_hira_ne
#define RU	FONT_hira_ru
#define ME	FONT_hira_me
#define RO	FONT_hira_ro
#define SPC	FONT_ascii_20_spc
#define YEN	FONT_ascii_5C_yen
#define DAKU	FONT_x_dakuten
#define HANDAKU	FONT_x_handakuten

unsigned char asc2kana[] = {
	['1'] = FONT_hira_nu,
	['2'] = FONT_hira_fu,
	['3'] = FONT_hira_a,
	['4'] = FONT_hira_u,
	['5'] = FONT_hira_e,
	['6'] = FONT_hira_o,
	['7'] = FONT_hira_ya,
	['8'] = FONT_hira_yu,
	['9'] = FONT_hira_yo,
	['0'] = FONT_hira_wa,
	['-'] = FONT_hira_ho,
	['^'] = FONT_hira_he,

	['q'] = FONT_hira_ta,
	['w'] = FONT_hira_te,
	['e'] = FONT_hira_i,
	['r'] = FONT_hira_su,
	['t'] = FONT_hira_ka,
	['y'] = FONT_hira_n,
	['u'] = FONT_hira_na,
	['i'] = FONT_hira_ni,
	['o'] = FONT_hira_ra,
	['p'] = FONT_hira_se,
	['@'] = FONT_x_dakuten,
	['['] = FONT_x_handakuten,

	['a'] = FONT_hira_chi,
	['s'] = FONT_hira_to,
	['d'] = FONT_hira_shi,
	['f'] = FONT_hira_ha,
	['g'] = FONT_hira_ki,
	['h'] = FONT_hira_ku,
	['j'] = FONT_hira_ma,
	['k'] = FONT_hira_no,
	['l'] = FONT_hira_ri,
	[';'] = FONT_hira_re,
	[':'] = FONT_hira_ke,
	[']'] = FONT_hira_mu,

	['z'] = FONT_hira_tsu,
	['x'] = FONT_hira_sa,
	['c'] = FONT_hira_so,
	['v'] = FONT_hira_hi,
	['b'] = FONT_hira_ko,
	['n'] = FONT_hira_mi,
	['m'] = FONT_hira_mo,
	[','] = FONT_hira_ne,
	['.'] = FONT_hira_ru,
	['/'] = FONT_hira_me,
	[0x5f] = FONT_hira_ro,

	[' '] = FONT_ascii_20_spc,
	['\r'] = '\r',
	['\n'] = '\n'
};

static void kbc_handler(unsigned char c);

int main(void)
{
	set_kbc_handler(kbc_handler);
	set_fg(0, 0, 0);
	set_bg(255, 255, 255);
	clear_screen();
	vcursor_reset();
	vputc(YEN);
	vputc(SPC);

	return 0;
}

enum KS_INPUT_STATE {
	COMMAND_INPUT_MODE,
	PARAMETER_INPUT_MODE
};

static void cmd_undefined(unsigned char *param __attribute__((unused)))
{
	unsigned char s[] = {WA, KA, RA, N, '\r', '\n', '\0'};
	vputs(s);
}

#define CMD_KAKE_IDX	((KA << 8U) + KE)
void cmd_kake(unsigned char *param)
{
	vputs(param);
	vputs((unsigned char *)"\r\n");
}

/* #define CMD_SHIKEN_IDX	((SHI << 16U) + (KE << 8U) + N) */
#define CMD_SHIKEN_IDX	(TE)
void cmd_shiken(unsigned char *param __attribute__((unused)))
{
	exec(open("test"));
}

#define CMD_ICHIRAN_IDX	(((I << 24U) + (CHI << 16U) + (RA << 8U) + N)	\
			 & 0x00000000ffffffff)
#define MAX_FILES	100
void cmd_ichiran(void)
{
	struct file *f[MAX_FILES];
	unsigned long long num_files = get_files(f);
	unsigned long long i;
	for (i = 0; i < num_files; i++) {
		vputs((unsigned char *)f[i]->name);
		vputc(' ');
	}
	vputs((unsigned char *)"\r\n");
}

#define CMD_HIYOUSHI_IDX	(((HI << 24U) + (YO << 16U) + (U << 8U) + SHI)	\
				 & 0x00000000ffffffff)
void cmd_hiyoushi(void)
{
	unsigned char title[] = {
		SHI, SU, TE, MU, KO, '|', RU, NO, U, SU, I, HO, N,
		'\r', '\n', '\0'};
	vputs(title);

	vputs((unsigned char *)"\r\n");

	unsigned char auther[] = {
		O, O, DAKU, KA, MI, ' ', YU, U, MA, ' ', CHI, YO,
		'\r', '\n', '\0'};
	vputs(auther);

	vputs((unsigned char *)"\r\n");

	/* unsigned char date[] = { */
	/* 	'2', '0', '1', '8', NE, N, */
	/* 	'1', '2', DAKU, KA, TSU, */
	/* 	'3', '0', NI, CHI, '\r', '\n', '\0'}; */
	unsigned char date[] = {
		'2', '0', '1', '8', '.',
		'1', '2', '.',
		'3', '0', '\r', '\n', '\0'};
	vputs(date);

	vputs((unsigned char *)"\r\n");

	unsigned char circle[] = {
		HE, NI, YA, HANDAKU, HE, N, TE, '\r', '\n', '\0'};
	vputs(circle);

	vputs((unsigned char *)"\r\n");
}

void cmd_exec(unsigned long long cmd_id, unsigned char *param)
{
	switch (cmd_id) {
	case CMD_KAKE_IDX:
		cmd_kake(param);
		break;

	case CMD_SHIKEN_IDX:
		cmd_shiken(param);
		break;

	case CMD_ICHIRAN_IDX:
		cmd_ichiran();
		break;

	case CMD_HIYOUSHI_IDX:
		cmd_hiyoushi();
		break;

	default:
		cmd_undefined(param);
	}
}

#define KBC_HANKAKU_ZENKAKU	0xff
unsigned char is_hira = 1;

unsigned char parse_ch(unsigned char c)
{
	if (is_hira)
		c = asc2kana[c];
	return c;
}

#define PARAMETER_BUF_LEN	32
static void kbc_handler(unsigned char c)
{
	if (c == KBC_HANKAKU_ZENKAKU) {
		is_hira = !is_hira;
		return;
	}

	vputc(parse_ch(c));

	static unsigned char input_state = COMMAND_INPUT_MODE;
	static unsigned long long command_id = 0;
	static unsigned char param_buf[PARAMETER_BUF_LEN];
	static unsigned char param_buf_idx = 0;
	static unsigned char cmd_chr_counter = 0;

	switch (c) {
	case '\n':
		vputc('\r');
		if (command_id || input_state != COMMAND_INPUT_MODE) {
			param_buf[param_buf_idx] = '\0';
			cmd_exec(command_id, param_buf);
		}
		vputc(YEN);
		vputc(SPC);

		input_state = COMMAND_INPUT_MODE;
		command_id = param_buf_idx = cmd_chr_counter = 0;
		break;

	case ' ':
		if (input_state == COMMAND_INPUT_MODE)
			input_state = PARAMETER_INPUT_MODE;
		else
			param_buf[param_buf_idx++] = SPC;
		break;

	default:
		switch (input_state) {
		case COMMAND_INPUT_MODE:
			if (cmd_chr_counter < 8) {
				command_id = (command_id << 8U) + parse_ch(c);
				cmd_chr_counter++;
			}
			break;

		case PARAMETER_INPUT_MODE:
			param_buf[param_buf_idx++] = parse_ch(c);
			break;
		}
	}
}
