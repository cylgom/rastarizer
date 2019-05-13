#include <stdint.h> // uint8_t, uint16_t, uint32_t
#include <stdio.h> // FILE, EOF, NULL, fopen, fclose, fwrite
#include "rastarizer.h"

static inline uint32_t swap_u32(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0x00FF00FF);
    return (val << 16) | (val >> 16);
}

uint8_t farbfeld(
	const char* file,
	uint32_t width,
	uint32_t height,
	uint8_t* rgba)
{
	FILE* fp = fopen(file, "w");

	if (fp == NULL)
	{
		return 0;
	}

	uint32_t size = ((uint32_t) 4) * width * height;
	int8_t ok = fwrite("farbfeld", 8, 1, fp);

	if (ok == 0)
	{
		return 0;
	}

	// remove for big-endian
	width = swap_u32(width);
	ok = fwrite(&width, 4, 1, fp);

	if (ok == 0)
	{
		return 0;
	}

	// remove for big-endian
	height = swap_u32(height);
	ok = fwrite(&height, 4, 1, fp);

	if (ok == 0)
	{
		return 0;
	}

	uint16_t value;

	for (uint32_t i = 0; i < size; ++i)
	{
		// leverages endianness mismatch to avoid byte swapping
		// replace by the following line for big-endian
		// value = ((uint16_t) rgba[i]) << 8;
		value = rgba[i];
		ok = fwrite(&value, 2, 1, fp);

		if (ok == 0)
		{
			return 0;
		}
	}

	ok = fclose(fp);

	return ok != EOF;
}

int main(void)
{
	uint8_t rgba[400*400*4];
	struct ras_buf ras = {rgba, 400, 400};

	for (uint32_t i = 0; i < 400*400*4; ++i)
	{
		rgba[i] = 0xFF;
	}

	ras_disk(ras, 80, 80, 40);
	ras_disk(ras, 10, 20, 10);
	ras_disk(ras, 10, 40, 9);
	ras_disk(ras, 10, 60, 8);
	ras_disk(ras, 10, 80, 7);
	ras_disk(ras, 10, 100, 6);
	ras_disk(ras, 10, 120, 5);
	ras_disk(ras, 10, 140, 4);
	ras_disk(ras, 10, 160, 3);
	ras_disk(ras, 10, 180, 2);
	ras_disk(ras, 10, 200, 1);

	ras_rounded_rectangle(ras, 200, 300, 200, 350, 17);

	uint8_t ok = farbfeld("test.ff", 400, 400, rgba);

	return (ok == 1) ? 0 : 1;
}
