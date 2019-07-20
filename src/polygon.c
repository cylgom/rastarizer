#include "rastarizer.h"
#include "math32.h"
#include "fixmath.h"
#include <stdint.h>

// taken from rosetta code and modified to use fixed-point arithmetic
// doesn't compensate the fixed-point multiplication rounding error
void ras_precision_line(
	struct ras_buf ras,
	int32_t x1,
	int32_t y1,
	int32_t x2,
	int32_t y2)
{
	int32_t dx = x2 - x1;
	int32_t dy = y2 - y1;

	if (abs32(dx) > abs32(dy))
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
		int32_t gradient;
		
		// set the gradient to maximum if we divide by zero
		if (dx == 0)
		{
			gradient = 0x7FFFFFFF;
		}
		else
		{
			gradient = (((int64_t) dy) << 16) / dx;
		}

		int32_t xend = round32(x1);
		int32_t yend = y1 + ((gradient * (xend - x1)) >> 16);
		int32_t xgap = r_frac_part(x1 + 0x8000);

		int32_t xpxl1 = xend >> 16;
		int32_t ypxl1 = int_part(yend) >> 16;

		pixel_set(ras, xpxl1, ypxl1, 0x00, 0x00, 0x00, (r_frac_part(yend) * xgap) >> 24);
		pixel_set(ras, xpxl1, ypxl1 + 1, 0x00, 0x00, 0x00, (frac_part(yend) * xgap) >> 24);

		// end
		int32_t intery = yend + gradient;
		xend = round32(x2);
		yend = y2 + ((gradient * (xend - x2)) >> 16);
		xgap = frac_part(x2 + 0x8000);

		int32_t xpxl2 = xend >> 16;
		int32_t ypxl2 = int_part(yend) >> 16;

		pixel_set(ras, xpxl2, ypxl2, 0x00, 0x00, 0x00, (r_frac_part(yend) * xgap) >> 24);
		pixel_set(ras, xpxl2, ypxl2 + 1, 0x00, 0x00, 0x00, (frac_part(yend) * xgap) >> 24);

		// middle
		for (int32_t x = xpxl1 + 1; x < xpxl2; ++x)
		{
			pixel_set(ras, x, int_part(intery) >> 16, 0x00, 0x00, 0x00, r_frac_part(intery) >> 8);
			pixel_set(ras, x, (int_part(intery) >> 16) + 1, 0x00, 0x00, 0x00, frac_part(intery) >> 8);
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
		int32_t gradient;
		
		// set the gradient to maximum if we divide by zero
		if ((dy >> 16) == 0)
		{
			gradient = 0x7FFFFFFF;
		}
		else
		{
			gradient = (((int64_t) dx) << 16) / dy;
		}

		int32_t yend = round32(y1);
		int32_t xend = x1 + ((gradient * (yend - y1)) >> 16);
		int32_t ygap = r_frac_part(y1 + 0x8000);

		int32_t ypxl1 = yend >> 16;
		int32_t xpxl1 = int_part(xend) >> 16;

		pixel_set(ras, xpxl1, ypxl1, 0x00, 0x00, 0x00, (r_frac_part(xend) * ygap) >> 24);
		pixel_set(ras, xpxl1 + 1, ypxl1, 0x00, 0x00, 0x00, (frac_part(xend) * ygap) >> 24);

		// end
		int32_t interx = xend + gradient;
		yend = round32(y2);
		xend = x2 + ((gradient * (yend - y2)) >> 16);
		ygap = frac_part(y2 + 0x8000);

		int32_t ypxl2 = yend >> 16;
		int32_t xpxl2 = int_part(xend) >> 16;

		pixel_set(ras, xpxl2, ypxl2, 0x00, 0x00, 0x00, (r_frac_part(xend) * ygap) >> 24);
		pixel_set(ras, xpxl2 + 1, ypxl2, 0x00, 0x00, 0x00, (frac_part(xend) * ygap) >> 24);

		// middle
		for (int32_t y = ypxl1 + 1; y < ypxl2; ++y)
		{
			pixel_set(ras, int_part(interx) >> 16, y, 0x00, 0x00, 0x00, r_frac_part(interx) >> 8);
			pixel_set(ras, (int_part(interx) >> 16) + 1, y, 0x00, 0x00, 0x00, frac_part(interx) >> 8);
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
	ras_precision_line(ras, x1 << 16, y1 << 16, x2 << 16, y2 << 16);
}
