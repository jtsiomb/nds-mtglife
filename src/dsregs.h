#ifndef DSREGS_H_
#define DSREGS_H_

#include <stdint.h>

#define REG_BASE		0x4000000
#define REG8(x)			(*(volatile int8_t*)(REG_BASE + (x)))
#define REG16(x)		(*(volatile int16_t*)(REG_BASE + (x)))
#define REG32(x)		(*(volatile int32_t*)(REG_BASE + (x)))
#define REG64(x)		(*(volatile int64_t*)(REG_BASE + (x)))

/* ---- display engine A ---- */
#define REG_DISPCNT		REG32(0x00)
#define REG_DISPSTAT	REG16(0x04)
#define REG_VCOUNT		REG16(0x06)
#define REG_BG0CNT		REG16(0x08)
#define REG_BG1CNT		REG16(0x0a)
#define REG_BG2CNT		REG16(0x0c)
#define REG_BG3CNT		REG16(0x0e)
/* scrolling registers */
#define REG_BG0HOFS		REG16(0x10)
#define REG_BG0VOFS		REG16(0x12)
#define REG_BG1HOFS		REG16(0x14)
#define REG_BG1VOFS		REG16(0x16)
#define REG_BG2HOFS		REG16(0x18)
#define REG_BG2VOFS		REG16(0x1a)
#define REG_BG3HOFS		REG16(0x1c)
#define REG_BG3VOFS		REG16(0x1e)
/* BG rotation and scaling registers */
#define REG_BG2PA		REG16(0x20)
#define REG_BG2PB		REG16(0x22)
#define REG_BG2PC		REG16(0x24)
#define REG_BG2PD		REG16(0x26)
#define REG_BG2X		REG32(0x28)
#define REG_BG2Y		REG32(0x2c)
#define REG_BG3PA		REG16(0x30)
#define REG_BG3PB		REG16(0x32)
#define REG_BG3PC		REG16(0x34)
#define REG_BG3PD		REG16(0x36)
#define REG_BG3X		REG32(0x38)
#define REG_BG3Y		REG32(0x3c)
/* window registers */
#define REG_WIN0H		REG16(0x40)
#define REG_WIN1H		REG16(0x42)
#define REG_WIN0V		REG16(0x44)
#define REG_WIN1V		REG16(0x46)
#define REG_WININ		REG16(0x48)
#define REG_WINOUT		REG16(0x4a)
/* mosaic */
#define REG_MOSAIC		REG16(0x4c)
/* color effects */
#define REG_BLDCNT		REG16(0x50)
#define REG_BLDALPHA	REG16(0x52)
#define REG_BLDY		REG16(0x54)

#define REG_DISP3DCNT		REG16(0x60)
#define REG_DISPCAPCNT		REG32(0x64)
#define REG_DISP_MMEM_FIFO	REG32(0x68)
#define REG_MASTER_BRIGHT	REG16(0x6c)

/* ---- display engine B ---- */
#define REG_B_DISPCNT		REG32(0x1000)
#define REG_B_BG0CNT		REG16(0x1008)
#define REG_B_BG1CNT		REG16(0x100a)
#define REG_B_BG2CNT		REG16(0x100c)
#define REG_B_BG3CNT		REG16(0x100e)
/* scrolling registers */
#define REG_B_BG0HOFS		REG16(0x1010)
#define REG_B_BG0VOFS		REG16(0x1012)
#define REG_B_BG1HOFS		REG16(0x1014)
#define REG_B_BG1VOFS		REG16(0x1016)
#define REG_B_BG2HOFS		REG16(0x1018)
#define REG_B_BG2VOFS		REG16(0x101a)
#define REG_B_BG3HOFS		REG16(0x101c)
#define REG_B_BG3VOFS		REG16(0x101e)
/* BG rotation and scaling registers */
#define REG_B_BG2PA			REG16(0x1020)
#define REG_B_BG2PB			REG16(0x1022)
#define REG_B_BG2PC			REG16(0x1024)
#define REG_B_BG2PD			REG16(0x1026)
#define REG_B_BG2X			REG32(0x1028)
#define REG_B_BG2Y			REG32(0x102c)
#define REG_B_BG3PA			REG16(0x1030)
#define REG_B_BG3PB			REG16(0x1032)
#define REG_B_BG3PC			REG16(0x1034)
#define REG_B_BG3PD			REG16(0x1036)
#define REG_B_BG3X			REG32(0x1038)
#define REG_B_BG3Y			REG32(0x103c)
/* window registers */
#define REG_B_WIN0H			REG16(0x1040)
#define REG_B_WIN1H			REG16(0x1042)
#define REG_B_WIN0V			REG16(0x1044)
#define REG_B_WIN1V			REG16(0x1046)
#define REG_B_WININ			REG16(0x1048)
#define REG_B_WINOUT		REG16(0x104a)
/* mosaic */
#define REG_B_MOSAIC		REG16(0x104c)
/* color effects */
#define REG_B_BLDCNT		REG16(0x1050)
#define REG_B_BLDALPHA		REG16(0x1052)
#define REG_B_BLDY			REG16(0x1054)

