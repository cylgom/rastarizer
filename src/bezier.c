#include "rastarizer.h"
#include "math32.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

static inline uint16_t bernstein_cubic(
	uint16_t a,
	uint16_t b,
	uint16_t c,
	uint16_t d,
	uint32_t t,
	uint32_t base)
{
	uint16_t t_1 = base - t;

	return (a * (t_1 * t_1 * t_1)
		+ b * 3 * (t_1 * t_1) * t
		+ c * 3 * t_1 * (t * t)
		+ d * (t * t * t)) / (base * base * base);
}

static inline void add_time(
	uint32_t* t,
	uint8_t* t_count,
	uint32_t k,
	uint32_t v)
{
	if (v <= k)
	{
		t[*t_count] = v;
		++(*t_count);
	}
}

static inline bool inflexions(
	uint16_t a,
	uint16_t b,
	uint16_t c,
	uint16_t d,
	uint32_t k,
	uint32_t* t,
	uint8_t* t_count)
{
	uint16_t root_den = a - 3*b + 3*c - d;

	if (root_den == 0)
	{
		uint16_t root2_den = 2*(a - 2*b +c);

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
		uint32_t root_nu1 = k * (a - 2*b + c);
		uint32_t root_nu2 = isqrt(k*k*(a*(d-c) + b*b - b*(c+d) + c*c));

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
	uint16_t ax,
	uint16_t ay,
	uint16_t bx,
	uint16_t by,
	uint16_t cx,
	uint16_t cy,
	uint16_t dx,
	uint16_t dy)
{
}

void ras_bezier_cubic(
	struct ras_buf ras,
	uint16_t ax,
	uint16_t ay,
	uint16_t bx,
	uint16_t by,
	uint16_t cx,
	uint16_t cy,
	uint16_t dx,
	uint16_t dy)
{
	uint32_t k = 256;

	uint32_t t[4];
	uint8_t t_count = 1;

	t[0] = 0;

	// compute inflexion times
	if (!(inflexions(ax, bx, cx, dx, k, t, &t_count)
	&& inflexions(ay, by, cy, dy, k, t, &t_count)))
	{
		return;
	}

#if 0
	uint16_t mx;
	uint16_t my;
	uint16_t sx;
	uint16_t sy;

	uint16_t b2x;
	uint16_t b2y;
	uint16_t c2x;
	uint16_t c2y;

	uint16_t b1x;
	uint16_t b1y;
	uint16_t c1x;
	uint16_t c1y;

	uint32_t t_new;
	uint32_t k_new;
#endif
	
	for (uint8_t i = 1; i < t_count; ++i)
	{
		printf("%d %d\n",
			bernstein_cubic(ax, bx, cx, dx, t[i], k),
			bernstein_cubic(ay, by, cy, dy, t[i], k));

#if 0
		// compensate splitting
		t_new = t[i] - t[i-1];
		k_new = k - t[i-1];

		// split the curve
		b1x = (t_new * (bx - ax) + ax) / k_new;
		b1y = (t_new * (by - ay) + ay) / k_new;
		c1x = (t_new * (dx - cx) + cx) / k_new;
		c1y = (t_new * (dy - cy) + cy) / k_new;
		sx = (t_new * (cx - bx) + bx) / k_new;
		sy = (t_new * (cy - by) + by) / k_new;

		c2x = (t_new * (sx - b1x) + b1x) / k_new;
		c2y = (t_new * (sy - b1y) + b1y) / k_new;
		b2x = (t_new * (c1x - sx) + sx) / k_new;
		b2y = (t_new * (c1y - sy) + sy) / k_new;

		mx = (t_new * (b2x - c2x) + c2x) / k_new;
		my = (t_new * (b2y - c2y) + c2y) / k_new;

		// plot bijective bezier section
		ras_bezier_cubic_bijective(ras, ax, ay, b1x, b1y, c2x, c2y, mx, my);

		// update anchors and handles
		ax = mx;
		ay = my;
		bx = b2x;
		by = b2y;
		cx = c1x;
		cy = c1y;
#endif
	}

	// plot the remaining section
	ras_bezier_cubic_bijective(ras, ax, ay, bx, by, cx, cy, dx, dy);
}
