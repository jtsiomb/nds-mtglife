#include <stdlib.h>
#include <string.h>
#include "dsregs.h"
#include "ds.h"
#include "ds3.h"
#include "ds3impl.h"
#include "vmem.h"


static int calc_texture_size(int width, int height, int fmt);

#define DEF_TEXPAR	(TEXPAR_REPEAT_S | TEXPAR_REPEAT_T | TEXPAR_FMT_RGB)


#define MAX_TEXTURES	128
struct texture textures[MAX_TEXTURES];
static int curtex = -1;

#define MAX_PALETTES	32
struct tex_palette palettes[MAX_PALETTES];

#define MAX_TEX_BANKS	4
#define TEX_BANK_SIZE	131072
static int texbank[MAX_TEX_BANKS];
static int num_tex_banks;

#define MAX_PAL_BANKS	3
#define PAL_SLOT_SIZE	16384
static int palbank[MAX_PAL_BANKS];
static int num_pal_banks;

int ds3_add_texmem(int bank)
{
	int i;
	struct mem_range *mem;

	if(bank < DS_VRAM_A_128 || bank > DS_VRAM_D_128 || num_tex_banks >= MAX_TEX_BANKS) {
		return -1;
	}

	/* make sure we don't already have it */
	for(i=0; i<num_tex_banks; i++) {
		if(texbank[i] == bank) {
			return 0;
		}
	}

	if(!(mem = malloc(sizeof *mem))) {
		ds_panic("ds3_add_texmem: failed to allocate memory range");
		return -1;
	}
	mem->start = num_tex_banks * TEX_BANK_SIZE;
	mem->size = TEX_BANK_SIZE;
	mem->prev = mem->next = 0;
	ds3_free_vmem(TEX_MEM, mem);	/* add it to the free list */

	ds_vram_map(bank, DS_VRAM_USE_TEXIMG, num_tex_banks);
	texbank[num_tex_banks++] = bank;
	return 0;
}

static int palbank_offset(int bank)
{
	switch(bank) {
	case DS_VRAM_E_64:
		return 0;
	case DS_VRAM_F_16:
		return 4;
	case DS_VRAM_G_16:
		return 5;
	default:
		break;
	}
	return -1;
}

static int paloffs_bank(int offs)
{
	switch(offs) {
	case 0:
	case 1:
	case 2:
	case 3:
		return DS_VRAM_E_64;
	case 4:
		return DS_VRAM_F_16;
	case 5:
		return DS_VRAM_G_16;
	default:
		break;
	}
	return -1;
}

int ds3_add_palmem(int bank)
{
	struct mem_range *mem;
	int i, offs;

	if(bank < DS_VRAM_E_64 || bank > DS_VRAM_G_16) {
		return -1;
	}

	/* make sure we don't already have it */
	for(i=0; i<num_pal_banks; i++) {
		if(palbank[i] == bank) {
			return 0;
		}
	}

	if(!(mem = malloc(sizeof *mem))) {
		ds_panic("ds3_add_palmem: failed to allocate memory range");
		return -1;
	}

	offs = palbank_offset(bank);

	mem->start = offs * PAL_SLOT_SIZE;
	mem->size = bank == DS_VRAM_E_64 ? 65536 : 16384;
	mem->prev = mem->next = 0;
	ds3_free_vmem(PAL_MEM, mem);

	palbank[num_pal_banks++] = bank;
	ds_vram_map(bank, DS_VRAM_USE_TEXPAL, offs);
}

int ds3_gen_texture(void)
{
	int i;

	for(i=0; i<MAX_TEXTURES; i++) {
		if(!textures[i].used) {
			textures[i].used = 1;
			textures[i].mem = 0;
			textures[i].param = DEF_TEXPAR;
			textures[i].pal_base = -1;
			return i;
		}
	}
	return -1;
}

void ds3_del_texture(int id)
{
	if(!textures[id].used) return;

	if(textures[id].mem) {
		ds3_free_vmem(TEX_MEM, textures[id].mem);
	}
	textures[id].mem = 0;
	textures[id].param = 0;
	textures[id].used = 0;
	textures[id].pal_base = 0;

	if(curtex == id) curtex = -1;
}

void ds3_bind_texture(int id)
{
	REG_TEXIMAGE_PARAM = textures[id].param;
	curtex = id;

	if(textures[id].pal_base >= 0) {
		REG_PLTT_BASE = textures[id].pal_base;
	}
}