#define REG_B_MASTER_BRIGHT	REG16(0x106c)


#define DISPCNT_BGMODE(x)		(x)
#define DISPCNT_BGMODE_MASK		0x00000007
#define DISPCNT_MODE(x)			((uint32_t)(x) << 16)
#define DISPCNT_MODE_MASK		0x00000300
#define DISPCNT_BG0_3D			0x00000008
#define DISPCNT_TILE_OBJ_1DMAP	0x00000010
#define DISPCNT_BM_OBJ_256X256	0x00000020
#define DISPCNT_BM_OBJ_1DMAP	0x00000040
#define DISPCNT_BLANK			0x00000080
#define DISPCNT_BG0				0x00000100
#define DISPCNT_BG1				0x00000200
#define DISPCNT_BG2				0x00000400
#define DISPCNT_BG3				0x00000800
#define DISPCNT_OBJ				0x00001000
#define DISPCNT_WIN0			0x00002000
#define DISPCNT_WIN1			0x00004000
#define DISPCNT_OBJWIN			0x00008000

#define BGXCNT_PRIO(x)			(x)
#define BGXCNT_CHARBASE(x)		((x) << 2)
#define BGXCNT_MOSAIC			0x0040
#define BGXCNT_COL_256			0x0080
#define BGXCNT_BM				0x0080
#define BGXCNT_SCRBASE(x)		((x) << 8)
#define BGXCNT_OVF_WRAP			0x2000
#define BGXCNT_SCRSIZE(x)		((x) << 14)
#define BGXCNT_TX_256X256		BGXCNT_SCRSIZE(0)
#define BGXCNT_TX_512X256		BGXCNT_SCRSIZE(1)
#define BGXCNT_TX_256X512		BGXCNT_SCRSIZE(2)
#define BGXCNT_TX_512X512		BGXCNT_SCRSIZE(3)
#define BGXCNT_RS_128X128		BGXCNT_SCRSIZE(0)
#define BGXCNT_RS_256X256		BGXCNT_SCRSIZE(1)
#define BGXCNT_RS_512X512		BGXCNT_SCRSIZE(2)
#define BGXCNT_RS_1024X1024		BGXCNT_SCRSIZE(3)
#define BGXCNT_BM_128X128		(BGXCNT_SCRSIZE(0) | BGXCNT_BM)
#define BGXCNT_BM_256X256		(BGXCNT_SCRSIZE(1) | BGXCNT_BM)
#define BGXCNT_BM_512X256		(BGXCNT_SCRSIZE(2) | BGXCNT_BM)
#define BGXCNT_BM_512X512		(BGXCNT_SCRSIZE(3) | BGXCNT_BM)
#define BGXCNT_BM8				0
#define BGXCNT_BM16				0x0004

#define VRAM_OFFSET(x)		((x) << 3)
#define VRAM_ENABLE			0x80

/* ---- DMA registers ---- */
#define REG_DMA0SAD		REG32(0xb0)
#define REG_DMA0DAD		REG32(0xb4)
#define REG_DMA0CNT_L	REG16(0xb8)
#define REG_DMA0CNT_H	REG16(0xba)
#define REG_DMA1SAD		REG32(0xbc)
#define REG_DMA1DAD		REG32(0xc0)
#define REG_DMA1CNT_L	REG16(0xc4)
#define REG_DMA1CNT_H	REG16(0xc6)
#define REG_DMA2SAD		REG32(0xc8)
#define REG_DMA2DAD		REG32(0xcc)
#define REG_DMA2CNT_L	REG16(0xd0)
#define REG_DMA2CNT_H	REG16(0xd2)
#define REG_DMA3SAD		REG32(0xd4)
#define REG_DMA3DAD		REG32(0xd8)
#define REG_DMA3CNT_L	REG16(0xdc)
#define REG_DMA3CNT_H	REG16(0xde)
#define REG_DMA0FILL	REG32(0xe0)
#define REG_DMA1FILL	REG32(0xe4)
#define REG_DMA2FILL	REG32(0xe8)
#define REG_DMA3FILL	REG32(0xec)

