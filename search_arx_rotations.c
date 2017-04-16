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
#include <string.h>

#include "arx_diffusion.c"
#include "cast.h"
#include "loadstore.h"
#include "rotate.h"

int next(unsigned int *x, unsigned int len, unsigned int max)
{
	int i;

	for (i = len - 1; i >= 0; i--)
	{
		if ((x[i] = (x[i] + 1) % max)) return -1;
	}

	return 0;
}

#define ROTATIONS 4

unsigned int rot[ROTATIONS];

void g_diff(void *x)
{
	uint64_t A = LOAD64L(CU8(x));
	uint64_t B = LOAD64L(CU8(x) + 8);
	uint64_t C = LOAD64L(CU8(x) + 16);
	uint64_t D = LOAD64L(CU8(x) + 24);

	A |= B; D = ROL64(D | A, rot[0]);
	C |= D; B = ROL64(B | C, rot[1]);
	A |= B; D = ROL64(D | A, rot[2]);
	C |= D; B = ROL64(B | C, rot[3]);

	STORE64L(A, U8(x));
	STORE64L(B, U8(x) + 8);
	STORE64L(C, U8(x) + 16);
	STORE64L(D, U8(x) + 24);
}

int main(void)
{
	unsigned int i;
	struct diffusion best;
	struct diffusion s;
	unsigned int sum;

	s.size = 16;
	s.f = &g_diff;
	
	for (i = 0; i < ROTATIONS; i++) rot[i] = 1;
	
	best.rounds = MAX_ROUNDS;
	for (i = 0; i < best.rounds - 1; i++)
	{
		best.avg[i] = best.worst[i] = 0;
	}
	
	while (next(rot, ROTATIONS, 64))
	{
		sum = 0;
		for (i = 0; i < ROTATIONS; i++) sum += rot[i];
		
		if (!(sum & 1)) continue;

		diffusion(&s);

		if (is_better(&best, &s))
		{
			printf("\n{");
			for (i = 0; i < ROTATIONS; i++)
			{
				printf("%2u, ", rot[i]);
			}
			printf("\b\b}\n");

			print_diffusion(&s);
		}
	}

	return 0;
}