struct texture *ds3_priv_current_texture(void)
{
	if(curtex >= 0) {
		return textures + curtex;
	}
	return 0;
}

void ds3_tex_parameter(int id, int par, int val)
{
	uint32_t tmp, shift;

	switch(par) {
	case DS3_TEX_WRAP_S:
	case DS3_TEX_WRAP_T:
		shift = par - DS3_TEX_WRAP_S;
		switch(val) {
		case DS3_REPEAT:
			tmp = TEXPAR_REPEAT_S << shift;
			break;
		case DS3_CLAMP:
			tmp = 0;
			break;
		case DS3_MIRRORED_REPEAT:
			tmp = (TEXPAR_REPEAT_S | TEXPAR_FLIP_S) << shift;
			break;
		default:
			return;
		}
		textures[id].param = (textures[id].param & ~TEXPAR_WRAP_BITS) | tmp;
		break;

	case DS3_TEX_COLORKEY:
		if(val) {
			textures[id].param |= TEXPAR_KEY0;
		} else {
			textures[id].param &= ~TEXPAR_KEY0;
		}
		break;

	case DS3_TEX_PALETTE:
		if(palettes[val].used && palettes[val].mem) {
			/* pal_base is specified in 8-byte units for 4-color formats,
			 * and 16-byte units for all others
			 */
			int shift = ds3_priv_tex_fmt(textures + id) == DS3_INDEXED2 ? 3 : 4;
			textures[id].pal_base = palettes[val].mem->start >> shift;
		}
		break;

	default:
		break;
	}
}

#define PARAM_MASK	\
	(TEXPAR_ADDR_MASK | TEXPAR_SIZE_S_MASK | TEXPAR_SIZE_T_MASK | TEXPAR_FMT_MASK)

int ds3_tex_image(int id, int fmt, int xsz, int ysz, void *pixels)
{
	int i, sx, sy, bankidx;
	int texsize, prev_texsize;
	struct texture *tex = textures + id;
	unsigned int offs;
	uint16_t *dest, *src = pixels;

	texsize = calc_texture_size(xsz, ysz, fmt);
	prev_texsize = ds3_priv_tex_size(tex + id);

	/* if the previous size wasn't identical to the new requirements, realloc it */
	if(!tex->mem || texsize != prev_texsize) {
		if(tex->mem) {
			ds3_free_vmem(TEX_MEM, tex->mem);
		}
		if(!(tex->mem = ds3_alloc_vmem(TEX_MEM, texsize))) {
			return -1;
		}
	}

	/* texture size in param register is 3 bits (0-7) which represents
	 * the N shift amount in 8 << N. [8, 16, 32, 64, 128, 256, 512, 1024]
	 */
	for(i=0; i<8; i++) {
		if((8 << i) == xsz) sx = i;
		if((8 << i) == ysz) sy = i;
	}

	tex->param = (tex->param & ~PARAM_MASK) | TEXPAR_ADDR(tex->mem->start) |
		TEXPAR_SIZE_S(sx) | TEXPAR_SIZE_T(sy) | fmt;

	/* map the texture storage temporarily in CPU-accessible memory to upload the pixels */
	offs = tex->mem->start;
	bankidx = tex->mem->start / TEX_BANK_SIZE;
	dest = ds_vram_map(texbank[bankidx], DS_VRAM_USE_LCDC, 0) + offs % TEX_BANK_SIZE;

	while(texsize > 0) {
		unsigned int next_bank_offs = (bankidx + 1) * TEX_BANK_SIZE;
		unsigned int nbytes = next_bank_offs - offs;
		unsigned int hwords = nbytes / 2;

		if(texsize < nbytes) {
			nbytes = texsize;
		}

		ds_vram_map(texbank[bankidx], DS_VRAM_USE_LCDC, 0);
		for(i=0; i<hwords; i++) {
			*dest++ = *src++;
		}
		/*memcpy(dest, src, nbytes);*/
		ds_vram_map(texbank[bankidx], DS_VRAM_USE_TEXIMG, bankidx);

		texsize -= nbytes;
		offs = next_bank_offs;
		bankidx++;
	}

	return 0;
}

