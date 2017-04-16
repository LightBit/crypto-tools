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

#ifndef PRNG_H
#define PRNG_H

#include <stdio.h>
#include <stdint.h>

#include "rotate.h"
#include "loadstore.h"

#define G(A, B, C, D)				\
{									\
	A += B; D = ROL64_07(D ^ A);	\
	C += D; B = ROL64_02(B ^ C);	\
	A += B; D = ROL64_24(D ^ A);	\
	C += D; B = ROL64_32(B ^ C);	\
}

static uint64_t rx[4];
static uint8_t buf[32];
static unsigned int pos = 32;

void prng_get(void *x, unsigned int bytes)
{
	unsigned int i;

	for (i = 0; i < bytes; i++)
	{
		if (pos == 32)
		{
			register uint64_t A = rx[0];
			register uint64_t B = rx[1];
			register uint64_t C = rx[2];
			register uint64_t D = rx[3];

			A += 0x4ef12e145d0e3ccd;

			G(A, B, C, D);
			G(A, B, C, D);
			G(A, B, C, D);
			G(A, B, C, D);
			G(A, B, C, D);
			G(A, B, C, D);
			G(A, B, C, D);
			G(A, B, C, D);

			STORE64L(A, buf);
			STORE64L(B, buf + 8);
			STORE64L(C, buf + 16);
			STORE64L(D, buf + 24);

			rx[0] = A;
			rx[1] = B;
			rx[2] = C;
			rx[3] = D;

			pos = 0;
		}

		U8(x)[i] = buf[pos++];
	}
}

void prng_init(void)
{
	FILE *fp = fopen("/dev/urandom", "rb");
	fread(rx, sizeof(uint64_t), 4, fp);
	fclose(fp);
}

#endif
