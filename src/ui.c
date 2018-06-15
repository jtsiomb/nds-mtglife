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
	dtx_string("42");
}

static void drawcb(struct dtx_vertex *v, int vcount, struct dtx_pixmap *pixmap, void *cls)
{
	int i;
	unsigned int tex;

	tex = (unsigned int)pixmap->udata;
	if(!tex) {
		tex = ds3_gen_texture();
		ds3_tex_image(tex, DS3_RGB5_A1, pixmap->width, pixmap->height,
				pixmap->pixels);
		pixmap->udata = (void*)tex;
	}

	ds3_enable(DS3_TEXTURE_2D);
	ds3_bind_texture(tex);

	ds3_begin(DS3_TRIANGLES);
	ds3_color3b(255, 255, 255);
	for(i=0; i<vcount; i++) {
		ds3_texcoord2f(v->s, v->t);
		ds3_vertex2f(v->x, v->y);
		++v;
	}
	ds3_end();

	ds3_disable(DS3_TEXTURE_2D);
}
