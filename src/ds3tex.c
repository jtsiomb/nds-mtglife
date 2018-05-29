#include <stdlib.h>
#include <string.h>
#include "dsregs.h"
#include "ds.h"
#include "ds3.h"

struct mem_range {
	unsigned int start, size;
	struct mem_range *prev, *next;
};

struct texture {
	int used;
	struct mem_range *mem;
	uint32_t param;
};

static int tex_width(struct texture *tex);
static int tex_height(struct texture *tex);
static int tex_fmt(struct texture *tex);
static int tex_size(struct texture *tex);

static int calc_texture_size(int width, int height, int fmt);

static struct mem_range *alloc_tex_mem(int sz);
static void free_tex_mem(struct mem_range *mem);

#define DEF_TEXPAR	(TEXPAR_REPEAT_S | TEXPAR_REPEAT_T | TEXPAR_FMT_RGB)

static struct mem_range *free_list;

#define MAX_TEXTURES	128
struct texture textures[MAX_TEXTURES];
static int curtex = -1;

#define MAX_TEX_BANKS	4
#define TEX_BANK_SIZE	131072
static int texbank[MAX_TEX_BANKS];
static int num_tex_banks;

int ds3_add_texmem(int bank)
{
	int i, slot = -1;
	struct mem_range *mem;

	if(bank < DS_VRAM_A_128 || bank > DS_VRAM_D_128 || num_tex_banks >= MAX_TEX_BANKS) {
		return -1;
	}

	if(!(mem = malloc(sizeof *mem))) {
		ds_panic("ds3_add_texmem: failed to allocate memory range");
		return -1;
	}
	mem->start = num_tex_banks * TEX_BANK_SIZE;
	mem->size = TEX_BANK_SIZE;
	mem->prev = mem->next = 0;
	free_tex_mem(mem);	/* add it to the free list */

	texbank[num_tex_banks++] = bank;

	ds_vram_map(bank, DS_VRAM_USE_TEXIMG, 0);
	return 0;
}


int ds3_gen_texture(void)
{
	int i;

	for(i=0; i<MAX_TEXTURES; i++) {
		if(!textures[i].used) {
			textures[i].mem = 0;
			textures[i].param = DEF_TEXPAR;
			return i;
		}
	}
	return -1;
}

void ds3_del_texture(int id)
{
	if(!textures[id].used) return;

	if(textures[id].mem) {
		free_tex_mem(textures[id].mem);
	}
	textures[id].mem = 0;
	textures[id].param = 0;
	textures[id].used = 0;

	if(curtex == id) curtex = -1;
}

void ds3_bind_texture(int id)
{
	REG_TEXIMAGE_PARAM = textures[id].param;
	curtex = id;
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
	unsigned char *dest, *src = pixels;

	texsize = calc_texture_size(xsz, ysz, fmt);
	prev_texsize = tex_size(tex + id);

	/* if the previous size wasn't identical to the new requirements, realloc it */
	if(tex->mem) {
		free_tex_mem(tex->mem);
	}
	if(!(tex->mem = alloc_tex_mem(texsize))) {
		return -1;
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

		if(texsize < nbytes) {
			nbytes = texsize;
		}

		ds_vram_map(texbank[bankidx], DS_VRAM_USE_LCDC, 0);
		memcpy(dest, src, nbytes);
		ds_vram_map(texbank[bankidx], DS_VRAM_USE_TEXIMG, 0);

		dest += nbytes;
		src += nbytes;
		texsize -= nbytes;
		offs = next_bank_offs;
		bankidx++;
	}

	return 0;
}

static int tex_width(struct texture *tex)
{
	return (tex->param >> TEXPAR_SIZE_S_SHIFT) & 7;
}

static int tex_height(struct texture *tex)
{
	return (tex->param >> TEXPAR_SIZE_T_SHIFT) & 7;
}

static int tex_fmt(struct texture *tex)
{
	return (tex->param >> TEXPAR_FMT_SHIFT) & 7;
}

static int tex_size(struct texture *tex)
{
	return calc_texture_size(tex_width(tex), tex_height(tex), tex_fmt(tex));
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

	case DS3_RGB:
		return npix * 2;

	default:
		break;
	}
	return 0;
}

static struct mem_range *alloc_tex_mem(int sz)
{
	struct mem_range *res, *m = free_list;

	while(m) {
		if(m->size == sz) {
			/* found a block exactly the right size, rip the node off and return it */
			if(m->prev) {
				m->prev->next = m->next;
			} else {
				free_list = m->next;
			}
			if(m->next) m->next->prev = m->prev;
			m->prev = m->next = 0;
			return m;
		}

		if(m->size > sz) {
			/* found a block big enough, carve a piece and return new node */
			if(!(res = malloc(sizeof *res))) {
				ds_panic("alloc_tex_mem: failed to allocate mem_range node");
				return 0;
			}
			res->start = m->start;
			res->size = sz;
			m->start += sz;
			m->size -= sz;
			return res;
		}

		m = m->next;
	}

	return 0;
}

static void free_tex_mem(struct mem_range *mem)
{
	struct mem_range *m, *mnext;

	if(!free_list) {
		free_list = mem;
		return;
	}

	/* find where to put it */
	m = free_list;
	while(m->next && m->next->start < mem->start) {
		m = m->next;
	}
	mnext = m->next;

	if(m->start + m->size == mem->start) {
		if(mnext && mem->start + mem->size == mnext->start) {
			/* coalesce both sides */
			m->size += mem->size + mnext->size;
			m->next = mnext->next;
			if(mnext->next) mnext->next->prev = m;
			free(mem);
			free(mnext);
			return;
		}

		/* coalesce low */
		m->size += mem->size;
		free(mem);
		return;
	}

	if(mem->start + mem->size == mnext->start) {
		/* coalesce high */
		mnext->size += mem->size;
		mnext->start = mem->start;
		free(mem);
		return;
	}

	/* disjoint, just link it in */
	mem->prev = m;
	mem->next = mnext;
	m->next = mem;
	if(mnext) mnext->prev = mem;
}
