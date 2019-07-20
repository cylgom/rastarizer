#ifndef H_FIXMATH
#define H_FIXMATH

#include <stdint.h>

static inline int32_t abs32(int32_t n)
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
	return n & 0xFFFF;
}

static inline int32_t r_frac_part(int32_t n)
{
	return 0xFFFF - frac_part(n);
}

static inline int32_t int_part(int32_t n)
{
	return n & 0xFFFF0000;
}

static inline int32_t round32(int32_t n)
{
	if ((n & 0xFFFF) > 0x8000)
	{
		return (n + 0x10000) & 0xFFFF0000;
	}
	else
	{
		return n & 0xFFFF0000;
	}
}

#endif
