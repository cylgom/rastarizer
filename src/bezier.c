#include "rastarizer.h"
#include "math32.h"
#include <stdint.h>

// quadratic bezier

struct bezier_quad
{
	int16_t ax;
	int16_t ay;
	int16_t bx;
	int16_t by;
	int16_t mx;
	int16_t my;
	// tangent vectors
	int16_t tax;
	int16_t tay;
	int16_t tbx;
	int16_t tby;
};

// TODO finish this
void ras_quad(
	struct ras_buf ras,
	int16_t ax,
	int16_t ay,
	int16_t bx,
	int16_t by,
	int16_t mx,
	int16_t my)
{
}

// cubic bezier

static int16_t max_dist(int16_t a, int16_t b, int16_t c, int16_t d)
{
	int16_t max;
	int16_t min;

	if (a > b)
	{
		min = b;
		max = a;
	}
	else
	{
		min = a;
		max = b;
	}

	if (c > max)
	{
		max = c;
	}
	else if (c < min)
	{
		min = c;
	}

	if (d > max)
	{
		max = d;
	}
	else if (d < min)
	{
		min = d;
	}

	return max - min;
}

static int8_t grad_cubic(
	int16_t a,
	int16_t b,
	int16_t c,
	int16_t d,
	int16_t timebase,
	int16_t* times,
	int8_t count)
{
	int16_t a2 = a - (3 * b) + (3 * c) - d;
	int16_t delta =
		- (a * c)
		+ (a * d)
		+ (b * b)
		- (b * c)
		- (b * d)
		+ (c * c);

	if (delta > 0)
	{
		int16_t sqrt_delta = isqrt(delta);
		// 2 direction changes
		times[count + 0] = (a - (2 * b) + c + sqrt_delta) * timebase / a2;
		times[count + 1] = (a - (2 * b) + c - sqrt_delta) * timebase / a2;
		count += 2;
	}
	else if (delta == 0)
	{
		// 1 direction change
		times[count] = (a - (2 * b) + c) * timebase / a2;
		count += 1;
	}

	return count;
}

static int8_t infl_cubic(
	int16_t ax,
	int16_t ay,
	int16_t bx,
	int16_t by,
	int16_t cx,
	int16_t cy,
	int16_t dx,
	int16_t dy,
	int16_t timebase,
	int16_t* times,
	int8_t count)
{
	int16_t t1 = timebase * (ax + (2 * bx) - cx) / (dx + cx - bx - ax);
	int16_t t2 = timebase * (ay + (2 * by) - cy) / (dy + cy - by - ay);

	if (t1 == t2)
	{
		times[count] = t1;
		++count;
	}

	return count;
}

static inline int16_t bezier_cubic(
	int16_t a,
	int16_t b,
	int16_t c,
	int16_t d,
	int16_t t,
	int16_t base)
{
	int16_t t_1 = base - t;

	return (a * (t_1 * t_1 * t_1)
		+ b * 3 * (t_1 * t_1) * t
		+ c * 3 * t_1 * (t * t)
		+ d * (t * t * t)) / (base * base * base);
}

static inline int16_t bezier_cubic_tan(
	int16_t a,
	int16_t b,
	int16_t c,
	int16_t d,
	int16_t t,
	int16_t base)
{
	int16_t t_1 = 1 - t;

	return (3 * (t_1 * t_1) * (b - a)
		+ 6 * t_1 * t * (c - b)
		+ 3 * (t * t) * (d - c)) / (base * base);
}

void ras_cubic(
	struct ras_buf ras,
	int16_t ax,
	int16_t ay,
	int16_t bx,
	int16_t by,
	int16_t cx,
	int16_t cy,
	int16_t dx,
	int16_t dy)
{
	int16_t times[3];
	int8_t count = 0;
	int16_t timebase = 2 * max_dist(ax, bx, cx, dx);
	int8_t i;

	// find splitting times (max 3 overall)
	// x direction change (max 2)
	count = grad_cubic(ax, bx, cx, dx, timebase, times, count);
	// y direction change (max 2)
	count = grad_cubic(ay, by, cy, dy, timebase, times, count);
	// inflexion point (max 1)
	count = infl_cubic(ax, ay, bx, by, cx, cy, dx, dy, timebase, times, count);

	// sort times
	uint16_t t;
	// max 3 cycles
	for (i = 0; i < count; ++i)
	{
		if (times[i] > times[i + 1])
		{
			t = times[i];
			times[i] = times[i + 1];
			times[i + 1] = t;
		}
	}

	// generate perfect quad bezier substitutes
	struct bezier_quad quads[4];
	// copy starting node from cubic bezier
	quads[0].ax = ax;
	quads[0].ay = ay;
	quads[0].tax = bezier_cubic_tan(ax, bx, cx, dx, 0, timebase);
	quads[0].tay = bezier_cubic_tan(ay, by, cy, dy, 0, timebase);
	// intersection variables
	int16_t mu;
	int16_t mu_div;
	// max 3 cycles
	for (i = 0; i < count; ++i)
	{
		// set current quad bezier ending point
		quads[i].bx = bezier_cubic(ax, bx, cx, dx, times[i], timebase);
		quads[i].by = bezier_cubic(ay, by, cy, dy, times[i], timebase);
		quads[i].tbx = bezier_cubic_tan(ax, bx, cx, dx, times[i], timebase);
		quads[i].tby = bezier_cubic_tan(ay, by, cy, dy, times[i], timebase);
		// set next quad bezier starting point
		quads[i + 1].ax = quads[i].bx;
		quads[i + 1].ay = quads[i].by;
		quads[i + 1].tax = quads[i].tbx;
		quads[i + 1].tay = quads[i].tby;
		// generate quad bezier handles
		mu = quads[i].tax * (quads[i].by - quads[i].ay) - quads[i].tay * (quads[i].bx - quads[i].ax);
		mu_div = quads[i].tay * quads[i].tbx - quads[i].tax * quads[i].tby;
		// we don't need to check for intersection
		quads[i].mx = quads[i].bx + mu * quads[i].tbx / mu_div;
		quads[i].my = quads[i].by + mu * quads[i].tby / mu_div;
	}
	// copy ending node from cubic bezier and process last quad bezier 
	quads[i].bx = dx;
	quads[i].by = dy;
	quads[i].tbx = bezier_cubic_tan(ax, bx, cx, dx, timebase, timebase);
	quads[i].tby = bezier_cubic_tan(ay, by, cy, dy, timebase, timebase);
	mu = quads[i].tax * (quads[i].by - quads[i].ay) - quads[i].tay * (quads[i].bx - quads[i].ax);
	mu_div = quads[i].tay * quads[i].tbx - quads[i].tax * quads[i].tby;
	quads[i].mx = quads[i].bx + mu * quads[i].tbx / mu_div;
	quads[i].my = quads[i].by + mu * quads[i].tby / mu_div;
	
	// plot quadratic bezier curves
	for (i = 0; i <= count; ++i)
	{
		ras_quad(
			ras,
			quads[i].ax,
			quads[i].ay,
			quads[i].bx,
			quads[i].by,
			quads[i].mx,
			quads[i].my);
	}
}
