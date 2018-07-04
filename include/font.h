#ifndef _FONT_H_
#define _FONT_H_

#define FONT_WIDTH		8
#define FONT_HEIGHT		10
#define FONT_HIRA_WIDTH		16
#define FONT_HIRA_HEIGHT	21
/* #define FONT_HIRA_WIDTH		24 */
/* #define FONT_HIRA_HEIGHT	32 */

enum FONT_HIRA_IDX {
	FONT_hira_space = 128,
	FONT_hira_a,
	FONT_hira_chi,
	FONT_hira_e,
	FONT_hira_fu,
	FONT_hira_ha,
	FONT_hira_he,
	FONT_hira_hi,
	FONT_hira_ho,
	FONT_hira_i,
	FONT_hira_ka,
	FONT_hira_ke,
	FONT_hira_ki,
	FONT_hira_ko,
	FONT_hira_ku,
	FONT_hira_ma,
	FONT_hira_me,
	FONT_hira_mi,
	FONT_hira_mo,
	FONT_hira_mu,
	FONT_hira_n,
	FONT_hira_na,
	FONT_hira_ne,
	FONT_hira_ni,
	FONT_hira_no,
	FONT_hira_nu,
	FONT_hira_o,
	FONT_hira_ra,
	FONT_hira_re,
	FONT_hira_ri,
	FONT_hira_ro,
	FONT_hira_ru,
	FONT_hira_sa,
	FONT_hira_se,
	FONT_hira_shi,
	FONT_hira_so,
	FONT_hira_su,
	FONT_hira_ta,
	FONT_hira_te,
	FONT_hira_to,
	FONT_hira_tsu,
	FONT_hira_u,
	FONT_hira_wa,
	FONT_hira_wo,
	FONT_hira_ya,
	FONT_hira_yo,
	FONT_hira_yu,
};

extern const unsigned char font_bitmap[][FONT_HEIGHT][FONT_WIDTH];
extern const unsigned char font_hira_bitmap[][FONT_HIRA_HEIGHT][FONT_HIRA_WIDTH];

#endif
