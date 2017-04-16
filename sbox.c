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

/* Based on Tom St Denis's Sbox Generator (sboxgen.c from 27 October 2001) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define IABS(X) ((unsigned int)(((int)(X) < 0) ? -(X) : (X)))

unsigned int parity(unsigned int x)
{
	unsigned int count = 0;

	while (x)
	{
		count ^= x & 1;
		x >>= 1;
	}

	return count;
}

unsigned int hamming(unsigned int x)
{
	unsigned int count = 0;

	while (x)
	{
		count += x & 1;
		x >>= 1;
	}

	return count;
}

unsigned int fixed_points(const unsigned int *f, unsigned int n, unsigned int limit)
{
	unsigned int count = 0;
	unsigned int i;

	for (i = 0; i < n; i++)
	{
		if (i == f[i]) count++;
		if (count > limit) return count;
	}

	return count;
}

unsigned int involutions(const unsigned int *f, unsigned int n)
{
	unsigned int count = 0;
	unsigned int i;

	for (i = 0; i < n; i++)
	{
		if (i == f[f[i]]) count++;
	}

	return count;
}

unsigned int branch(const unsigned int *f, unsigned int n)
{
	unsigned int bn;
	unsigned int min = UINT_MAX;
	unsigned int a;
	unsigned int b;

	for (a = 0; a < n; a++)
	{
		unsigned int fa = f[a];

		for (b = 0; b < n; b++)
		{
			if (b == a) continue;
			bn = hamming(a ^ b) + hamming(fa ^ f[b]);
			if (bn < min) min = bn;
		}
	}

	return min;
}

/*
 * Perform the Walsh Transform of the table using specific input/output
 * masks. 'alpha' is the input (to the function) mask and 'beta' is the
 * output (of the function) mask.
 */
int walsh_transform
(
	const unsigned int *f,
	unsigned int size,
	unsigned int alpha,
	unsigned int beta
)
{
	unsigned int i;
	int sum = 0;

	for (i = 0; i < size; i++)
	{
		if (parity(i & alpha))
		{
			sum -= parity(f[i] & beta);
		}
		else
		{
			sum += parity(f[i] & beta);
		}
	}

	return sum;
}

void build_wt(const unsigned int *f, int *wt, unsigned int n)
{
	unsigned int x;
	unsigned int y;

	for (x = 1; x < n; x++)
	{
		for (y = 0; y < n; y++)
		{
			wt[x * n + y] = walsh_transform(f, n, x, y);
		}
	}
}

unsigned int dc(const unsigned int *f, unsigned int *dt, unsigned int n, unsigned int limit)
{
	unsigned int x;
	unsigned int y;
	unsigned int z;
	unsigned int max = 0;

	memset(dt, 0, n * n * sizeof(unsigned int));

	for (x = 0; x < n; x++)
	{
		unsigned int fx = f[x];

		for (y = 0; y < n; y++)
		{
			z = (x ^ y) * n + (fx ^ f[y]);
			dt[z]++;

			if (z && dt[z] > max)
			{
				max = dt[z];
				if (max > limit) return max;
			}
		}
	}

	return max;
}

unsigned int lc(const unsigned int *f, unsigned int n, unsigned int limit)
{
	unsigned int x;
	unsigned int y;
	unsigned int t;
	unsigned int max = 0;

	for (x = 1; x < n; x++)
	{
		for (y = 0; y < n; y++)
		{
			t = IABS(walsh_transform(f, n, x, y));

			if (t > max) max = t;

			if (max > limit) return max;
		}
	}

	return max;
}

unsigned int dc_count
(
	const unsigned int *dt,
	unsigned int n,
	unsigned int trait
)
{
	unsigned int x;
	unsigned int y;
	unsigned int count = 0;

	for (x = 0; x < n; x++)
	{
		for (y = 0; y < n; y++)
		{
			if (dt[x * n + y] == trait) count++;
		}
	}

	return count;
}

unsigned int lc_count
(
	const int *wt,
	unsigned int n,
	unsigned int trait
)
{
	unsigned int x;
	unsigned int y;
	unsigned int count = 0;

	for (x = 0; x < n; x++)
	{
		for (y = 0; y < n; y++)
		{
			if (IABS(wt[x * n + y]) == trait) count++;
		}
	}

	return count;
}

unsigned int dc_single_bit(const unsigned int *f, unsigned int n)
{
	unsigned int x;
	unsigned int y;
	unsigned int count = 0;

	for (x = 0; x < n; x++)
	{
		unsigned int fx = f[x];

		for (y = x + 1/*0*/; y < n; y++)
		{
			if (hamming(x ^ y) == 1 && hamming(fx ^ f[y]) == 1)
			{
				count++;
			}
		}
	}

	return count;
}

unsigned int lc_single_bit
(
	const int *wt,
	unsigned int n
)
{
	unsigned int x;
	unsigned int y;
	unsigned int count = 0;

	for (x = 1; x < n; x++)
	{
		if (hamming(x) == 1)
		{
			for (y = 1; y < n; y++)
			{
				if (hamming(y) == 1)
				{
					if (IABS(wt[x * n + y]) > 2) count++;
				}
			}
		}
	}

	return count;
}

/*unsigned int dc_all(const unsigned int *dt, unsigned int n)
{
	unsigned int x;
	unsigned int y;
	unsigned int count = 0;

	for (x = 0; x < n; x++)
	{
		for (y = 0; y < n; y++)
		{
			count += dt[x * n + y];
		}
	}

	return count;
}

unsigned int lc_all(const int *wt, unsigned int n)
{
	unsigned int x;
	unsigned int y;
	unsigned int count = 0;

	for (x = 0; x < n; x++)
	{
		for (y = 0; y < n; y++)
		{
			count += IABS(wt[x * n + y]);
		}
	}

	return count;
}*/

