#include "rastarizer.h"
#include "math32.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

static inline uint16_t bernstein_cubic(
	int16_t a,
	int16_t b,
	int16_t c,
	int16_t d,
	int32_t t,
	int32_t base)
{
	int32_t t_1 = base - t;

	return (a * (t_1 * t_1 * t_1)
		+ b * 3 * (t_1 * t_1) * t
		+ c * 3 * t_1 * (t * t)
		+ d * (t * t * t)) / (base * base * base);
}

static inline void add_time(
	int32_t* t,
	uint8_t* t_count,
	int32_t k,
	int32_t v)
{
	if (v <= k)
	{
		t[*t_count] = v;
		++(*t_count);
	}

	int32_t tmp;

	if ((*t_count == 3) && (t[1] > t[2]))
	{
		tmp = t[2];
		t[2] = t[1];
		t[1] = tmp;
	}
	else if (*t_count == 4)
	{
		if (t[1] > t[3])
		{
			tmp = t[3];
			t[3] = t[1];
			t[1] = tmp;
		}

		if (t[2] > t[3])
		{
			tmp = t[3];
			t[3] = t[2];
			t[2] = tmp;
		}

		if (t[1] > t[2])
		{
			tmp = t[2];
			t[2] = t[1];
			t[1] = tmp;
		}
	}
}

static inline bool inflexions(
	int16_t a,
	int16_t b,
	int16_t c,
	int16_t d,
	int32_t k,
	int32_t* t,
	uint8_t* t_count)
{
	int16_t root_den = a - 3*b + 3*c - d;

	if (root_den == 0)
	{
		int16_t root2_den = 2*(a - 2*b +c);

		// plot line
		if (root2_den == 0)
		{
			// TODO
			return false;
		}
		// one root
		else
		{
			add_time(t, t_count, k, k*(a - b) / root2_den);
		}
	}
	else
	{
		int32_t root_nu1 = k * (a - 2*b + c);
		int32_t root_nu2 = isqrt(k*k*(a*(d-c) + b*b - b*(c+d) + c*c));

		// one root
		if (root_nu2 == 0)
		{
			add_time(t, t_count, k, root_nu1 / root_den);
		}
		// two roots
		else
		{
			add_time(t, t_count, k, (root_nu1 - root_nu2) / root_den);
			add_time(t, t_count, k, (root_nu1 + root_nu2) / root_den);
		}
	}

	return true;
}

static void ras_bezier_cubic_bijective(
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
}

void ras_bezier_cubic(
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
	int32_t k = 181;

	int32_t t[4] = {0};
	uint8_t t_count = 1;

	// compute inflexion times
	if (!(inflexions(ax, bx, cx, dx, k, t, &t_count)
	&& inflexions(ay, by, cy, dy, k, t, &t_count)))
	{
		return;
	}

	int16_t mx;
	int16_t my;
	int16_t sx;
	int16_t sy;

	int16_t b2x;
	int16_t b2y;
	int16_t c2x;
	int16_t c2y;

	int16_t b1x;
	int16_t b1y;
	int16_t c1x;
	int16_t c1y;

	int32_t t_new;
	int32_t k_new;
	
	for (uint8_t i = 1; i < t_count; ++i)
	{
		// compensate splitting
		t_new = t[i] - t[i-1];
		k_new = k - t[i-1];

		// debug
		printf("%d %d\n",
			bernstein_cubic(ax, bx, cx, dx, t_new, k_new),
			bernstein_cubic(ay, by, cy, dy, t_new, k_new));

		// split the curve
		b1x = t_new * (bx - ax) / k_new + ax;
		b1y = t_new * (by - ay) / k_new + ay;
		c1x = t_new * (dx - cx) / k_new + cx;
		c1y = t_new * (dy - cy) / k_new + cy;
		sx = t_new * (cx - bx) / k_new + bx;
		sy = t_new * (cy - by) / k_new + by;

		c2x = t_new * (sx - b1x) / k_new + b1x;
		c2y = t_new * (sy - b1y) / k_new + b1y;
		b2x = t_new * (c1x - sx) / k_new + sx;
		b2y = t_new * (c1y - sy) / k_new + sy;

		mx = t_new * (b2x - c2x) / k_new + c2x;
		my = t_new * (b2y - c2y) / k_new + c2y;

		// plot bijective bezier section
		ras_bezier_cubic_bijective(ras, ax, ay, b1x, b1y, c2x, c2y, mx, my);

		// update anchors and handles
		ax = mx;
		ay = my;
		bx = b2x;
		by = b2y;
		cx = c1x;
		cy = c1y;
	}

	// plot the remaining section
	ras_bezier_cubic_bijective(ras, ax, ay, bx, by, cx, cy, dx, dy);
}
