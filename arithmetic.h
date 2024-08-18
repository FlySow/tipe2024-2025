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

// Tests to see if the given intx_t intger is a prime number
bool fermat_primality(intx_t n);

// Generates a intx_t integer prime number
intx_t generate_prime_number();

#endif

