#include "rastarizer.h"
#include "math32.h"
#include <stdint.h>

// midpoint circle algorithm with anti-aliasing (Xiaolin Wu)
// implemented with integer square root using point shifting
void ras_disk(
	struct ras_buf ras,
	uint16_t ox,
	uint16_t oy,
	uint16_t r)
{
	int16_t x = 0;
	int16_t y = r;
	int16_t p = (5 - r * 4) / 4;

	uint32_t r2 = r * r;
	uint8_t a;
	int16_t i;

	// fill center, top, bottom, left, right pixels
	pixel_set(ras, ox,     oy, 0x00, 0x00, 0x00, 0xFF);
	pixel_set(ras, ox, oy + y, 0x00, 0x00, 0x00, 0x80);
	pixel_set(ras, ox, oy - y, 0x00, 0x00, 0x00, 0x80);
	pixel_set(ras, ox + y, oy, 0x00, 0x00, 0x00, 0x80);
	pixel_set(ras, ox - y, oy, 0x00, 0x00, 0x00, 0x80);

	// loop offset by one to skip center pixel
	for (x = 1; x < y; ++x)
	{
		if (p < 0)
		{
			p += 2 * x + 1;

			// update alpha value
			a = (isqrt((r2 - x * x) << 16) + 0x80) & 0xFF;
		}
		else
		{
			y--;
			p += 2 * (x - y) + 1;

			// fix opacity rounding
			a = (isqrt((r2 - x * x - 1) << 16) + 0x80) & 0xFF;

			// fill vertical and horizontal diameters (from top)
			pixel_set(ras, ox, oy + y, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, ox, oy - y, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, ox + y, oy, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, ox - y, oy, 0x00, 0x00, 0x00, 0xFF);
		}

		// fill vertical and horizontal diameters (from bottom)
		pixel_set(ras, ox, oy + x, 0x00, 0x00, 0x00, 0xFF);
		pixel_set(ras, ox, oy - x, 0x00, 0x00, 0x00, 0xFF);
		pixel_set(ras, ox + x, oy, 0x00, 0x00, 0x00, 0xFF);
		pixel_set(ras, ox - x, oy, 0x00, 0x00, 0x00, 0xFF);

		// fill diagonal diameters
		// will waste 4 pixels in some rare cases where
		// the extrema are overwritten as part of the border
		pixel_set(ras, ox + x, oy + x, 0x00, 0x00, 0x00, 0xFF);
		pixel_set(ras, ox + x, oy - x, 0x00, 0x00, 0x00, 0xFF);
		pixel_set(ras, ox - x, oy + x, 0x00, 0x00, 0x00, 0xFF);
		pixel_set(ras, ox - x, oy - x, 0x00, 0x00, 0x00, 0xFF);

		// draw anti-aliased border
		pixel_set(ras, ox + x, oy + y, 0x00, 0x00, 0x00, a);
		pixel_set(ras, ox - x, oy + y, 0x00, 0x00, 0x00, a);
		pixel_set(ras, ox + x, oy - y, 0x00, 0x00, 0x00, a);
		pixel_set(ras, ox - x, oy - y, 0x00, 0x00, 0x00, a);
		pixel_set(ras, ox + y, oy + x, 0x00, 0x00, 0x00, a);
		pixel_set(ras, ox - y, oy + x, 0x00, 0x00, 0x00, a);
		pixel_set(ras, ox + y, oy - x, 0x00, 0x00, 0x00, a);
		pixel_set(ras, ox - y, oy - x, 0x00, 0x00, 0x00, a);

		// fill circle
		for (i = x + 1; i < y; ++i)
		{
			pixel_set(ras, ox + x, oy + i, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, ox - x, oy - i, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, ox + x, oy - i, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, ox - x, oy + i, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, ox + i, oy + x, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, ox - i, oy - x, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, ox + i, oy - x, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, ox - i, oy + x, 0x00, 0x00, 0x00, 0xFF);
		}
	}
}

