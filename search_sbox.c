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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#include "sbox.c"
#include "prng.h"
#include "rotate.h"
#include "gf.h"

void permute(unsigned int *s, unsigned int len)
{
	unsigned int i;
	unsigned int r;
	unsigned int t;

	for (i = 0; i < len; i++)
	{
		prng_get(&r, sizeof(unsigned int));
		r %= len;

		t = s[i];
		s[i] = s[r];
		s[r] = t;
	}
}

/*void generate_rijndael_sbox(unsigned int *s)
{
	// loop invariant: p * q == 1 in the Galois field
	uint8_t p = 1, q = 1;
	do
	{
		// multiply p by x+1
		p = p ^ (p << 1) ^ (p & 0x80 ? 0x1b : 0);

		// divide q by x+1
		q ^= q << 1;
		q ^= q << 2;
		q ^= q << 4;
		q ^= q & 0x80 ? 0x09 : 0;

		// compute the affine transformation
		s[p] = 0x63 ^ q ^ ROL8_1(q) ^ ROL8_2(q) ^ ROL8_3(q) ^ ROL8_4(q);
	}
	while (p != 1);

	// 0 is a special case since it has no inverse
	s[0] = 0x63;
}*/

// APA
// http://www.ijicic.org/ijicic-10-01041.pdf
// https://github.com/dufferzafar/substitute/
void generate_apa_sbox(unsigned int *s)
{
	unsigned int i;

	for (i = 0; i < 256; i++)
	{
		s[i] = i;
		s[i] = gf8_affine(s[i], 0x5b, 0x5d);
		//s[i] = gf8_affine(s[i], 0x0e, 0x25);

		s[i] = gf8_inverse(s[i], 0x1b);

		s[i] = gf8_affine(s[i], 0x5b, 0x5d);
		//s[i] = gf8_affine(s[i], 0x0e, 0x25);
	}
}

int main(void)
{
	struct sbox s;
	struct sbox min;

	min.fixed_points = 0;
	min.involutions = 0;
	min.dc_max = UINT_MAX;
	min.lc_max = UINT_MAX;
	min.sac = UINT_MAX;
	min.bic = UINT_MAX;
	min.branch_number = 2;
	min.dc_count = UINT_MAX;
	min.lc_count = UINT_MAX;
	min.dc_single_bit = UINT_MAX;
	min.lc_single_bit = UINT_MAX;

	sbox_init(&s, 8, 256);

	unsigned int p[256];

	for (unsigned int i = 0; i < 256; i++)
	{
		p[i] = i;
	}

	/*generate_apa_sbox(p);
	sbox(&s, p, &min);
	sbox_print(&s);*/

	prng_init();

	for (;;)
	{
		if (sbox(&s, p, &min))
		{
			sbox_print(&s);
			memcpy(&min, &s, sizeof(struct sbox));
		}

		permute(p, 256);
	}

	return 0;
}
