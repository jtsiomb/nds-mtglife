#include "ds3.h"
#include "dtx/drawtext.h"

extern char gmap_bignum[];

static struct dtx_font *fnt_bnum;

int init_ui(void)
{
	if(!(fnt_bnum = dtx_open_font_glyphmap_mem(gmap_bignum, 12626))) {
		return -1;
	}
	return 0;
}

void draw_ui(void)
{
}