void ras_rounded_rectangle(
	struct ras_buf ras,
	uint16_t x1,
	uint16_t x2,
	uint16_t y1,
	uint16_t y2,
	uint16_t r)
{
	int16_t x = 0;
	int16_t y = r;
	int16_t p = (5 - r * 4) / 4;

	uint32_t r2 = r * r;
	uint8_t a;
	int16_t i;

	for (x = 1; x < y; ++x)
	{
		if (p < 0)
		{
			p += 2 * x + 1;
			a = (isqrt((r2 - x * x) << 16) + 0x80) & 0xFF;
		}
		else
		{
			y--;
			p += 2 * (x - y) + 1;
			a = (isqrt((r2 - x * x - 1) << 16) + 0x80) & 0xFF;
		}

		pixel_set(ras, x2 + x, y2 + x, 0x00, 0x00, 0x00, 0xFF);
		pixel_set(ras, x2 + x, y1 - x, 0x00, 0x00, 0x00, 0xFF);
		pixel_set(ras, x1 - x, y2 + x, 0x00, 0x00, 0x00, 0xFF);
		pixel_set(ras, x1 - x, y1 - x, 0x00, 0x00, 0x00, 0xFF);

		pixel_set(ras, x2 + x, y2 + y, 0x00, 0x00, 0x00, a);
		pixel_set(ras, x1 - x, y2 + y, 0x00, 0x00, 0x00, a);
		pixel_set(ras, x2 + x, y1 - y, 0x00, 0x00, 0x00, a);
		pixel_set(ras, x1 - x, y1 - y, 0x00, 0x00, 0x00, a);
		pixel_set(ras, x2 + y, y2 + x, 0x00, 0x00, 0x00, a);
		pixel_set(ras, x1 - y, y2 + x, 0x00, 0x00, 0x00, a);
		pixel_set(ras, x2 + y, y1 - x, 0x00, 0x00, 0x00, a);
		pixel_set(ras, x1 - y, y1 - x, 0x00, 0x00, 0x00, a);

		for (i = x + 1; i < y; ++i)
		{
			pixel_set(ras, x2 + x, y2 + i, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, x1 - x, y1 - i, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, x2 + x, y1 - i, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, x1 - x, y2 + i, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, x2 + i, y2 + x, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, x1 - i, y1 - x, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, x2 + i, y1 - x, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, x1 - i, y2 + x, 0x00, 0x00, 0x00, 0xFF);
		}
	}

	int16_t k;

	for (k = y1; k < (y2 + 1); ++k)
	{
		for (i = (x1 - r + 1); i < (x2 + r); ++i)
		{
			pixel_set(ras, i, k, 0x00, 0x00, 0x00, 0xFF);
		}

		// this code performs an accurate extension of rounded
		// corners by adding lines of semi-transparent pixels on the
		// sides of the rectangle (which actually doesn't look good)
		pixel_set(ras, x1 - r, k, 0x00, 0x00, 0x00, 0x80);
		pixel_set(ras, x2 + r, k, 0x00, 0x00, 0x00, 0x80);
	}

	for (i = x1; i < (x2 + 1); ++i)
	{
		for (k = 0; k < r; ++k)
		{
			pixel_set(ras, i, y1 - k, 0x00, 0x00, 0x00, 0xFF);
			pixel_set(ras, i, y2 + k, 0x00, 0x00, 0x00, 0xFF);
		}

		pixel_set(ras, i, y1 - r, 0x00, 0x00, 0x00, 0x80);
		pixel_set(ras, i, y2 + r, 0x00, 0x00, 0x00, 0x80);
	}
}

