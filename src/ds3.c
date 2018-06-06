#include <stdint.h>
#include <math.h>
#include "dsregs.h"
#include "ds.h"
#include "ds3.h"
#include "ds3impl.h"

void ds3_init(void)
{
	while(REG_GXSTAT & GXSTAT_BUSY);
	REG_GXSTAT |= GXSTAT_CLEAR_FIFO;
	/* initialize zbuffer mode */
	REG_SWAP_BUFFERS = 0;
	/* clear display 3D control register */
	REG_DISP3DCNT = 0;
}

void ds3_enable(unsigned int x)
{
	REG_DISP3DCNT |= x;
}

void ds3_disable(unsigned int x)
{
	REG_DISP3DCNT &= ~x;
}

void ds3_clear_color(uint16_t color, int a)
{
	REG_CLEAR_COLOR = color | ((a & 0x1f) << 16);
}

void ds3_clear_depth(int z)
{
	REG_CLEAR_DEPTH = z;
}

void ds3_viewport(int x, int y, int w, int h)
{
	int x1 = x + w - 1;
	int y1 = y + h - 1;

	if(x1 > 255) x1 = 255;
	if(y1 > 191) y1 = 191;

	REG_VIEWPORT = x | (y << 8) | (x1 << 16) | (y1 << 24);
}

void ds3_matrix_mode(int mmode)
{
	REG_MTX_MODE = mmode;
}

void ds3_load_identity(void)
{
	REG_MTX_IDENTITY = 0;
}

void ds3_load_matrix(int32_t *m)
{
	int i;
	for(i=0; i<16; i++) {
		int16_t val = (int16_t)(*m++ >> 4);
		REG_MTX_LOAD_4X4 = val;
	}
}

void ds3_load_matrix4x3(int32_t *m)
{
	int i;
	for(i=0; i<12; i++) {
		int16_t val = (int16_t)(*m++ >> 4);
		REG_MTX_LOAD_4X3 = val;
	}
}

void ds3_mult_matrix(int32_t *m)
{
	int i;
	for(i=0; i<16; i++) {
		int16_t val = (int16_t)(*m++ >> 4);
		REG_MTX_MULT_4X4 = val;
	}
}

void ds3_mult_matrix4x3(int32_t *m)
{
	int i;
	for(i=0; i<12; i++) {
		int16_t val = (int16_t)(*m++ >> 4);
		REG_MTX_MULT_4X3 = val;
	}
}

void ds3_mult_matrix3x3(int32_t *m)
{
	int i;
	for(i=0; i<9; i++) {
		int16_t val = (int16_t)(*m++ >> 4);
		REG_MTX_MULT_3X3 = val;
	}
}

void ds3_push_matrix(void)
{
	REG_MTX_PUSH = 0;
}

void ds3_pop_matrix(void)
{
	REG_MTX_POP = 1;
}

void ds3_translate(int32_t x, int32_t y, int32_t z)
{
	REG_MTX_TRANS = (int16_t)(x >> 4);
	REG_MTX_TRANS = (int16_t)(y >> 4);
	REG_MTX_TRANS = (int16_t)(z >> 4);
}

void ds3_scale(int32_t x, int32_t y, int32_t z)
{
	REG_MTX_SCALE = (int16_t)(x >> 4);
	REG_MTX_SCALE = (int16_t)(y >> 4);
	REG_MTX_SCALE = (int16_t)(z >> 4);
}

/* TODO:
 * 0     Translucent polygon Y-sorting (0=Auto-sort, 1=Manual-sort)
 * 1     Depth Buffering  (0=With Z-value, 1=With W-value)
 */
void ds3_swap_buffers(void)
{
	REG_SWAP_BUFFERS = 1;
}

void ds3_begin(int prim)
{
	REG_BEGIN_VTXS = prim;
}

void ds3_end(void)
{
	REG_END_VTXS = 0;
}

void ds3_vertex3(int32_t x, int32_t y, int32_t z)
{
	REG_VTX_16 = ((x >> 4) & 0xffff) | ((y << 12) & 0xffff0000);
	REG_VTX_16 = (z >> 4) & 0xffff;
}

void ds3_vertex3f(float x, float y, float z)
{
	ds3_vertex3((int32_t)(x * 65536.0f), (int32_t)(y * 65536.0f), (int32_t)(z * 65536.0f));
}

void ds3_vertex2(int32_t x, int32_t y)
{
	REG_VTX_XY = ((x >> 4) & 0xffff) | ((y << 12) & 0xffff0000);
}

void ds3_vertex2f(float x, float y)
{
	ds3_vertex2((int32_t)(x * 65536.0f), (int32_t)(y * 65536.0f));
}

void ds3_color(uint16_t color)
{
	REG_COLOR = color;
}

void ds3_color3b(unsigned char r, unsigned char g, unsigned char b)
{
	REG_COLOR = RGB15(r >> 3, g >> 3, b >> 3);
}

void ds3_color3f(float r, float g, float b)
{
	uint16_t ir = r * 31.0f;
	uint16_t ig = g * 31.0f;
	uint16_t ib = b * 31.0f;
	REG_COLOR = RGB15(ir, ig, ib);
}

