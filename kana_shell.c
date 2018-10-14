#include <fb.h>
#include <fbcon.h>
#include <font.h>
#include <kbc.h>
#include <sched.h>
#include <common.h>

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

enum KS_INPUT_STATE {
	COMMAND_INPUT_MODE,
	PARAMETER_INPUT_MODE
};

static void cmd_undefined(unsigned char *param __attribute__((unused)))
{
	unsigned char s[] = {WA, KA, RA, N, '\r', '\n', '\0'};
	vputs(s);
}

#define CMD_KAKE_IDX	((unsigned char)(KA + KE))
void cmd_kake(unsigned char *param)
{
	vputs(param);
	vputs((unsigned char *)"\r\n");
}

#define MAX_CMD_OPS	256
void (*cmd_ops[MAX_CMD_OPS])(unsigned char *param) = {NULL};

void cmd_ops_init(void)
{
	unsigned short i;
	for (i = 0; i < MAX_CMD_OPS; i++)
		cmd_ops[i] = cmd_undefined;

	cmd_ops[CMD_KAKE_IDX] = cmd_kake;
}

#define PARAMETER_BUF_LEN	32
static void kbc_handler(char c)
{
	vputc(asc2kana[(unsigned char)c]);

	static unsigned char input_state = COMMAND_INPUT_MODE;
	static unsigned char command_id = 0;
	static unsigned char param_buf[PARAMETER_BUF_LEN];
	static unsigned char param_buf_idx = 0;

	switch (c) {
	case '\n':
		vputc('\r');
		if (command_id || input_state != COMMAND_INPUT_MODE) {
			param_buf[param_buf_idx] = '\0';
			cmd_ops[command_id](param_buf);
		}
		vputc(YEN);
		vputc(SPC);

		input_state = COMMAND_INPUT_MODE;
		command_id = param_buf_idx = 0;
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
			command_id += asc2kana[(unsigned char)c];
			break;

		case PARAMETER_INPUT_MODE:
			param_buf[param_buf_idx++] = asc2kana[(unsigned char)c];
			break;
		}
	}
}

void kana_main(void)
{
	cmd_ops_init();
	kbc_set_handler(kbc_handler);
	set_fg(0, 0, 0);
	set_bg(255, 255, 255);
	clear_screen();
	vcursor_reset();
	vputc(YEN);
	vputc(SPC);
}
