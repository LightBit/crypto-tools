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

#include "bit.h"

#ifndef MAX_ROUNDS
#define MAX_ROUNDS 64
#endif

struct diffusion
{
	void (*f)(void *);
	unsigned int size;
	unsigned int avg[MAX_ROUNDS];
	unsigned int worst[MAX_ROUNDS];
	unsigned int rounds;
	unsigned int avg_sum;
	unsigned int worst_sum;
};

static unsigned int diffuse(struct diffusion *s, const void *x, unsigned int *bits)
{
	unsigned int i;
	unsigned int r;
	uint8_t t[s->size];
	
	memcpy(t, x, s->size);

	for (i = 0; i < MAX_ROUNDS; i++)
	{
		s->f(t);

		bits[i] = hamming(t, s->size);

		if (bits[i] == 8 * s->size)
		{
			r = i + 1;
			while (i < MAX_ROUNDS)
			{
				bits[i++] = 8 * s->size;
			}
			return r;
		}
	}

	return MAX_ROUNDS;
}

static void bits_stat
(
	const unsigned int *bits,
	unsigned int *worst,
	unsigned int *avg
)
{
	unsigned int i;

	for (i = 0; i < MAX_ROUNDS; i++)
	{
		if (bits[i] < worst[i])
		{
			worst[i] = bits[i];
		}

		avg[i] += bits[i];
	}
}

void diffusion(struct diffusion *s)
{
	uint8_t t[s->size];
	unsigned int max_r = 0;
	unsigned int r;
	unsigned int i;

	unsigned int diff[s->size][MAX_ROUNDS];
	unsigned int avg[s->size][MAX_ROUNDS];
	unsigned int worst[s->size][MAX_ROUNDS];

	for (i = 0; i < s->size; i++)
	{
		t[i] = 0;
		for (r = 0; r < MAX_ROUNDS; r++)
		{
			worst[i][r] = 8 * s->size;
			avg[i][r] = 0;
		}
	}

	for (i = 0; i < s->size; i++)
	{
		for (t[i] = 1; t[i]; t[i] <<= 1)
		{
			r = diffuse(s, t, diff[i]);
			if (r > max_r)
			{
				max_r = r;
			}

			bits_stat(diff[i], worst[i], avg[i]);
		}
	}

	for (r = 0; r < MAX_ROUNDS; r++)
	{
		s->worst[r] = 8 * s->size;
		s->avg[r] = 0;
	}

	for (i = 0; i < s->size; i++)
	{
		for (r = 0; r < MAX_ROUNDS; r++)
		{
			if (worst[i][r] < s->worst[r])
			{
				s->worst[r] = worst[i][r];
			}

			s->avg[r] += avg[i][r];
		}
	}

	for (r = 0; r < MAX_ROUNDS; r++)
	{
		s->avg[r] /= 8 * s->size;
	}

	s->rounds = max_r;
	
	s->worst_sum = 0;
	s->avg_sum = 0;

	for (i = 0; i < s->rounds; i++)
	{
		s->worst_sum += s->worst[i];
		s->avg_sum += s->avg[i];
	}
}

int is_better(struct diffusion *best, const struct diffusion *new)
{
	int i;

	if (new->rounds == best->rounds)
	{
		for (i = best->rounds - 2; i >= 0; i--)
		{
			if (new->worst[i] < best->worst[i]) return 0;
			if (new->avg[i] < best->avg[i]) return 0;

			if (new->worst_sum < best->worst_sum) return 0;
			if (new->avg_sum < best->avg_sum) return 0;

			if (new->worst[i] > best->worst[i])
			{
				best->worst[i] = new->worst[i];
				return -1;
			}

			if (new->avg[i] > best->avg[i])
			{
				best->avg[i] = new->avg[i];
				return -1;
			}
		}
	}
	else if (new->rounds < best->rounds)
	{
		best->rounds = new->rounds;

		for (i = 0; i < best->rounds - 1; i++)
		{
			best->avg[i] = best->worst[i] = 0;
		}

		best->avg_sum = best->worst_sum = 0;

		return -1;
	}

	return 0;
}

void print_diffusion(struct diffusion *s)
{
	unsigned int i;

	printf("Rounds: %u\n", s->rounds);

	printf("worst diffusion:   ");
	for (i = 0; i < s->rounds - 1; i++)
	{
		printf("%u, ", s->worst[i]);
	}
	printf("(%u)\n", s->worst_sum);

	printf("average diffusion: ");
	for (i = 0; i < s->rounds - 1; i++)
	{
		printf("%u, ", s->avg[i]);
	}
	printf("(%u)\n", s->avg_sum);
}