void ds3_normal(int32_t x, int32_t y, int32_t z)
{
	REG_NORMAL = ((x >> 7) & 0x3ff) | ((y << 17) & 0xffc00) | ((z << 27) & 0x3ff00000);
}

void ds3_normal3f(float x, float y, float z)
{
	ds3_normal((int32_t)(x * 65536.0f), (int32_t)(y * 65536.0f), (int32_t)(z * 65536.0f));
}

/* DS texcoords are in texel space 12.4 fixed point
 * convert 16.16 [0, 1] to 12.4 [0, texsize]
 */

void ds3_texcoord2(int32_t s, int32_t t)
{
	int sx = 0, sy = 0;
	struct texture *tex = ds3_priv_current_texture();
	if(!tex) return;

	sx = ds3_priv_tex_width_shift(tex);
	sy = ds3_priv_tex_height_shift(tex);

	s >>= 12 - sx;
	t >>= 12 - sy;

	REG_TEXCOORD = (s & 0xffff) | ((t << 16) & 0xffff0000);
}

void ds3_texcoord2f(float s, float t)
{
	ds3_texcoord2((int32_t)(s * 65536.0f), (int32_t)(t * 65536.0f));
}

void ds3_ortho(int32_t left, int32_t right, int32_t top, int32_t bottom, int32_t znear, int32_t zfar)
{
	int32_t m[16] = {0};
	int32_t dx = right - left;
	int32_t dy = top - bottom;
	int32_t dz = zfar - znear;

	m[0] = x16div(0x20000, dx);
	m[5] = x16div(0x20000, dy);
	m[10] = x16div(-0x20000, dz);
	m[12] = x16div(-(right + left), dx);
	m[13] = x16div(-(top + bottom), dy);
	m[14] = x16div(-(zfar + znear), dz);
	m[15] = 65536;

	ds3_mult_matrix(m);
}

void ds3_orthof(float left, float right, float top, float bottom, float znear, float zfar)
{
	int32_t m[16] = {0};
	float dx = right - left;
	float dy = top - bottom;
	float dz = zfar - znear;

	m[0] = (int32_t)(2.0f / dx * 65536.0f);
	m[5] = (int32_t)(2.0f / dy * 65536.0f);
	m[10] = (int32_t)(-2.0f / dz * 65536.0f);
	m[12] = (int32_t)(-(right + left) / dx * 65536.0f);
	m[13] = (int32_t)(-(top + bottom) / dy * 65536.0f);
	m[14] = (int32_t)(-(zfar + znear) / dz * 65536.0f);
	m[15] = 65536;

	ds3_mult_matrix(m);
}

void ds3_frustum(int32_t left, int32_t right, int32_t top, int32_t bottom, int32_t znear, int32_t zfar)
{
	int32_t m[16] = {0};

	int32_t dx = right - left;
	int32_t dy = top - bottom;
	int32_t dz = zfar - znear;

	int32_t a = x16div(right + left, dx);
	int32_t b = x16div(top + bottom, dy);
	int32_t c = x16div(-(zfar + znear), dz);
	int32_t d = x16div(-((zfar >> 8) * znear) << 1, dz);

	m[0] = x16div(znear << 1, dx);
	m[5] = x16div(znear << 1, dy);
	m[8] = a;
	m[9] = b;
	m[10] = c;
	m[11] = -65536;
	m[14] = d;

	ds3_mult_matrix(m);
}

void g3d_frustumf(float left, float right, float bottom, float top, float nr, float fr)
{
	int32_t m[16] = {0};

	float dx = right - left;
	float dy = top - bottom;
	float dz = fr - nr;

	float a = (right + left) / dx;
	float b = (top + bottom) / dy;
	float c = -(fr + nr) / dz;
	float d = -2.0 * fr * nr / dz;

	m[0] = (int32_t)(2.0 * nr / dx * 65536.0f);
	m[5] = (int32_t)(2.0 * nr / dy * 65536.0f);
	m[8] = (int32_t)(a * 65536.0f);
	m[9] = (int32_t)(b * 65536.0f);
	m[10] = (int32_t)(c * 65536.0f);
	m[11] = -65536;
	m[14] = (int32_t)(d * 65536.0f);

	ds3_mult_matrix(m);
}

void ds3_perspectivef(float vfov_deg, float aspect, float znear, float zfar)
{
	int32_t m[16] = {0};

	float vfov = M_PI * vfov_deg / 180.0f;
	float s = 1.0f / tan(vfov * 0.5f);
	float range = znear - zfar;

	m[0] = (int32_t)(s / aspect * 65536.0f);
	m[5] = (int32_t)(s * 65536.0f);
	m[10] = (int32_t)((znear + zfar) / range * 65536.0f);
	m[11] = -65536;
	m[14] = (int32_t)(2.0f * znear * zfar / range * 65536.0f);

	ds3_mult_matrix(m);
}

int32_t x16div(int32_t a, int32_t b)
{
	REG_DIVCNT = DIVCNT_64_32;
	REG_DIV_NUMER = (int64_t)a << 16;
	REG_DIV_DENOM = (int64_t)b;

	while(REG_DIVCNT & DIVCNT_BUSY);
	return (int32_t)REG_DIV_RESULT;
}
