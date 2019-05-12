#include "rastarizer.h"

void pixel_blend(
	struct ras_buf ras,
	uint16_t x,
	uint16_t y,
	uint8_t r,
	uint8_t g,
	uint8_t b,
	uint8_t a)
{
}

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

	*p = r;
	++p;
	*p = g;
	++p;
	*p = b;
	++p;
	*p = a;
}
