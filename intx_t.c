#include "intx_t.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>

intx_t intx_from_64(int64_t n) {
	uint32_t* wordx = malloc(sizeof(uint32_t)*INT_SIZE);
	
	wordx[0] = (uint32_t) n;
	wordx[1] = n >> 32;

	for(int i = 2; i < INT_SIZE; i++)
		wordx[i] = n < 0 ? -1 : 0;

	return wordx;
}


intx_t intx_from_u64(uint64_t n) {
	uint32_t* wordx = malloc(sizeof(uint32_t)*INT_SIZE);
	
	wordx[0] = (uint32_t) n;
	wordx[1] = n >> 32;

	for(int i = 2; i < INT_SIZE; i++)
		wordx[i] = 0;

	return wordx;
}


void intx_print_h(intx_t n) {
	for(int i = INT_SIZE-1; i >= 0; i--)
		printf("%08x", n[i]);
	printf("\n");
}


intx_t intx_add(intx_t a, intx_t b) {
	intx_t res = malloc(sizeof(uint32_t));
	
	uint32_t carry = 0;
	for(int i = 0; i < INT_SIZE; i++) {
		uint32_t x = a[i];
		uint32_t y = b[i];
		if(carry) {
			asm {
				mov eax, x
				mov ebx, y
				add eax, ebx
				mov 
			}
	}
	return res;
}


intx_t intx_push32(intx_t n, uint32_t v) {
	uint32_t carry = 0;
	if(n[0] > UINT_MAX - v) carry = 1;
	n[0]+= v;
	for(int i = 1; i < INT_SIZE; i++) {
		uint32_t temp = 0;
		if(n[i] > UINT_MAX - carry) temp = 1;
		n[i]+= carry;
		carry = temp;
	}
	return n;
}


intx_t intx_not(intx_t n) {
	intx_t res = malloc(sizeof(uint32_t)*INT_SIZE);

	for(int i = 0; i < INT_SIZE; i++) res[i] = ~n[i];

	return res;
}


intx_t intx_sub(intx_t a, intx_t b) {
	intx_t negation = intx_not(b);
	intx_t one = intx_from_64(1);
	intx_t comp_2 = intx_add(negation, one);
	intx_print_h(comp_2);
	intx_t res = intx_add(a, comp_2);
	free(one);
	free(negation);
	free(comp_2);
	return res;
}


intx_t intx_lshift(intx_t n, int p) {
	assert(p >= 0);
	
	intx_t res = malloc(sizeof(uint32_t)*INT_SIZE);
	
	int chunk_offset = p / 32;
	int shift_offset = p % 32;

	for(int i = 0; i < chunk_offset && i < INT_SIZE; i++) res[i] = 0;
	if(chunk_offset < INT_SIZE) res[chunk_offset] = n[0] << shift_offset;
	for(int i = chunk_offset+1; i < INT_SIZE; i++) 
		res[i] = (n[i-chunk_offset] << shift_offset) + (shift_offset != 0 ? n[i-chunk_offset-1] >> (32-shift_offset) : 0);
	return res;
}


intx_t intx_rshift(intx_t n, int p) {
	assert(p >= 0);

	intx_t res = malloc(sizeof(uint32_t)*INT_SIZE);

	int chunk_offset = p / 32;
	int shift_offset = p % 32;
	
	for(int i = 0; i < INT_SIZE-chunk_offset-1; i++) 
		res[i] = (n[i+chunk_offset] >> shift_offset) + (shift_offset != 0 ? n[i+chunk_offset+1] << (32-shift_offset) : 0);
	if(chunk_offset < INT_SIZE) res[INT_SIZE-chunk_offset-1] = n[INT_SIZE-1] >> shift_offset;
	for(int i = INT_SIZE-chunk_offset+1; i < INT_SIZE; i++) res[i] = 0;
	return res;
}


intx_t intx_two_complement(intx_t n) {
	return intx_push32(intx_not(n), 1);
}


bool intx_is_negative(intx_t n) {
	return (int32_t) n[INT_SIZE-1] < 0;
}


intx_t intx_mul(intx_t a, intx_t b) {
	bool tc = false;
	if(intx_is_negative(a) && intx_is_negative(b)) {
		a = intx_two_complement(a);
		b = intx_two_complement(b);
	}else if(intx_is_negative(a)) {
		tc = true;
		a = intx_two_complement(a);
	}else if(intx_is_negative(b)) {
		tc = true;
		b = intx_two_complement(b);
	}
	
	intx_t res = intx_from_64(0);
	
	for(int i = 0; i < INT_SIZE; i++) {
		for(int j = 0; j < INT_SIZE-1; j++) {
			uint64_t temp64 = (uint64_t) a[i]*(uint64_t) b[j];
			intx_t temp = intx_from_u64(temp64);
			intx_t temp_shift = intx_lshift(temp, 32*(i+j));
			intx_t temp_res = intx_add(res, temp_shift);
			free(temp);
			free(res);
			res = temp_res;
		}
	}
	
	if(tc) {
	       	intx_t tc_res = intx_two_complement(res);
		free(res);
		return tc_res;
	}
	return res;
}


intx_t intx_copy(intx_t n) {
	intx_t res = malloc(sizeof(uint32_t)*INT_SIZE);
	for(int i = 0; i < INT_SIZE; i++) res[i] = n[i];
	return res;
}


intx_t intx_set_digit(intx_t n, uint32_t i, bool digit) {
	assert(i < INT_SIZE*32);
	
	n[i/32] = (n[i/32] & ~(1 << i % 32)) + (digit ? 1 : 0) << i % 32;
	return n;
}


bool intx_get_digit(intx_t n, uint32_t i) {
	assert(i < INT_SIZE*32);

	return n[i/32] & (1 << i % 32);
}


intx_t intx_div(intx_t n, intx_t d) {
	bool tc = false;
	if(intx_is_negative(n) && intx_is_negative(d)) {
		n = intx_two_complement(n);
		d = intx_two_complement(d);
	}else if(intx_is_negative(n)) {
		tc = true;
		n = intx_two_complement(n);
	}else if(intx_is_negative(d)) {
		tc = true;
		d = intx_two_complement(d);
	}

	intx_t q = intx_from_64(0);
	intx_t r = intx_from_64(0);

	for(int i = INT_SIZE*32-1; i >= 0; i--) {
		intx_t temp_shift = intx_lshift(r, 1);
		temp_shift = intx_set_digit(temp_shift, 0, intx_get_digit(n, i));
		intx_t temp_sub = intx_sub(temp_shift, d);
		if(!intx_is_negative(temp_sub)) {
			r = temp_sub;
			free(temp_shift);
			intx_set_digit(q, i, true);
		} else {
			r = temp_shift;
			free(temp_sub);
		}

	}
	
	free(r);

	if(tc) {
	       	intx_t tc_q = intx_two_complement(q);
		free(q);
		return tc_q;
	}
	return q;
}


intx_t intx_mod(intx_t n, intx_t d) {
	bool neg = false;
	if(intx_is_negative(n) && intx_is_negative(d)) {
		n = intx_two_complement(n);
		d = intx_two_complement(d);
		neg = true;
	}else if(intx_is_negative(n)) {
		neg = true;
		n = intx_two_complement(n);
	}else if(intx_is_negative(d)) {
		d = intx_two_complement(d);
	}

	intx_t q = intx_from_64(0);
	intx_t r = intx_from_64(0);

	for(int i = INT_SIZE*32-1; i >= 0; i--) {
		intx_t temp_shift = intx_lshift(r, 1);
		temp_shift = intx_set_digit(temp_shift, 0, intx_get_digit(n, i));
		intx_t temp_sub = intx_sub(temp_shift, d);
		if(!intx_is_negative(temp_sub)) {
			r = temp_sub;
			free(temp_shift);
			intx_set_digit(q, i, true);
		} else {
			r = temp_shift;
			free(temp_sub);
		}

	}

	free(q);

	if(neg) {
	       	intx_t temp_r = intx_two_complement(r);
		free(r);
		return temp_r;
	}
	return r;
}


bool intx_is_zero(intx_t n) {
	for(int i = 0; i < INT_SIZE; i++) {
		if(n[i] != 0) return false;
	}
	return true;
}
