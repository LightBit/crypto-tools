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

#include "prng.h"

void perm_init(unsigned int *p, unsigned int len)
{
	unsigned int i;
	
	for (i = 0; i < len; i++) p[i] = i;
}

void perm_init_even(unsigned int *p, unsigned int len)
{
	unsigned int i;
	
	for (i = 0; i < len; i++)
	{
		p[i] = i << 1;
	}
}

void perm_init_odd(unsigned int *p, unsigned int len)
{
	unsigned int i;
	
	for (i = 0; i < len; i++)
	{
		p[i] = (i << 1) | 1;
	}
}

void random_permute(unsigned int *p, unsigned int len)
{
	unsigned int i;
	unsigned int r;
	unsigned int t;

	for (i = 0; i < len; i++)
	{
		prng_get(&r, sizeof(unsigned int));
		r %= len;

		t = p[i];
		p[i] = p[r];
		p[r] = t;
	}
}

int permute(unsigned int *p, unsigned int len)
{
	int i;
	int a = -1;
	int b = -1;
	unsigned int t;

	for (i = len - 2; i >= 0; i--)
	{
		if (p[i] < p[i + 1])
		{
			a = i;
			break;
		}
	}

	if (a < 0) return 0;

	for (i = len - 1; i >= 0; i--)
	{
		if (p[a] < p[i])
		{
			b = i;
			break;
		}
	}

	t = p[a];
	p[a] = p[b];
	p[b] = t;

	for (i = a + 1, b = len - 1; i < b; i++, b--)
	{
		t = p[i];
		p[i] = p[b];
		p[b] = t;
	}

	return -1;
}

int repeats(const unsigned int *p, unsigned int len, unsigned int r)
{
	unsigned int i;
	unsigned int j;
	unsigned int t;

	for (i = 0; i < len; i++)
	{
		for (j = 0, t = i; j < r; j++)
		{
			t = p[t];
		}

		if (t != i) return 0;
	}

	return -1;
}

#define MIX(A, B) { if (B) A = 1; if (A) B = 1; }

unsigned int diffusion
(
	unsigned int *x,
	const unsigned int *p,
	unsigned int len,
	unsigned int r
)
{
	unsigned int t[len];
	unsigned int i;
	unsigned int j;
	unsigned int sum = 0;

	for (j = 0; j < r; j++)
	{
		
		for (i = 0; i < len; i++) t[i] = x[p[i]];
		for (i = 0; i < len; i++) x[i] = t[i];
		for (i = 0; i < len; i += 2) MIX(x[i], x[i + 1]);
	}
	for (i = 0; i < len; i++) sum += x[i];

	return sum;
}

unsigned int diffusion_all
(
	const unsigned int *p,
	unsigned int len,
	unsigned int r
)
{
	unsigned int x[len];
	unsigned int i;
	unsigned int sum = 0;

	for (i = 0; i < len; i++)
	{
		memset(x, 0, len * sizeof(unsigned int));
		x[i] = 1;

		sum += diffusion(x, p, len, r);
	}

	return sum;
}

void print_permutation(const unsigned int *p, unsigned int words)
{
	unsigned int a;
	unsigned int b;
	unsigned int i;
	unsigned int t0;
	unsigned int t1;

	for (a = 0; a < (words >> 1) - 1; a++)
	{
		for (i = 0; i < words; i+=2)
		{
			t0 = i;
			t1 = i + 1;
			for (b = 0; b < a; b++) t0 = p[t0];
			for (b = 0; b < a; b++) t1 = p[t1];
			printf("MIX(x[%u], x[%u]);\n", t0, t1);
		}
		putchar('\n');
	}
}

#define WORDS 8
#define REPEATS 3

int main(void)
{
	unsigned int p[WORDS];
	unsigned int even[WORDS / 2];
	unsigned int odd[WORDS / 2];
	unsigned int i;
	unsigned int sum;
	unsigned int max_sum = 0;

#ifdef RANDOM

	prng_init();

	perm_init_odd(odd, WORDS / 2);
	perm_init_even(even, WORDS / 2);

	for (;;)
	{
		random_permute(odd, WORDS / 2);
		random_permute(even, WORDS / 2);
		
		for (i = 0; i < WORDS; i++)
		{
			if (i & 1) p[i] = odd[i >> 1];
			else p[i] = even[i >> 1];
		}

		if (!repeats(p, WORDS, REPEATS)) continue;

		sum = diffusion_all(p, WORDS, REPEATS);

		if (sum >= max_sum)
		{
			max_sum = sum;

			for (i = 0; i < WORDS; i++)
			{
				printf("%u, ", p[i]);
			}
			putchar('\n');

			//print_permutation(p, WORDS);

			printf("%u / %u\n\n", sum, WORDS * WORDS);
		}
	}

#else

	perm_init_odd(odd, WORDS / 2);
	do
	{
		perm_init_even(even, WORDS / 2);
		do
		{
			for (i = 0; i < WORDS; i++)
			{
				if (i & 1) p[i] = odd[i >> 1];
				else p[i] = even[i >> 1];
			}

			if (!repeats(p, WORDS, REPEATS)) continue;

			sum = diffusion_all(p, WORDS, REPEATS);

			if (sum >= max_sum)
			{
				max_sum = sum;

				for (i = 0; i < WORDS; i++)
				{
					printf("%u, ", p[i]);
				}
				putchar('\n');

				//print_permutation(p, WORDS);

				printf("%u / %u\n\n", sum, WORDS * WORDS);
			}
		}
		while (permute(even, WORDS / 2));
	}
	while (permute(odd, WORDS / 2));

#endif

	return 0;
}
