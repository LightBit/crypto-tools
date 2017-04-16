/*
 * Written in 2016 by Gregor Pintar <grpintar@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated
 * all copyright and related and neighboring rights to this software
 * to the public domain worldwide.
 * 
 * This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication.
 * If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef GF_H
#define GF_H

#include <stdint.h>

/* 8 */
uint8_t gf8_add(uint8_t a, uint8_t b)
{
	return a ^ b;
}

uint8_t gf8_mul(uint8_t a, uint8_t b, uint8_t r)
{
	uint8_t p = 0;

	while (b)
	{
		if (b & 1) p ^= a;

		a = (a << 1) ^ (a & 0x80U ? r : 0);

		b >>= 1;
	}

	return p;
}

uint8_t gf8_inverse(uint8_t x, uint8_t r)
{
	unsigned int i;

	for (i = 0; i < 256; i++)
	{
		if (gf8_mul(x, i, r) == 1) return i;
	}

	return 0;
}

uint8_t gf8_affine(uint8_t a, uint8_t u, uint8_t v)
{
	return gf8_add(gf8_mul(a, u, 1), v);
}

/* 16 */
uint16_t gf16_add(uint16_t a, uint16_t b)
{
	return a ^ b;
}

uint16_t gf16_mul(uint16_t a, uint16_t b, uint16_t r)
{
	uint16_t p = 0;

	while (b)
	{
		if (b & 1) p ^= a;

		a = (a << 1) ^ (a & 0x8000U ? r : 0);

		b >>= 1;
	}

	return p;
}

uint16_t gf16_inverse(uint16_t x, uint16_t r)
{
	unsigned int i;

	for (i = 0; i < 256; i++)
	{
		if (gf16_mul(x, i, r) == 1) return i;
	}

	return 0;
}

uint16_t gf16_affine(uint16_t a, uint16_t u, uint16_t v)
{
	return gf16_add(gf16_mul(a, u, 1), v);
}

/* 32 */
uint32_t gf32_add(uint32_t a, uint32_t b)
{
	return a ^ b;
}

uint32_t gf32_mul(uint32_t a, uint32_t b, uint32_t r)
{
	uint32_t p = 0;

	while (b)
	{
		if (b & 1) p ^= a;

		a = (a << 1) ^ (a & 0x80000000U ? r : 0);

		b >>= 1;
	}

	return p;
}

uint32_t gf32_inverse(uint32_t x, uint32_t r)
{
	unsigned int i;

	for (i = 0; i < 256; i++)
	{
		if (gf32_mul(x, i, r) == 1) return i;
	}

	return 0;
}

uint32_t gf32_affine(uint32_t a, uint32_t u, uint32_t v)
{
	return gf32_add(gf32_mul(a, u, 1), v);
}

/* 64 */
uint64_t gf64_add(uint64_t a, uint64_t b)
{
	return a ^ b;
}

uint64_t gf64_mul(uint64_t a, uint64_t b, uint64_t r)
{
	uint64_t p = 0;

	while (b)
	{
		if (b & 1) p ^= a;

		a = (a << 1) ^ (a & 0x8000000000000000UL ? r : 0);

		b >>= 1;
	}

	return p;
}

uint64_t gf64_inverse(uint64_t x, uint64_t r)
{
	unsigned int i;

	for (i = 0; i < 256; i++)
	{
		if (gf64_mul(x, i, r) == 1) return i;
	}

	return 0;
}

uint64_t gf64_affine(uint64_t a, uint64_t u, uint64_t v)
{
	return gf64_add(gf64_mul(a, u, 1), v);
}

#endif
