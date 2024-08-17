#ifndef __INTX__
#define __INTX__

#define INT_SIZE 4

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t* intx_t;

// Converts int64_t to intx_t
intx_t intx_from_64(int64_t n);

// Prints intx_t as hex
void intx_print_h(intx_t n);

// Adds two intx_t
intx_t intx_add(intx_t a, intx_t b);

// Flips all of the bits of an intx_t integer
intx_t intx_not(intx_t n);

// Substract intx_t integer b from intx_t intger a
intx_t intx_sub(intx_t a, intx_t b);

// Adds a 32bits value to n (permanently affects n)
intx_t intx_push32(intx_t n, int32_t v);

// Shifts intx_t integer to the left by i bits
intx_t intx_lshift(intx_t n, int i);

// Shifts intx_t integer to the right by i bits
intx_t intx_rshift(intx_t n, int i);

// Detects if a intx_t is negative
bool intx_is_negative(intx_t n);

// Takes the 2's complement of n
intx_t intx_two_complement(intx_t n);

// Multiply two intx_t together
intx_t intx_mul(intx_t a, intx_t b);

// Divide intx_t int n(umerator) by intx_t int d(enominator)
intx_t intx_div(intx_t n, intx_t d);

intx_t intx_copy(intx_t n);

// Set a 0 (false) or 1 (true) at pos i (i = 0 => LSB, i = INT_SIZE*32-1 => MSB) (permanently affects n)
intx_t intx_set_digit(intx_t n, uint32_t i, bool digit);

// Get digit at position i (i = 0 => LSB)
bool intx_get_digit(intx_t n, uint32_t i);

// Returns the remainder of the division between n(umerator) and d(enominator)
intx_t intx_mod(intx_t n, intx_t d);


#endif
