#include "ds.h"
#include "dsregs.h"

void ds_init(unsigned int flags)
{
	uint32_t pwr = POWCNT1_LCD | POWCNT1_DSWAP;	/* by default make top be disp A */

	if(flags & DS_INIT_2D) {
		pwr |= POWCNT1_2DA | POWCNT1_2DB;
	}
	if(flags & DS_INIT_3D) {
		pwr |= POWCNT1_3DREND | POWCNT1_3DGEOM;
	}
	REG_POWCNT1 = pwr;

	if(flags & DS_INIT_AUDIO) {
		REG_POWCNT2 = POWCNT2_SOUND;
	}
}

void ds_disp_swap(void)
{
	REG_POWCNT1 ^= POWCNT1_DSWAP;
}

void ds_disp_mode(int eng, int dispmode)
{
	if(eng == 0) {
		REG_DISPCNT = (REG_DISPCNT & ~DISPCNT_MODE_MASK) | DISPCNT_MODE(dispmode);
	} else {
		REG_B_DISPCNT = (REG_B_DISPCNT & ~DISPCNT_MODE_MASK) | DISPCNT_MODE(dispmode);
	}
}

void ds_bg_mode(int eng, int bgmode)
{
	if(eng == 0) {
		REG_DISPCNT = (REG_DISPCNT & ~DISPCNT_BGMODE_MASK) | DISPCNT_BGMODE(bgmode);
	} else {
		REG_B_DISPCNT = (REG_B_DISPCNT & ~DISPCNT_BGMODE_MASK) | DISPCNT_BGMODE(bgmode);
	}
}

void ds_enable_bg(int eng, int bg)
{
	if(eng == 0) {
		REG_DISPCNT |= DISPCNT_BG0 << bg;
	} else {
		REG_B_DISPCNT |= DISPCNT_BG0 << bg;
	}
}

void ds_disable_bg(int eng, int bg)
{
	if(eng == 0) {
		REG_DISPCNT &= ~(DISPCNT_BG0 << bg);
	} else {
		REG_B_DISPCNT &= ~(DISPCNT_BG0 << bg);
	}
}

void ds_bg0_mode(int eng, int bg0mode)
{
	volatile int32_t *reg = eng == 0 ? &REG_DISPCNT : &REG_B_DISPCNT;

	if(bg0mode == DS_BG0_3D) {
		*reg |= DISPCNT_BG0_3D;
	} else {
		*reg &= ~DISPCNT_BG0_3D;
	}
}


static volatile uint8_t *bankreg[] = {
	&REG_VRAMCNT_A, &REG_VRAMCNT_B, &REG_VRAMCNT_C, &REG_VRAMCNT_D,
	&REG_VRAMCNT_E, &REG_VRAMCNT_F, &REG_VRAMCNT_G, &REG_VRAMCNT_H, &REG_VRAMCNT_I
};

