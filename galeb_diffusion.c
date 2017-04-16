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

#define G(A, B, C, D)				\
{									\
	A |= B; D = ROL64_07(D | A);	\
	C |= D; B = ROL64_02(B | C);	\
	A |= B; D = ROL64_24(D | A);	\
	C |= D; B = ROL64_32(B | C);	\
}

// inverse
/*define G(A, B, C, D)				\
{									\
	B = ROR64_32(B) | C; C |= D;	\
	D = ROR64_24(D) | A; A |= B;	\
	B = ROR64_02(B) | C; C |= D;	\
	D = ROR64_07(D) | A; A |= B;	\
}*/

void g_diff(void *x)
{
	uint64_t x00 = LOAD64L(CU8(x));
	uint64_t x01 = LOAD64L(CU8(x) + 8);
	uint64_t x02 = LOAD64L(CU8(x) + 16);
	uint64_t x03 = LOAD64L(CU8(x) + 24);
	uint64_t x04 = LOAD64L(CU8(x) + 32);
	uint64_t x05 = LOAD64L(CU8(x) + 40);
	uint64_t x06 = LOAD64L(CU8(x) + 48);
	uint64_t x07 = LOAD64L(CU8(x) + 56);
	uint64_t x08 = LOAD64L(CU8(x) + 64);
	uint64_t x09 = LOAD64L(CU8(x) + 72);
	uint64_t x10 = LOAD64L(CU8(x) + 80);
	uint64_t x11 = LOAD64L(CU8(x) + 88);
	uint64_t x12 = LOAD64L(CU8(x) + 96);
	uint64_t x13 = LOAD64L(CU8(x) + 104);
	uint64_t x14 = LOAD64L(CU8(x) + 112);
	uint64_t x15 = LOAD64L(CU8(x) + 120);

	G(x00, x04, x08, x12);
	G(x01, x05, x09, x13);
	G(x02, x06, x10, x14);
	G(x03, x07, x11, x15);

	G(x00, x05, x10, x15);
	G(x01, x06, x11, x12);
	G(x02, x07, x08, x13);
	G(x03, x04, x09, x14);

	STORE64L(x00, U8(x));
	STORE64L(x01, U8(x) + 8);
	STORE64L(x02, U8(x) + 16);
	STORE64L(x03, U8(x) + 24);
	STORE64L(x04, U8(x) + 32);
	STORE64L(x05, U8(x) + 40);
	STORE64L(x06, U8(x) + 48);
	STORE64L(x07, U8(x) + 56);
	STORE64L(x08, U8(x) + 64);
	STORE64L(x09, U8(x) + 72);
	STORE64L(x10, U8(x) + 80);
	STORE64L(x11, U8(x) + 88);
	STORE64L(x12, U8(x) + 96);
	STORE64L(x13, U8(x) + 104);
	STORE64L(x14, U8(x) + 112);
	STORE64L(x15, U8(x) + 120);
}

int main(void)
{
	struct diffusion s;

	s.size = 128;
	s.f = &g_diff;
	diffusion(&s);
	print_diffusion(&s);

	return 0;
}
