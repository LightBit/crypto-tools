/*
 * Written in 2015 by Gregor Pintar <grpintar@gmail.com>
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

#ifndef BIT_H
#define BIT_H

#include <stdint.h>

#include "cast.h"

unsigned int parity(const void *x, size_t len)
{
	unsigned int c = 0;
	uint8_t mask;

	while (len--)
	{
		for (mask = 1; mask; mask <<= 1)
		{
			if (CU8(x)[len] & mask) c ^= 1;
		}
	}

	return c;
}

unsigned int hamming(const void *x, size_t len)
{
	unsigned int c = 0;
	uint8_t mask;

	while (len--)
	{
		for (mask = 1; mask; mask <<= 1)
		{
			if (CU8(x)[len] & mask) c++;
		}
	}

	return c;
}

#endif
