#include <stdint.h>
#include "dsregs.h"
#include "ds.h"
#include "ds3.h"

static void xorpat(void *addr, int xsz, int ysz);

int main(void)
{
	int tex;
	uint16_t *bgmem;

	ds_init(DS_INIT_2D | DS_INIT_3D);

	ds_disp_mode(DS_A, DS_DISP_GFX);
	ds_bg_mode(DS_A, 0);
	ds_enable_bg(DS_A, 0);
	ds_bg0_mode(DS_A, DS_BG0_3D);

	ds_disp_mode(DS_B, DS_DISP_GFX);
	ds_bg_mode(DS_B, 5);
	ds_enable_bg(DS_B, 2);
	ds_bg0_mode(DS_B, DS_BG0_2D);

	REG_B_BG2CNT = BGXCNT_BM_256X256 | BGXCNT_BM16 | BGXCNT_OVF_WRAP;
	REG_B_BG2PA = 0x100;
	REG_B_BG2PB = 0;
	REG_B_BG2PC = 0;
	REG_B_BG2PD = 0x100;

	ds_vram_map(DS_VRAM_A_128, DS_VRAM_USE_LCDC, 0);
	bgmem = ds_vram_map(DS_VRAM_C_128, DS_VRAM_USE_B_BG, 0);
	ds3_add_texmem(DS_VRAM_B_128);
	ds3_add_texmem(DS_VRAM_D_128);

	xorpat(bgmem, 256, 256);

	tex = ds3_gen_texture();
	ds3_tex_image(tex, DS3_RGB, 256, 256, bgmem);

	ds3_clear_color(RGB15(0, 0, 0), 31);
	ds3_clear_depth(0x7fff);
	ds3_viewport(0, 0, 256, 192);

	ds3_enable(DS3_POLYGON_SMOOTH);

	REG_POLYGON_ATTR = 0x001f0080;	/* alpha = 31, cull back */

	ds3_matrix_mode(DS3_PROJECTION);
	ds3_load_identity();
	ds3_perspectivef(45, 1.33333, 1.0, 100.0);

	for(;;) {
		ds3_matrix_mode(DS3_MODELVIEW);
		ds3_load_identity();
		ds3_translate(0, 0, -0x30000);

		ds3_enable(DS3_TEXTURE_2D);
		ds3_bind_texture(tex);

		ds3_begin(DS3_QUADS);
		ds3_color3b(255, 255, 255);
		ds3_texcoord2(0, 0);
		/*ds3_color3b(255, 0, 0);*/
		ds3_vertex2(-0x8000, -0x8000);
		ds3_texcoord2(0x8000, 0);
		/*ds3_color3b(0, 255, 0);*/
		ds3_vertex2(0x8000, -0x8000);
		ds3_texcoord2(0x8000, 0x8000);
		/*ds3_color3b(0, 0, 255);*/
		ds3_vertex2(0x8000, 0x8000);
		ds3_texcoord2(0, 0x8000);
		/*ds3_color3b(255, 255, 0);*/
		ds3_vertex2(-0x8000, 0x8000);
		ds3_end();

		ds3_disable(DS3_TEXTURE_2D);

		ds3_swap_buffers();
		ds_wait_vsync();
	}
	return 0;
}

static void xorpat(void *addr, int xsz, int ysz)
{
	int i, j;
	uint16_t *p = addr;

	for(i=0; i<ysz; i++) {
		for(j=0; j<xsz; j++) {
			int xor = i^j;
			uint16_t red = xor >> 2;
			uint16_t green = xor >> 1;
			uint16_t blue = xor;
			*p++ = 0x8000 | red | ((green & 0x1f) << 5) | ((blue & 0x1f) << 10);
		}
	}
}
