#ifndef __INTX_ARITHMETIC__
#define __INTX_ARITHMETIC__

#include "intx_t.h"
#include <stdbool.h>

typedef struct eucl_ret {
	intx_t a;
	intx_t b;
	intx_t gcd;
} eucl_ret;


// extended euclidean algorithm to get greatest common divisor and the coefficient of the Bezout's decomposition
eucl_ret extended_euclidean(intx_t a, intx_t b);

// Tests to see if the given intx_t integer is probable prime number
bool fermat_test(intx_t p, int k);

// Tests to see if the given intx_t integer is a prime number
bool primality_test_naive(intx_t n);

intx_t fermat_mod(intx_t a, intx_t p);

// Generates a intx_t integer prime number
intx_t generate_prime(bool (* primality_test) (intx_t p, int k), int k);

#endif