void ras_ring(
	struct ras_buf ras,
	uint16_t ox,
	uint16_t oy,
	uint16_t ro,
	uint16_t ri)
{
	int16_t x = 0;

	int16_t y = ro;
	int16_t p = (5 - ro * 4) / 4;

	int16_t yi = ri;
	int16_t pi = (5 - ri * 4) / 4;

	uint8_t a;
	uint16_t i;

	uint32_t r2 = ro * ro;
	uint32_t ri2 = ri * ri;

	pixel_set(ras, ox, oy + y, 0x00, 0x00, 0x00, 0x80);
	pixel_set(ras, ox, oy - y, 0x00, 0x00, 0x00, 0x80);
	pixel_set(ras, ox + y, oy, 0x00, 0x00, 0x00, 0x80);
	pixel_set(ras, ox - y, oy, 0x00, 0x00, 0x00, 0x80);

	pixel_set(ras, ox, oy + yi, 0x00, 0x00, 0x00, 0x80);
	pixel_set(ras, ox, oy - yi, 0x00, 0x00, 0x00, 0x80);
	pixel_set(ras, ox + yi, oy, 0x00, 0x00, 0x00, 0x80);
	pixel_set(ras, ox - yi, oy, 0x00, 0x00, 0x00, 0x80);

	for (x = 1; x < y; ++x)
	{
		if (p < 0)
		{
			p += 2 * x + 1;
			a = (isqrt((r2 - x * x) << 16) + 0x80);
		}
		else
		{
			y--;
			p += 2 * (x - y) + 1;
			a = (isqrt((r2 - x * x - 1) << 16) + 0x80);
		}

		pixel_set(ras, ox + x, oy + y, 0x00, 0x00, 0x00, a);
		pixel_set(ras, ox - x, oy + y, 0x00, 0x00, 0x00, a);
		pixel_set(ras, ox + x, oy - y, 0x00, 0x00, 0x00, a);
		pixel_set(ras, ox - x, oy - y, 0x00, 0x00, 0x00, a);
		pixel_set(ras, ox + y, oy + x, 0x00, 0x00, 0x00, a);
		pixel_set(ras, ox - y, oy + x, 0x00, 0x00, 0x00, a);
		pixel_set(ras, ox + y, oy - x, 0x00, 0x00, 0x00, a);
		pixel_set(ras, ox - y, oy - x, 0x00, 0x00, 0x00, a);

		if (x < yi)
		{
			if (pi < 0)
			{
				pi += 2 * x + 1;
				a = 0xFF - ((isqrt((ri2 - x * x) << 16) + 0x80));
			}
			else
			{
				yi--;
				pi += 2 * (x - yi) + 1;
				a = 0xFF - ((isqrt((ri2 - x * x - 1) << 16) + 0x80));
			}

			pixel_set(ras, ox + x, oy + yi, 0x00, 0x00, 0x00, a);
			pixel_set(ras, ox - x, oy + yi, 0x00, 0x00, 0x00, a);
			pixel_set(ras, ox + x, oy - yi, 0x00, 0x00, 0x00, a);
			pixel_set(ras, ox - x, oy - yi, 0x00, 0x00, 0x00, a);
			pixel_set(ras, ox + yi, oy + x, 0x00, 0x00, 0x00, a);
			pixel_set(ras, ox - yi, oy + x, 0x00, 0x00, 0x00, a);
			pixel_set(ras, ox + yi, oy - x, 0x00, 0x00, 0x00, a);
			pixel_set(ras, ox - yi, oy - x, 0x00, 0x00, 0x00, a);

			for (i = yi + 1; i < y; ++i)
			{
				pixel_set(ras, ox + x, oy + i, 0x00, 0x00, 0x00, 0xFF);
				pixel_set(ras, ox - x, oy + i, 0x00, 0x00, 0x00, 0xFF);
				pixel_set(ras, ox + x, oy - i, 0x00, 0x00, 0x00, 0xFF);
				pixel_set(ras, ox - x, oy - i, 0x00, 0x00, 0x00, 0xFF);
				pixel_set(ras, ox + i, oy + x, 0x00, 0x00, 0x00, 0xFF);
				pixel_set(ras, ox - i, oy + x, 0x00, 0x00, 0x00, 0xFF);
				pixel_set(ras, ox + i, oy - x, 0x00, 0x00, 0x00, 0xFF);
				pixel_set(ras, ox - i, oy - x, 0x00, 0x00, 0x00, 0xFF);
			}
		}
		else
		{
			for (i = x; i < y; ++i)
			{
				pixel_set(ras, ox + x, oy + i, 0x00, 0x00, 0x00, 0xFF);
				pixel_set(ras, ox - x, oy + i, 0x00, 0x00, 0x00, 0xFF);
				pixel_set(ras, ox + x, oy - i, 0x00, 0x00, 0x00, 0xFF);
				pixel_set(ras, ox - x, oy - i, 0x00, 0x00, 0x00, 0xFF);
				pixel_set(ras, ox + i, oy + x, 0x00, 0x00, 0x00, 0xFF);
				pixel_set(ras, ox - i, oy + x, 0x00, 0x00, 0x00, 0xFF);
				pixel_set(ras, ox + i, oy - x, 0x00, 0x00, 0x00, 0xFF);
				pixel_set(ras, ox - i, oy - x, 0x00, 0x00, 0x00, 0xFF);
			}
		}
	}

	for (i = ri + 1; i < ro; ++i)
	{
		pixel_set(ras, ox + i, oy, 0x00, 0x00, 0x00, 0xFF);
		pixel_set(ras, ox - i, oy, 0x00, 0x00, 0x00, 0xFF);
		pixel_set(ras, ox, oy + i, 0x00, 0x00, 0x00, 0xFF);
		pixel_set(ras, ox, oy - i, 0x00, 0x00, 0x00, 0xFF);
	}
}
