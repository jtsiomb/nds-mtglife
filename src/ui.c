#include "ds3.h"
#include "dtx/drawtext.h"

static void drawcb(struct dtx_vertex *v, int vcount, struct dtx_pixmap *pixmap, void *cls);

extern char gmap_bignum[];

static struct dtx_font *fnt_bnum;

int init_ui(void)
{
	if(!(fnt_bnum = dtx_open_font_glyphmap_mem(gmap_bignum, 12626))) {
		return -1;
	}
	dtx_target_user(drawcb, 0);

	return 0;
}

void draw_ui(void)
{
	ds3_matrix_mode(DS3_MODELVIEW);
	ds3_push_matrix();
	ds3_translate(0, 0, -0x30000);
	ds3_scale(0x2000, 0x2000, 0x2000);

	dtx_string("42");

	ds3_pop_matrix();
}

static void drawcb(struct dtx_vertex *v, int vcount, struct dtx_pixmap *pixmap, void *cls)
{
	int i;
	unsigned int tex, pal;
	static uint16_t cmap[256];

	if(!v || !vcount) return;

	tex = (unsigned int)pixmap->udata;
	if(!tex) {
		for(i=0; i<256; i++) {
			cmap[i] = RGB15(i >> 3, i >> 3, i >> 3);
		}
		pal = ds3_gen_palette();
		ds3_palette_data(pal, 256, cmap);

		tex = ds3_gen_texture();
		ds3_tex_parameter(tex, DS3_TEX_PALETTE, pal);
		ds3_tex_image(tex, DS3_INDEXED8, pixmap->width, pixmap->height,
				pixmap->pixels);
		pixmap->udata = (void*)tex;
	}

	ds3_enable(DS3_TEXTURE_2D);
	ds3_bind_texture(tex);

	ds3_begin(DS3_TRIANGLES);
	ds3_color3b(255, 255, 255);
	for(i=0; i<vcount; i++) {
		ds3_texcoord2f(v->s, v->t);
		ds3_vertex3f(v->x, v->y, 0.0f);
		++v;
	}
	ds3_end();

	ds3_disable(DS3_TEXTURE_2D);
}