int ds3_gen_palette(void)
{
	int i;

	for(i=0; i<MAX_PALETTES; i++) {
		if(!palettes[i].used) {
			palettes[i].used = 1;
			palettes[i].ncols = 0;
			palettes[i].mem = 0;
			return i;
		}
	}
	return -1;
}

void ds3_del_palette(int id)
{
	if(!palettes[id].used) return;

	if(palettes[id].mem) {
		ds3_free_vmem(PAL_MEM, palettes[id].mem);
	}
	palettes[id].mem = 0;
	palettes[id].used = 0;
	palettes[id].ncols = 0;
}

int ds3_palette_data(int id, int ncols, uint16_t *cols)
{
	int i;
	uint16_t *dest;
	struct tex_palette *pal = palettes + id;
	int palsz = ncols * 2;
	int prev_palsz = pal->ncols * 2;

	/* if the storage requirements changed, realloc vmem for it */
	if(!pal->mem || palsz != prev_palsz) {
		if(pal->mem) {
			ds3_free_vmem(PAL_MEM, pal->mem);
		}
		if(!(pal->mem = ds3_alloc_vmem(PAL_MEM, palsz))) {
			return -1;
		}
	}
	pal->ncols = ncols;

	if(cols) {
		if(!(dest = ds3_map_palette(id))) {
			return -1;
		}
		for(i=0; i<ncols; i++) {
			*dest++ = *cols++;
		}
		ds3_unmap_palette(id);
	}
	return 0;
}

uint16_t *ds3_map_palette(int id)
{
	struct tex_palette *pal = palettes + id;
	int palsz = pal->ncols * 2;
	int slot, nslots;
	uint16_t *res = 0;

	if(!pal->mem) return 0;

	slot = pal->mem->start / PAL_SLOT_SIZE;
	while(palsz > 0) {
		int bank = paloffs_bank(slot);
		uint16_t *addr = ds_vram_map(bank, DS_VRAM_USE_LCDC, 0);
		if(!res) res = addr;	/* we'll return the first pointer of the range */

		nslots = bank == DS_VRAM_E_64 ? 4 : 1;
		slot += nslots;
		palsz -= nslots * PAL_SLOT_SIZE;
	}

	return res;
}

void ds3_unmap_palette(int id)
{
	struct tex_palette *pal = palettes + id;
	int palsz = pal->ncols * 2;
	int slot, nslots;

	if(!pal->mem) return;

	slot = pal->mem->start / PAL_SLOT_SIZE;
	while(palsz > 0) {
		int bank = paloffs_bank(slot);
		ds_vram_map(bank, DS_VRAM_USE_TEXPAL, slot);
		nslots = bank == DS_VRAM_E_64 ? 4 : 1;
		slot += nslots;
		palsz -= nslots * PAL_SLOT_SIZE;
	}
}

int ds3_priv_tex_width(struct texture *tex)
{
	return 1 << ds3_priv_tex_width_shift(tex);
}

int ds3_priv_tex_height(struct texture *tex)
{
	return 1 << ds3_priv_tex_height_shift(tex);
}

int ds3_priv_tex_width_shift(struct texture *tex)
{
	return ((tex->param >> TEXPAR_SIZE_S_SHIFT) & 7) + 3;
}

int ds3_priv_tex_height_shift(struct texture *tex)
{
	return ((tex->param >> TEXPAR_SIZE_T_SHIFT) & 7) + 3;
}

int ds3_priv_tex_fmt(struct texture *tex)
{
	return (tex->param >> TEXPAR_FMT_SHIFT) & 7;
}

int ds3_priv_tex_size(struct texture *tex)
{
	int w = ds3_priv_tex_width(tex);
	int h = ds3_priv_tex_height(tex);
	int fmt = ds3_priv_tex_fmt(tex);
	return calc_texture_size(w, h, fmt);
}

static int calc_texture_size(int width, int height, int fmt)
{
	int npix = width * height;

	switch(fmt) {
	case DS3_INDEXED5_ALPHA3:
	case DS3_INDEXED3_ALPHA5:
	case DS3_INDEXED8:
		return npix;

	case DS3_INDEXED2:
	case DS3_COMPRESSED_RGB:
		return npix / 4;

	case DS3_INDEXED4:
		return npix / 2;

	case DS3_RGB5_A1:
		return npix * 2;

	default:
		break;
	}
	return 0;
}
