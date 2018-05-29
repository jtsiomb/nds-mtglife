#ifndef DS3_H_
#define DS3_H_

#include <stdint.h>

#define RGB15(r, g, b)	(((r) & 0x1f) | (((g) & 0x1f) << 5) | (((b) & 0x1f) << 10))

#define DS3_TEXTURE_2D		0x0001
#define DS3_SPECULAR		0x0002
#define DS3_ALPHA_TEST		0x0004
#define DS3_BLEND			0x0008
#define DS3_POLYGON_SMOOTH	0x0010
#define DS3_EDGE			0x0020
#define DS3_FOG_ALPHA		0x0040
#define DS3_FOG				0x0080
#define DS3_CLEAR_BM		0x4000

#define DS3_TRIANGLES		0
#define DS3_QUADS			1
#define DS3_TRIANGLE_STRIP	2
#define DS3_QUAD_STRIP		3

#define DS3_PROJECTION		0
#define DS3_MODELVIEW		1
#define DS3_TEXTURE			2

#define DS3_TEX_WRAP_S		0
#define DS3_TEX_WRAP_T		1
#define DS3_TEX_COLORKEY	2

#define DS3_REPEAT			0
#define DS3_CLAMP			1
#define DS3_MIRRORED_REPEAT	2

/* these match the TEXPAR_FMT_* bits */
#define DS3_INDEXED5_ALPHA3	0x04000000
#define DS3_INDEXED2		0x08000000
#define DS3_INDEXED4		0x0c000000
#define DS3_INDEXED8		0x10000000
#define DS3_COMPRESSED_RGB	0x14000000
#define DS3_INDEXED3_ALPHA5	0x18000000
#define DS3_RGB				0x1c000000

int ds3_add_texmem(int bank);

void ds3_enable(unsigned int x);
void ds3_disable(unsigned int x);

void ds3_clear_color(uint16_t color, int a);
void ds3_clear_depth(int z);

void ds3_viewport(int x, int y, int w, int h);

void ds3_matrix_mode(int mmode);
void ds3_load_identity(void);
void ds3_load_matrix(int32_t *m);
void ds3_load_matrix4x3(int32_t *m);
void ds3_mult_matrix(int32_t *m);
void ds3_mult_matrix4x3(int32_t *m);
void ds3_mult_matrix3x3(int32_t *m);
void ds3_push_matrix(void);
void ds3_pop_matrix(void);
void ds3_translate(int32_t x, int32_t y, int32_t z);
void ds3_scale(int32_t x, int32_t y, int32_t z);

void ds3_swap_buffers(void);

void ds3_begin(int prim);
void ds3_end(void);

void ds3_vertex3(int32_t x, int32_t y, int32_t z);
void ds3_vertex3f(float x, float y, float z);
void ds3_vertex2(int32_t x, int32_t y);
void ds3_vertex2f(float x, float y);
void ds3_color(uint16_t color);
void ds3_color3b(unsigned char r, unsigned char g, unsigned char b);
void ds3_color3f(float r, float g, float b);
void ds3_normal(int32_t x, int32_t y, int32_t z);
void ds3_normal3f(float x, float y, float z);
void ds3_texcoord2(int32_t s, int32_t t);
void ds3_texcoord2f(float s, float t);

void ds3_ortho(int32_t left, int32_t right, int32_t top, int32_t bottom, int32_t znear, int32_t zfar);
void ds3_orthof(float left, float right, float top, float bottom, float znear, float zfar);

void ds3_frustum(int32_t left, int32_t right, int32_t top, int32_t bottom, int32_t znear, int32_t zfar);

void ds3_perspectivef(float vfov_deg, float aspect, float znear, float zfar);

int ds3_gen_texture(void);
void ds3_del_texture(int tex);
void ds3_bind_texture(int tex);
void ds3_tex_parameter(int tex, int par, int val);
int ds3_tex_image(int tex, int fmt, int xsz, int ysz, void *pixels);

int32_t x16div(int32_t a, int32_t b);

#endif	/* DS3_H_ */