/* ---- timer registers ---- */
#define REG_TM0CNT_L	REG16(0x100)
#define REG_TM0CNT_H	REG16(0x102)
#define REG_TM1CNT_L	REG16(0x104)
#define REG_TM1CNT_H	REG16(0x106)
#define REG_TM2CNT_L	REG16(0x108)
#define REG_TM2CNT_H	REG16(0x10a)
#define REG_TM3CNT_L	REG16(0x10c)
#define REG_TM3CNT_H	REG16(0x10e)

/* ---- keypad registers ---- */
#define REG_KEYINPUT	REG16(0x130)
#define REG_KEYCNT		REG16(0x132)

/* ---- IPC/ROM registers ---- */
#define REG_IPCSYNC		REG16(0x180)
#define REG_IPCFIFOCNT	REG16(0x184)
#define REG_IPCFIFOSEND	REG32(0x188)
#define REG_AUXSPICNT	REG16(0x1a0)
#define REG_AUXSPIDATA	REG16(0x1a2)
#define REG_GCARDCNT	REG32(0x1a4)
#define REG_GCARDCMD64	REG64(0x1a8)
#define REG_GCARDSEED0	REG32(0x1b0)
#define REG_GCARDSEED1	REG32(0x1b4)
#define REG_GCARDSEED0X	REG16(0x1b8)
#define REG_GCARDSEED1X	REG16(0x1ba)

/* ---- memory & IRQ control registers ---- */
#define REG_EXMEMCNT	REG16(0x204)
#define REG_IME			REG16(0x208)
#define REG_IE			REG32(0x210)
#define REG_IF			REG32(0x214)
#define REG_VRAMCNT_A	REG8(0x240)
#define REG_VRAMCNT_B	REG8(0x241)
#define REG_VRAMCNT_C	REG8(0x242)
#define REG_VRAMCNT_D	REG8(0x243)
#define REG_VRAMCNT_E	REG8(0x244)
#define REG_VRAMCNT_F	REG8(0x245)
#define REG_VRAMCNT_G	REG8(0x246)
#define REG_WRAMCNT		REG8(0x247)
#define REG_VRAMCNT_H	REG8(0x248)
#define REG_VRAMCNT_I	REG8(0x249)

/* ---- math hardware registers ---- */
#define REG_DIVCNT			REG32(0x280)
#define REG_DIV_NUMER		REG64(0x290)
#define REG_DIV_NUMERL		REG32(0x290)
#define REG_DIV_NUMERH		REG32(0x294)
#define REG_DIV_DENOM		REG64(0x298)
#define REG_DIV_DENOML		REG32(0x298)
#define REG_DIV_DENOMH		REG32(0x29c)
#define REG_DIV_RESULT		REG64(0x2a0)
#define REG_DIV_RESULTL		REG32(0x2a0)
#define REG_DIV_RESULTH		REG32(0x2a4)
#define REG_DIVREM_RESULT	REG64(0x2a8)
#define REG_DIVREM_RESULTL	REG32(0x2a8)
#define REG_DIVREM_RESULTH	REG32(0x2ac)
#define REG_SQRTCNT			REG16(0x2b0)
#define REG_SQRT_RESULT		REG32(0x2b4)
#define REG_SQRT_PARAM		REG64(0x2b8)
#define REG_POSTFLG			REG32(0x300)
#define REG_POWCNT1			REG16(0x304)
#define REG_POWCNT2			REG16(0x304)

#define DIVCNT_32_32		0
#define DIVCNT_64_32		1
#define DIVCNT_64_64		2
#define DIVCNT_DIV0			0x4000
#define DIVCNT_BUSY			0x8000

#define POWCNT1_LCD			0x0001
#define POWCNT1_2DA			0x0002
#define POWCNT1_3DREND		0x0004
#define POWCNT1_3DGEOM		0x0008
#define POWCNT1_2DB			0x0200
#define POWCNT1_DSWAP		0x8000