void *ds_vram_map(int bank, int usage, int offs)
{
	static uint32_t lcdc_addr[] = {
		0x6800000, 0x6820000, 0x6840000, 0x6860000,
		0x6880000, 0x6890000, 0x6894000, 0x6898000, 0x68a0000
	};
	static const char *vram_usage_str[] = {
		"LCDC",
		"engine A BG",
		"engine A OBJ",
		"engine A BG palette",
		"engine A OBJ palette",
		"texture image",
		"texture palette",
		"engine B BG",
		"engine B OBJ",
		"engine B BG palette",
		"engine B OBJ palette",
		"ARM7"
	};

	int mst;
	void *ptr;

	if(offs > 3 || offs < 0) {
		ds_panic("ds_vram_map: invalid offset: %d\n", offs);
	}

	switch(usage) {
	case DS_VRAM_USE_LCDC:
		*bankreg[bank] = VRAM_ENABLE;
		return (void*)lcdc_addr[bank];

	case DS_VRAM_USE_A_BG:
		if(bank >= DS_VRAM_H_32) {
			goto inval_use;
		}
		*bankreg[bank] = VRAM_ENABLE | VRAM_OFFSET(offs) | 1;
		if(bank <= DS_VRAM_D_128) {
			return (void*)(0x6800000 + 0x20000 * offs);
		} else if(bank >= DS_VRAM_F_16) {
			return (void*)(0x6800000 + 0x4000 * (offs & 1) + 0x10000 * (offs >> 1));
		}
		return (void*)0x6800000;	/* bank E */

	case DS_VRAM_USE_A_OBJ:
		if(bank == DS_VRAM_C_128 || bank == DS_VRAM_D_128 || bank >= DS_VRAM_H_32) {
			goto inval_use;
		}
		*bankreg[bank] = VRAM_ENABLE | VRAM_OFFSET(offs) | 2;
		if(bank <= DS_VRAM_B_128) {
			if(offs > 1) {
				ds_panic("ds_vram_map: bank %c, use A OBJ, invalid offset: %d\n", 'A' + bank, offs);
			}
			return (void*)(0x6400000 + 0x20000 * offs);
		} else if(bank >= DS_VRAM_F_16) {
			return (void*)(0x6400000 + 0x4000 * (offs & 1) + 0x10000 * (offs >> 1));
		}
		return (void*)0x6400000;	/* bank E */

	case DS_VRAM_USE_A_BGPAL:
		if(bank < DS_VRAM_E_64 || bank > DS_VRAM_G_16) {
			goto inval_use;
		}
		*bankreg[bank] = VRAM_ENABLE | VRAM_OFFSET(offs) | 4;
		return 0;

	case DS_VRAM_USE_A_OBJPAL:
		if(bank < DS_VRAM_F_16 || bank > DS_VRAM_G_16) {
			goto inval_use;
		}
		*bankreg[bank] = VRAM_ENABLE | 5;
		return 0;

	case DS_VRAM_USE_TEXIMG:
		if(bank > DS_VRAM_D_128) {
			goto inval_use;
		}
		*bankreg[bank] = VRAM_ENABLE | VRAM_OFFSET(offs) | 3;
		return 0;

	case DS_VRAM_USE_TEXPAL:
		if(bank < DS_VRAM_E_64) {
			goto inval_use;
		}
		*bankreg[bank] = VRAM_ENABLE | VRAM_OFFSET(offs) | 3;
		return 0;

	case DS_VRAM_USE_B_BG:
		if(bank != DS_VRAM_C_128 && bank < DS_VRAM_H_32) {
			goto inval_use;
		}
		*bankreg[bank] = VRAM_ENABLE | (bank == DS_VRAM_C_128 ? 4 : 1);
		if(bank != DS_VRAM_I_16) {
			return (void*)0x6200000;
		}
		return (void*)0x6208000;	/* bank I */

	case DS_VRAM_USE_B_OBJ:
		if(bank != DS_VRAM_D_128 && bank != DS_VRAM_I_16) {
			goto inval_use;
		}
		*bankreg[bank] = VRAM_ENABLE | (bank == DS_VRAM_D_128 ? 4 : 2);
		return (void*)0x6600000;

	case DS_VRAM_USE_B_BGPAL:
		if(bank != DS_VRAM_H_32) {
			goto inval_use;
		}
		*bankreg[bank] = VRAM_ENABLE | 2;
		return 0;

	case DS_VRAM_USE_B_OBJPAL:
		if(bank != DS_VRAM_I_16) {
			goto inval_use;
		}
		*bankreg[bank] = VRAM_ENABLE | 3;
		return 0;

	case DS_VRAM_USE_ARM7:
		if(bank != DS_VRAM_C_128 && bank != DS_VRAM_D_128) {
			goto inval_use;
		}
		if(offs > 1) {
			ds_panic("ds_vram_map: bank %c, use ARM7, invalid offset: %d\n", 'A' + bank, offs);
		}
		*bankreg[bank] = VRAM_ENABLE | VRAM_OFFSET(offs) | 2;
		return (void*)(0x6000000 + 0x20000 * offs);

	default:
		ds_panic("ds_vram_map: invalid usage number: %d\n", usage);
	}

inval_use:
	ds_panic("ds_vram_map: bank %c can't be used as %s", 'A' + bank, vram_usage_str[usage]);
	return 0;
}

void ds_vram_unmap(int bank)
{
	*bankreg[bank] = 0;
}


void ds_panic(const char *fmt, ...)
{
	/* TODO */
	for(;;);
}
