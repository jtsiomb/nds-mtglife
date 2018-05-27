#ifndef DS_H_
#define DS_H_

#include "dsregs.h"

/* ds_init flags */
enum {
	DS_INIT_2D	= 1,
	DS_INIT_3D	= 2,
	DS_INIT_AUDIO = 4
};

enum { DS_A, DS_B };

/* disp_mode options */
enum {
	DS_DISP_OFF		= 0,
	DS_DISP_GFX		= 1,
	DS_DISP_VRAM_FB	= 2,
	DS_DISP_DMA_FB	= 3
};

/* bg0_mode options */
enum { DS_BG0_2D, DS_BG0_3D };

/* VRAM banks */
enum {
	DS_VRAM_A_128,
	DS_VRAM_B_128,
	DS_VRAM_C_128,
	DS_VRAM_D_128,
	DS_VRAM_E_64,
	DS_VRAM_F_16,
	DS_VRAM_G_16,
	DS_VRAM_H_32,
	DS_VRAM_I_16
};

/* VRAM mapping usage */
enum {
	DS_VRAM_USE_LCDC,
	DS_VRAM_USE_A_BG,
	DS_VRAM_USE_A_OBJ,
	DS_VRAM_USE_A_BGPAL,
	DS_VRAM_USE_A_OBJPAL,
	DS_VRAM_USE_TEXIMG,
	DS_VRAM_USE_TEXPAL,
	DS_VRAM_USE_B_BG,
	DS_VRAM_USE_B_OBJ,
	DS_VRAM_USE_B_BGPAL,
	DS_VRAM_USE_B_OBJPAL,
	DS_VRAM_USE_ARM7
};

void ds_init(unsigned int flags);
void ds_disp_swap(void);

void ds_disp_mode(int eng, int dispmode);
void ds_bg_mode(int eng, int bgmode);
void ds_enable_bg(int eng, int bg);
void ds_disable_bg(int eng, int bg);
void ds_bg0_mode(int eng, int bg0mode);

/* VRAM bank mapping */
void *ds_vram_map(int bank, int usage, int offs);
void ds_vram_unmap(int bank);

#define ds_wait_vsync()		while(REG_VCOUNT < 192)

/* debugging */
void ds_panic(const char *fmt, ...);

#endif	/* DS_H_ */
