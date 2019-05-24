#ifndef H_RASTARIZER
#define H_RASTARIZER

#include <stdint.h>

struct ras_buf
{
	uint8_t *buf;
	uint16_t width;
	uint16_t height;
};

void pixel_blend(
	struct ras_buf ras,
	uint16_t x,
	uint16_t y,
	uint8_t r,
	uint8_t g,
	uint8_t b,
	uint8_t a);

void pixel_set(
	struct ras_buf ras,
	uint16_t x,
	uint16_t y,
	uint8_t r,
	uint8_t g,
	uint8_t b,
	uint8_t a);

void ras_disk(
	struct ras_buf ras,
	uint16_t ox,
	uint16_t oy,
	uint16_t r);

void ras_ring(
	struct ras_buf ras,
	uint16_t ox,
	uint16_t oy,
	uint16_t ro,
	uint16_t ri);

void ras_rounded_rectangle(
	struct ras_buf ras,
	uint16_t x1,
	uint16_t x2,
	uint16_t y1,
	uint16_t y2,
	uint16_t r);

void ras_quad(
	struct ras_buf ras,
	int16_t ax,
	int16_t ay,
	int16_t bx,
	int16_t by,
	int16_t mx,
	int16_t my);

void ras_cubic(
	struct ras_buf ras,
	int16_t ax,
	int16_t ay,
	int16_t bx,
	int16_t by,
	int16_t cx,
	int16_t cy,
	int16_t dx,
	int16_t dy);

#endif
