#include <lib.h>

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

static void kbc_handler(char c);

int main(void)
{
	set_kbc_handler(kbc_handler);

	while (1);

	return 0;
}

static void kbc_handler(char c)
{
	vputc(asc2kana[(unsigned int)c]);
	if (c == '\n')
		vputc('\r');
}