#define POWCNT2_SOUND		0x0001
#define POWCNT2_WIFI		0x0002

/* ---- sound registers ---- */
#define REG_SOUNDXCNT(x)	REG32(0x400 | ((x) << 4))
#define REG_SOUNDXSAD(x)	REG32(0x404 | ((x) << 4))
#define REG_SOUNDXTMR(x)	REG32(0x408 | ((x) << 4))
#define REG_SOUNDXPNT(x)	REG32(0x40a | ((x) << 4))
#define REG_SOUNDXLEN(x)	REG32(0x40c | ((x) << 4))
#define REG_SOUNDCNT		REG32(0x500)
#define REG_SOUNDBIAS		REG32(0x504)

/* ---- 3D hardware registers ---- */
/* rendering engine */
#define REG_RDLINES_COUNR	REG8(0x320)
#define REG_EDGE_COLOR0		REG16(0x330)
#define REG_EDGE_COLOR1		REG16(0x332)
#define REG_EDGE_COLOR2		REG16(0x334)
#define REG_EDGE_COLOR3		REG16(0x336)
#define REG_EDGE_COLOR4		REG16(0x338)
#define REG_EDGE_COLOR5		REG16(0x33a)
#define REG_EDGE_COLOR6		REG16(0x33c)
#define REG_EDGE_COLOR7		REG16(0x33e)
#define REG_ALPHA_TEST_REF	REG8(0x340)
#define REG_CLEAR_COLOR		REG32(0x350)
#define REG_CLEAR_DEPTH		REG16(0x354)
#define REG_CLRIMAGE_OFFSET	REG16(0x356)
#define REG_FOG_COLOR		REG32(0x358)
#define REG_FOG_OFFSET		REG16(0x35c)
#define FOG_TABLE_ADDR		((uint8_t*)(REG_BASE + 0x360))
#define TOON_TABLE_ADDR		((uint16_t*)(REG_BASE + 0x380))
/* geometry engine */
#define GXFIFO_ADDR			((uint8_t*)(REG_BASE + 0x400))
#define REG_GXSTAT			REG32(0x600)
#define REG_RAM_COUNT		REG32(0x604)
#define REG_DISP_1DOT_DEPTH	REG16(0x610)
#define POS_RESULT_ADDR		((uint8_t*)(REG_BASE + 0x620))
#define VEC_RESULT_ADDR		((uint8_t*)(REG_BASE + 0x630))
#define CLIPMTX_RESULT_ADDR	((uint32_t*)(REG_BASE + 0x640))
#define VECMTX_RESULT_ADDR	((uint32_t*)(REG_BASE + 0x680))
/* geometry command ports */
#define REG_MTX_MODE		REG32(0x440)	/* 1 */
#define REG_MTX_PUSH		REG32(0x444)	/* 0 */
#define REG_MTX_POP			REG32(0x448)	/* 1 */
#define REG_MTX_STORE		REG32(0x44c)	/* 1 */
#define REG_MTX_RESTORE		REG32(0x450)	/* 1 */
#define REG_MTX_IDENTITY	REG32(0x454)	/* 0 */
#define REG_MTX_LOAD_4X4	REG32(0x458)	/* 16 */
#define REG_MTX_LOAD_4X3	REG32(0x45c)	/* 12 */
#define REG_MTX_MULT_4X4	REG32(0x460)	/* 16 */
#define REG_MTX_MULT_4X3	REG32(0x464)	/* 12 */
#define REG_MTX_MULT_3X3	REG32(0x468)	/* 9 */
#define REG_MTX_SCALE		REG32(0x46c)	/* 3 */
#define REG_MTX_TRANS		REG32(0x470)	/* 3 */
#define REG_COLOR			REG32(0x480)	/* 1 */
#define REG_NORMAL			REG32(0x484)	/* 1 */
#define REG_TEXCOORD		REG32(0x488)	/* 1 */
#define REG_VTX_16			REG32(0x48c)	/* 2 */
#define REG_VTX_10			REG32(0x490)	/* 1 */
#define REG_VTX_XY			REG32(0x494)	/* 1 */
#define REG_VTX_XZ			REG32(0x498)	/* 1 */
#define REG_VTX_YZ			REG32(0x49c)	/* 1 */
#define REG_VTX_DIFF		REG32(0x4a0)	/* 1 */
#define REG_POLYGON_ATTR	REG32(0x4a4)	/* 1 */
#define REG_TEXIMAGE_PARAM	REG32(0x4a8)	/* 1 */
#define REG_PLTT_BASE		REG32(0x4ac)	/* 1 */
#define REG_DIF_AMB			REG32(0x4c0)	/* 1 */
#define REG_SPE_EMI			REG32(0x4c4)	/* 1 */
#define REG_LIGHT_VECTOR	REG32(0x4c8)	/* 1 */
#define REG_LIGHT_COLOR		REG32(0x4cc)	/* 1 */
#define REG_SHININESS		REG32(0x4d0)	/* 32 */
#define REG_BEGIN_VTXS		REG32(0x500)	/* 1 */
#define REG_END_VTXS		REG32(0x504)	/* 0 */
#define REG_SWAP_BUFFERS	REG32(0x540)	/* 1 */
#define REG_VIEWPORT		REG32(0x580)	/* 1 */
#define REG_BOX_TEST		REG32(0x5c0)	/* 3 */
#define REG_POS_TEST		REG32(0x5c4)	/* 2 */
#define REG_VEC_TEST		REG32(0x5c8)	/* 1 */