unsigned int sac(const unsigned int *f, unsigned int n, unsigned int ln)
{
	unsigned int x;
	unsigned int i;
	unsigned int j;
	int s;
	unsigned int max = 0;

	/* over all input bits */
	for (i = 0; i < ln; i++)
	{
		/* over all output bits */
		for (j = 0; j < ln; j++)
		{
			/* sum the differences */
			for (s = x = 0; x < n; x++)
			{
				if ((f[x] ^ f[x ^ (1 << i)]) & (1 << j)) s++;
				else s--;
			}

			s = IABS(s);
			if (s > max) max = s;
		}
	}

	return max;
}

unsigned int nonlinear_boolean(const unsigned int *f, unsigned int n)
{
	unsigned int i;
	unsigned int l;
	unsigned int max = 0;

	for (i = 1; i < n; i++)
	{
		l = IABS(walsh_transform(f, n, i, 1));
		if (l > max) max = l;
	}

	return max;
}

unsigned int bic
(
	const unsigned int *f,
	unsigned int n,
	unsigned int ln,
	unsigned int order
)
{
	unsigned int t[n];
	unsigned int x;
	unsigned int y;
	unsigned int test;
	unsigned int min = -1;

	for (test = 3; test < (1U << ln); test++)
	{
		unsigned int hw = hamming(test);

		if (hw > 1 && hw <= order)
		{
			memset(t, 0, sizeof(t));

			for (x = 0; x < ln; x++)
			{
				if (test & (1 << x))
				{
					for (y = 0; y < n; y++)
					{
						t[y] ^= (f[y] >> x) & 1;
					}
				}
			}

			x = nonlinear_boolean(t, n);
			if (x < min) min = x;
		}
	}

	return min;
}

void print_tables
(
	const int *wt,
	const unsigned int *dt,
	unsigned int n
)
{
	unsigned int x;
	unsigned int y;

	puts("Walsh-Transform:");
	for(x = 0; x < n; x++)
	{
		for(y = 0; y < n; y++) printf("%3d", wt[x * n + y]);
		putchar('\n');
	}

	puts("\nPairs XOR distribution:");
	for(x = 0; x < n; x++)
	{
		for(y = 0; y < n; y++) printf("%3u", dt[x * n + y]);
		putchar('\n');
	}
}

struct sbox
{
	unsigned int ln;
	unsigned int n;
	const unsigned int *f;
	unsigned int *dt;
	int *wt;
	unsigned int fixed_points;
	unsigned int involutions;
	unsigned int branch_number;
	unsigned int sac;
	unsigned int bic;
	unsigned int dc_max;
	unsigned int lc_max;
	unsigned int dc_count;
	unsigned int lc_count;
	unsigned int dc_single_bit;
	unsigned int lc_single_bit;
};

void sbox_init(struct sbox *s, unsigned int ln, unsigned int n)
{
	s->ln = ln;
	s->n = n;

	s->dt = (unsigned int *)malloc(n * n * sizeof(unsigned int));
	s->wt = (int *)malloc(n * n * sizeof(int));
}

void sbox_free(const struct sbox *s)
{
	free(s->dt);
	free(s->wt);
}

int sbox(struct sbox *s, const unsigned int *f, const struct sbox *min)
{
	s->f = f;

	s->lc_max = lc(f, s->n, min->lc_max);
	if (s->lc_max > min->lc_max) return 0;

	s->dc_max = dc(f, s->dt, s->n, min->dc_max);
	if (s->dc_max > min->dc_max) return 0;

	s->sac = sac(f, s->n, s->ln);
	if (s->sac > min->sac) return 0;

	s->bic = bic(f, s->n, s->ln, 2);
	if (s->bic > min->bic) return 0;

	s->dc_count = dc_count(s->dt, s->n, s->dc_max);

	build_wt(f, s->wt, s->n);
	s->lc_count = lc_count(s->wt, s->n, s->lc_max);

	s->dc_single_bit = dc_single_bit(f, s->n);
	s->lc_single_bit = lc_single_bit(s->wt, s->n);

	s->branch_number = branch(f, s->n);
	if (s->branch_number < min->branch_number) return 0;

	s->fixed_points = fixed_points(f, s->n, min->fixed_points);
	if (s->fixed_points > min->fixed_points) return 0;

	s->involutions = involutions(f, s->n);

	return -1;
}

void sbox_print(const struct sbox *s)
{
	unsigned int i;

	for (i = 0; i < s->n; i++)
	{
		printf("0x%.2x, ", s->f[i]);
	}
	putchar('\n');

	printf("Fixed points: %u/%u\n", s->fixed_points, s->n);
	printf("Involutions: %u/%u\n", s->involutions, s->n);
	printf("Branch number: %u\n", s->branch_number);
	printf("SAC: %u, %f\n", s->sac, (float)s->sac / s->n);
	printf("BIC: %u, %f\n", s->bic, (float)s->sac / (s->n >> 1));
	printf("DC max: %u/%u (%u)\n", s->dc_max, s->n, s->dc_count);
	printf("LC max: %u/%u (%u)\n", s->lc_max, s->n >> 1, s->lc_count);
	printf("DC single bit differece: %u\n", s->dc_single_bit);
	printf("LC single bit differece: %u\n", s->lc_single_bit);

	/*putchar('\n');
	print_tables(s->wt, s->dt, s->n);*/
}
