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
		// update alpha value
		a = (isqrt((r2 - x * x) << 16) + 0x80) & 0xFF;

		if (p < 0)
		{
			p += 2 * x + 1;
		}
		else
		{
			// force opacity on the first pixel of the second slab
			if (y == r)
			{
				a = 0xFF;
			}

			y--;
			p += 2 * (x - y) + 1;

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
