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
	uint32_t carry = 0;
	for(int i = 0; i < INT_SIZE; i++) {
		uint32_t r = a[i] + b[i] + carry;
		if((a[i] & 0x80000000 && b[i] & 0x80000000) || (!(r & 0x80000000) && (a[i] & 0x80000000 || b[i] & 0x80000000))) carry = 1;
		else carry = 0;
		a[i] = r;
	}
	return a;
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
	for(int i = 0; i < INT_SIZE; i++) n[i] = ~n[i];

	return n;
}


intx_t intx_sub(intx_t a, intx_t b) {
	intx_t b_copy = intx_copy(b);
	intx_t comp_2 = intx_two_complement(b_copy);
	intx_add(a, comp_2);
	free(b_copy);
	return a;
}


intx_t intx_lshift(intx_t n, int p) {
	assert(p >= 0);
	
	int chunk_offset = p / 32;
	int shift_offset = p % 32;
	

	for(int i = INT_SIZE-1; i > chunk_offset && i >= 0; i--) 
		n[i] = (n[i-chunk_offset] << shift_offset) + (shift_offset != 0 ? n[i-chunk_offset-1] >> (32-shift_offset) : 0);
	if(chunk_offset < INT_SIZE) n[chunk_offset] = n[0] << shift_offset;
	for(int i = 0; i < chunk_offset && i < INT_SIZE; i++) n[i] = 0;
	return n;
}


intx_t intx_rshift(intx_t n, int p) {
	assert(p >= 0);

	int chunk_offset = p / 32;
	int shift_offset = p % 32;
	
	for(int i = 0; i < INT_SIZE-chunk_offset-1; i++) 
		n[i] = (n[i+chunk_offset] >> shift_offset) + (shift_offset != 0 ? n[i+chunk_offset+1] << (32-shift_offset) : 0);
	if(chunk_offset < INT_SIZE) n[INT_SIZE-chunk_offset-1] = n[INT_SIZE-1] >> shift_offset;
	for(int i = INT_SIZE-chunk_offset; i < INT_SIZE; i++) n[i] = 0;
	return n;
}


intx_t intx_two_complement(intx_t n) {
	return intx_push32(intx_not(n), 1);
}


bool intx_is_negative(intx_t n) {
	return (int32_t) n[INT_SIZE-1] < 0;
}


intx_t intx_mul(intx_t a, intx_t b) {
	bool tc = false;
	b = intx_copy(b);
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
	
	uint64_t product[INT_SIZE][INT_SIZE];

	for(int i = 0; i < INT_SIZE; i++) {
		for(int j = 0; j < INT_SIZE; j++) product[i][j] = (uint64_t) a[i] * (uint64_t) b[j];
		a[i] = 0;
	}
	
	intx_t shift_int = intx_from_64(0);

	for(int i = 0; i < INT_SIZE; i++) {
		for(int j = 0; j < INT_SIZE-1; j++) {
			intx_lshift(shift_int, INT_SIZE*32);
			intx_push32(shift_int, product[i][j] >> 32);
			intx_lshift(shift_int, 32);
			intx_push32(shift_int, (uint32_t) product[i][j]);
			intx_lshift(shift_int, 32*(i+j));
			intx_add(a, shift_int);
		}
	}
	free(shift_int);
	free(b);

	if(tc) {
	       	return intx_two_complement(a);
	}
	return a;
}


intx_t intx_copy(intx_t n) {
	intx_t res = malloc(sizeof(uint32_t)*INT_SIZE);
	for(int i = 0; i < INT_SIZE; i++) res[i] = n[i];
	return res;
}


intx_t intx_set_digit(intx_t n, uint32_t i, bool digit) {
	assert(i < INT_SIZE*32);
	
	n[i/32] = (n[i/32] & ~(1 << i % 32)) + ((digit ? 1 : 0) << i % 32);
	return n;
}


bool intx_get_digit(intx_t n, uint32_t i) {
	assert(i < INT_SIZE*32);

	return n[i/32] & (1 << i % 32);
}


void intx_div(intx_t n, intx_t d, intx_t q, intx_t r) {
	bool neg = false;
	bool tc = true;
	
	d = intx_copy(d);
	n = intx_copy(n);
	
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

	intx_lshift(r, INT_SIZE*32);
	intx_lshift(q, INT_SIZE*32);
	
	for(int i = INT_SIZE*32-1; i >= 0; i--) {
		intx_lshift(r, 1);
		intx_set_digit(r, 0, intx_get_digit(n, i));
		intx_sub(r, d);
		if(!intx_is_negative(r)) {
			intx_set_digit(q, i, true);
		} else {
			intx_add(r, d);
		}

	}
	free(n);
	free(d);

	if(neg) intx_two_complement(r);
	if(tc) intx_two_complement(q);
}


bool intx_is_zero(intx_t n) {
	for(int i = 0; i < INT_SIZE; i++) {
		if(n[i] != 0) return false;
	}
	return true;
}


int intx_cmp(intx_t a, intx_t b) {
	int i = INT_SIZE-1;
	while(i >= 0 && (uint32_t) a[i] == (uint32_t) b[i]) i--;
	if(i < 0) return 0;
	if(i == INT_SIZE-1) return (int32_t) a[i] > (int32_t) b[i] ? 1 : -1;
	else return (uint32_t) a[i] > (uint32_t) b[i] ? 1 : -1;
}


intx_t intx_pow(intx_t b, intx_t p) {
	assert(!intx_is_negative(p));
	
	intx_t res = intx_from_64(1);
	intx_t acc = intx_copy(b);
	intx_t power = intx_copy(p);

	while(!intx_is_zero(power)) {
		if(power[0] & 1) {
			intx_t temp_res = intx_mul(res, acc);
			free(res);
			res = temp_res;
		}
		intx_t temp_acc = intx_mul(acc, acc);
		free(acc);
		acc = temp_acc;
		intx_t temp_power = intx_rshift(power, 1);
		free(power);
		power = temp_power;
	}
	free(power);
	free(acc);
	return res;
}
			

intx_t intx_rand(bool positive) {
	intx_t res = malloc(sizeof(uint32_t)*INT_SIZE);
	for(int i = 0; i < INT_SIZE; i++) res[i] = (rand() << 1) + rand();
	res[INT_SIZE-1] = positive ? res[INT_SIZE-1] & 0x7fffffff : res[INT_SIZE-1] | 0x80000000;
	return res;
}



