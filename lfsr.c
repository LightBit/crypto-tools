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

#include <stdint.h>
#include <stdio.h>

#include "bit.h"

#ifndef BITS
#define BITS 64
#endif

#if BITS == 64
#define BYTES 8
#define WORD_MAX UINT64_MAX
typedef uint64_t word;

#elif BITS == 32
#define BYTES 4
#define WORD_MAX UINT32_MAX
typedef uint32_t word;

#elif BITS == 16
#define BYTES 2
#define WORD_MAX UINT16_MAX
typedef uint16_t word;

#elif BITS == 8
#define BYTES 1
#define WORD_MAX UINT8_MAX
typedef uint8_t word;

#else
#error "not supported"
#endif

word ROL(word x, word r)
{
	r &= BITS - 1;
	return (x << r) | (x >> (BITS - r));
}

word ROR(word x, word r)
{
	r &= BITS - 1;
	return (x >> r) | (x << (BITS - r));
}

#define POLY 0x4ef12e145d0e3ccd

word lfsr_next(word x)
{
	if(x & 1 << (BITS - 1))
	{
		return (x << 1) ^ POLY;
	}
	else
	{
		return x << 1;
	}
}

int main(void)
{
	unsigned int i;
	unsigned int r;
	word rc = POLY;
	word t;
	unsigned int hw;
	unsigned long long int avg = 0;
	unsigned long long int avg_rot = 0;

	for (i = 0; i < 128; i++)
	{
		rc = lfsr_next(rc);
		//rc += POLY;

		hw = hamming(&rc, BYTES);
		avg += hw;
		//printf("%u\n", hw);

		for(r = 1; r < BITS; r++)
		{
			t = rc ^ ROL(rc, r);
			hw = hamming(&t, BYTES);
			avg_rot += hw;
			//printf("%u\n", hw);
		}
	}
	t = POLY;
	printf("hw: %u\n", hamming(&t, BYTES));
	printf("AVG: %llu\n", avg / i);
	printf("AVG rotations: %llu\n", avg_rot / (i * (r - 1)));

	/*rc = POLY;
	for (t = 0; t != WORD_MAX; t++)
	{
		rc = lfsr_next(rc);
		if(rc == POLY) printf("Repeat after %lX\n", t);
	}*/

	return 0;
}
