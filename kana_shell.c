#include <fbcon.h>
#include <font.h>
#include <kbc.h>

#define MAX_COM_LEN	128
#define KS_UNDEFINED	-1

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
#define SPC	FONT_hira_space

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

	[' '] = FONT_hira_space,
	['\r'] = '\r',
	['\n'] = '\n'
};

static void cmd_undefined(void)
{
	unsigned char s[] = {WA, KA, RA, N, '\r', '\n', '\0'};
	vputs(s);
}

static int select_command(unsigned char c)
{
	return KS_UNDEFINED;
}

static void exec_command(int com_id)
{
	if (com_id < 0)
		cmd_undefined();
}

static void kbc_handler(char c)
{
	static int com_id;
	if (c == '\n') {
		vputs((unsigned char *)"\r\n");
		exec_command(com_id);
	} else {
		unsigned char _c = asc2kana[(unsigned char)c];
		vputc(_c);
		com_id = select_command(_c);
	}
}

void kana_main(void)
{
	kbc_set_handler(kbc_handler);
	vcursor_reset();
}
