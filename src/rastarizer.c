#include "rastarizer.h"
#include "math32.h"

// gamma = 2.2 approximate transformations
// i.e. not exactly sRGB compliant (but close)
inline uint32_t gamma_22(uint32_t x)
{
	uint32_t t = (x << 16) / 0x101;
	uint32_t t3 = (((t * t) >> 16) * t) >> 8; // compensate left-shifting of division

	return t3 / (0x2300 + ((0xDC * t) >> 8));
}

inline uint32_t r_gamma_22(uint32_t x)
{
	uint32_t t = (x << 16) / 0x102; // compensate right-shifting of isqrt

	return (isqrt(t << 14) << 9) / (0xEB00 + ((0x14*t) >> 8));
}

// gamma-ignoring pixel blending
#if 1
void pixel_set(
	struct ras_buf ras,
	uint16_t x,
	uint16_t y,
	uint8_t r,
	uint8_t g,
	uint8_t b,
	uint8_t a)
{
	uint8_t* p = ras.buf + (y * ras.width + x) * 4;
	uint8_t a_dst = p[3];
	uint8_t a_out = (((uint32_t) a_dst) * (0xFF - a) / 0xFF) + a;

	if (a_out > 0)
	{
		p[0] = ((((uint32_t) r) * a) + ((((uint32_t) p[0]) * a_dst * (0xFF - a)) / 0xFF)) / a_out;
		p[1] = ((((uint32_t) g) * a) + ((((uint32_t) p[1]) * a_dst * (0xFF - a)) / 0xFF)) / a_out;
		p[2] = ((((uint32_t) b) * a) + ((((uint32_t) p[2]) * a_dst * (0xFF - a)) / 0xFF)) / a_out;
	}

	p[3] = a_out;
}
#endif

// gamma-aware pixel blending
#if 0
void pixel_set(
	struct ras_buf ras,
	uint16_t x,
	uint16_t y,
	uint8_t r,
	uint8_t g,
	uint8_t b,
	uint8_t a)
{
	uint8_t* p = ras.buf + (y * ras.width + x) * 4;
	uint8_t a_dst = p[3];
	uint8_t a_out = (((uint32_t) a_dst) * (0xFF - a) / 0xFF) + a;

	if (a_out > 0)
	{
		p[0] = r_gamma_22(((gamma_22(r) * a) + ((gamma_22(p[0]) * a_dst * (0xFF - a)) / 0xFF)) / a_out);
		p[1] = r_gamma_22(((gamma_22(g) * a) + ((gamma_22(p[1]) * a_dst * (0xFF - a)) / 0xFF)) / a_out);
		p[2] = r_gamma_22(((gamma_22(b) * a) + ((gamma_22(p[2]) * a_dst * (0xFF - a)) / 0xFF)) / a_out);
	}

	p[3] = a_out;
}
#endif
