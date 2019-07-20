#include "rastarizer.h"
#include "math32.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// arbitrary number of steps
#define STEPS 50
#define STEPS2 (STEPS * STEPS)
#define STEPS3 (STEPS * STEPS * STEPS)

// taken from Dr.Dobb's journal and modified to use fixed-point arithmetic
void ras_precision_bezier_cubic(
	struct ras_buf ras,
	int32_t p0x,
	int32_t p0y,
	int32_t p1x,
	int32_t p1y,
	int32_t p2x,
	int32_t p2y,
	int32_t p3x,
	int32_t p3y)
{
	// factorize and force operations order to gain precision
	// do not move the operands or develop the products
    int32_t ax = (3 * (p1x - p2x) + p3x) - p0x;
    int32_t ay = (3 * (p1y - p2y) + p3y) - p0y;

    int32_t bx = 3 * ((p0x + p2x) - (2 * p1x));
    int32_t by = 3 * ((p0y + p2y) - (2 * p1y));

    int32_t cx = 3 * (p1x - p0x);
    int32_t cy = 3 * (p1y - p0y);

    int32_t dx = p0x;
    int32_t dy = p0y;

	// starting value
    int32_t pointX = dx;
    int32_t pointY = dy;

	// initial starting point
	int32_t xpxl_old = pointX;
	int32_t ypxl_old = pointY;

    int32_t firstFDX = (ax / STEPS3) + (bx / STEPS2) + (cx / STEPS);
    int32_t firstFDY = (ay / STEPS3) + (by / STEPS2) + (cy / STEPS);

    int32_t secondFDX = ((6 * ax) / STEPS3) + ((2 * bx) / STEPS2);
    int32_t secondFDY = ((6 * ay) / STEPS3) + ((2 * by) / STEPS2);

    int32_t thirdFDX = (6 * ax) / STEPS3;
    int32_t thirdFDY = (6 * ay) / STEPS3;    

    for (int32_t i = 0; i < STEPS; ++i)
	{
        pointX += firstFDX;
        pointY += firstFDY;

        firstFDX += secondFDX;
        firstFDY += secondFDY;

        secondFDX += thirdFDX;
        secondFDY += thirdFDY;

		ras_precision_line(ras, xpxl_old, ypxl_old, pointX, pointY);

		xpxl_old = pointX;
		ypxl_old = pointY;
    }
}

void ras_bezier_cubic(
	struct ras_buf ras,
	int32_t p0x,
	int32_t p0y,
	int32_t p1x,
	int32_t p1y,
	int32_t p2x,
	int32_t p2y,
	int32_t p3x,
	int32_t p3y)
{
	ras_precision_bezier_cubic(
		ras,
		p0x << 16,
		p0y << 16,
		p1x << 16,
		p1y << 16,
		p2x << 16,
		p2y << 16,
		p3x << 16,
		p3y << 16);
}
