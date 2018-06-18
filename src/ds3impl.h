#ifndef DS3IMPL_H_
#define DS3IMPL_H_

struct mem_range;

struct texture {
	int used;
	struct mem_range *mem;
	uint32_t param;
	uint16_t pal_base;
};

struct tex_palette {
	int used;
	int ncols;
	struct mem_range *mem;
};

struct texture *ds3_priv_current_texture(void);
int ds3_priv_tex_width(struct texture *tex);
int ds3_priv_tex_height(struct texture *tex);
int ds3_priv_tex_width_shift(struct texture *tex);
int ds3_priv_tex_height_shift(struct texture *tex);
int ds3_priv_tex_fmt(struct texture *tex);
int ds3_priv_tex_size(struct texture *tex);

#endif	/* DS3IMPL_H_ */
