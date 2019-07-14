#include "rastarizer.h"
#include "math32.h"
#include <stdint.h>

// fixed-point utils for the line function
static inline int32_t abs(int32_t n)
{
	if (n < 0)
	{
		return -n;
	}
	else
	{
		return n;
	}
}

static inline int32_t frac_part(int32_t n)
{
	return n & 0xFF;
}

static inline int32_t r_frac_part(int32_t n)
{
	return 0xFF - frac_part(n);
}

static inline int32_t int_part(int32_t n)
{
	return n & 0xFFFFFF00;
}

static inline int32_t round32(int32_t n)
{
	if ((n & 0xFF) > 0x80)
	{
		return (n + 0x100) & 0xFFFFFF00;
	}
	else
	{
		return n & 0xFFFFFF00;
	}
}

// doesn't fix the fixed-point multiplication rounding error
void ras_precision_line(
	struct ras_buf ras,
	int32_t x1,
	int32_t y1,
	int32_t x2,
	int32_t y2)
{
	int32_t dx = x2 - x1;
	int32_t dy = y2 - y1;

	if (abs(dx) > abs(dy))
	{
		int32_t tmp;

		if (x2 < x1)
		{
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}

		// start
		int32_t gradient = dy / (dx >> 8);
		int32_t xend = round32(x1);
		int32_t yend = y1 + ((gradient * (xend - x1)) >> 8);
		int32_t xgap = r_frac_part(x1 + 0x80);

		int32_t xpxl1 = xend >> 8;
		int32_t ypxl1 = int_part(yend) >> 8;

		pixel_set(ras, xpxl1, ypxl1, 0x00, 0x00, 0x00, (r_frac_part(yend) * xgap) >> 8);
		pixel_set(ras, xpxl1, ypxl1 + 1, 0x00, 0x00, 0x00, (frac_part(yend) * xgap) >> 8);

		// end
		int32_t intery = yend + gradient;
		xend = round32(x2);
		yend = y2 + ((gradient * (xend - x2)) >> 8);
		xgap = frac_part(x2 + 0x80);

		int32_t xpxl2 = xend >> 8;
		int32_t ypxl2 = int_part(yend) >> 8;

		pixel_set(ras, xpxl2, ypxl2, 0x00, 0x00, 0x00, (r_frac_part(yend) * xgap) >> 8);
		pixel_set(ras, xpxl2, ypxl2 + 1, 0x00, 0x00, 0x00, (frac_part(yend) * xgap) >> 8);

		// middle
		for (int32_t x = xpxl1 + 1; x < xpxl2; ++x)
		{
			pixel_set(ras, x, int_part(intery) >> 8, 0x00, 0x00, 0x00, r_frac_part(intery));
			pixel_set(ras, x, (int_part(intery) >> 8) + 1, 0x00, 0x00, 0x00, frac_part(intery));
			intery += gradient;
		}
	}
	else
	{
		int32_t tmp;

		if (y2 < y1)
		{
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}

		// start
		int32_t gradient = dx / (dy >> 8);
		int32_t yend = round32(y1);
		int32_t xend = x1 + ((gradient * (yend - y1)) >> 8);
		int32_t ygap = r_frac_part(y1 + 0x80);

		int32_t ypxl1 = yend >> 8;
		int32_t xpxl1 = int_part(xend) >> 8;

		pixel_set(ras, xpxl1, ypxl1, 0x00, 0x00, 0x00, (r_frac_part(xend) * ygap) >> 8);
		pixel_set(ras, xpxl1 + 1, ypxl1, 0x00, 0x00, 0x00, (frac_part(xend) * ygap) >> 8);

		// end
		int32_t interx = xend + gradient;
		yend = round32(y2);
		xend = x2 + ((gradient * (yend - y2)) >> 8);
		ygap = frac_part(y2 + 0x80);

		int32_t ypxl2 = yend >> 8;
		int32_t xpxl2 = int_part(xend) >> 8;

		pixel_set(ras, xpxl2, ypxl2, 0x00, 0x00, 0x00, (r_frac_part(xend) * ygap) >> 8);
		pixel_set(ras, xpxl2 + 1, ypxl2, 0x00, 0x00, 0x00, (frac_part(xend) * ygap) >> 8);

		// middle
		for (int32_t y = ypxl1 + 1; y < ypxl2; ++y)
		{
			pixel_set(ras, int_part(interx) >> 8, y, 0x00, 0x00, 0x00, r_frac_part(interx));
			pixel_set(ras, (int_part(interx) >> 8) + 1, y, 0x00, 0x00, 0x00, frac_part(interx));
			interx += gradient;
		}
	}
}

// pixel-perfect line for the user
void ras_line(
	struct ras_buf ras,
	int32_t x1,
	int32_t y1,
	int32_t x2,
	int32_t y2)
{
	ras_precision_line(ras, x1 << 8, y1 << 8, x2 << 8, y2 << 8);
}