/* geometry commands */
#define GCMD_MTX_MODE		0x10	/* 1 */
#define GCMD_MTX_PUSH		0x11	/* 0 */
#define GCMD_MTX_POP		0x12	/* 1 */
#define GCMD_MTX_STORE		0x13	/* 1 */
#define GCMD_MTX_RESTORE	0x14	/* 1 */
#define GCMD_MTX_IDENTITY	0x15	/* 0 */
#define GCMD_MTX_LOAD_4X4	0x16	/* 16 */
#define GCMD_MTX_LOAD_4X3	0x17	/* 12 */
#define GCMD_MTX_MULT_4X4	0x18	/* 16 */
#define GCMD_MTX_MULT_4X3	0x19	/* 12 */
#define GCMD_MTX_MULT_3X3	0x1a	/* 9 */
#define GCMD_MTX_SCALE		0x1b	/* 3 */
#define GCMD_MTX_TRANS		0x1c	/* 3 */
#define GCMD_COLOR			0x20	/* 1 */
#define GCMD_NORMAL			0x21	/* 1 */
#define GCMD_TEXCOORD		0x22	/* 1 */
#define GCMD_VTX_16			0x23	/* 2 */
#define GCMD_VTX_10			0x24	/* 1 */
#define GCMD_VTX_XY			0x25	/* 1 */
#define GCMD_VTX_XZ			0x26	/* 1 */
#define GCMD_VTX_YZ			0x27	/* 1 */
#define GCMD_VTX_DIFF		0x28	/* 1 */
#define GCMD_POLYGON_ATTR	0x29	/* 1 */
#define GCMD_TEXIMAGE_PARAM	0x2a	/* 1 */
#define GCMD_PLTT_BASE		0x2b	/* 1 */
#define GCMD_DIF_AMB		0x30	/* 1 */
#define GCMD_SPE_EMI		0x31	/* 1 */
#define GCMD_LIGHT_VECTOR	0x32	/* 1 */
#define GCMD_LIGHT_COLOR	0x33	/* 1 */
#define GCMD_SHININESS		0x34	/* 32 */
#define GCMD_BEGIN_VTXS		0x40	/* 1 */
#define GCMD_END_VTXS		0x41	/* 0 */
#define GCMD_SWAP_BUFFERS	0x50	/* 1 */
#define GCMD_VIEWPORT		0x60	/* 1 */
#define GCMD_BOX_TEST		0x70	/* 3 */
#define GCMD_POS_TEST		0x71	/* 2 */
#define GCMD_VEC_TEST		0x72	/* 1 */

/* addresses of interest */
#define SHARED_WRAM_PTR		((void*)0x3000000)
#define VRAM_BGA_PTR		((void*)0x6000000)
#define VRAM_BGB_PTR		((void*)0x6200000)
#define VRAM_OBJA_PTR		((void*)0x6400000)
#define VRAM_OBJB_PTR		((void*)0x6600000)
#define VRAM_LCDC_PTR		((void*)0x6800000)
#define OAM_PTR				((void*)0x7000000)

#endif	/* DSREGS_H_ */
