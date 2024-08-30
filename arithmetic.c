#include "arithmetic.h"
#include <stdlib.h>
#include <stdio.h>


/**eucl_ret extended_euclidean(intx_t a, intx_t b) {
	intx_t r_prev = intx_copy(a);
	intx_t r = intx_copy(b);
	intx_t r_next = intx_mod(a, b);
	intx_t q = intx_div(a, b);

	intx_t s_prev = intx_from_64(1);
	intx_t s = intx_from_64(0);
	intx_t t_prev = intx_from_64(0);
	intx_t t = intx_from_64(1);
	
	while(!intx_is_zero(r_next)) {
		free(r_prev);
		r_prev = r;
		r = r_next;
		
		intx_t s_mul = intx_mul(s, q);
		intx_t t_mul = intx_mul(t, q);
		
		intx_t s_next = intx_sub(s_prev, s_mul);
		intx_t t_next = intx_sub(t_prev, t_mul);

		free(s_mul);
		free(t_mul);
		free(s_prev);
		free(t_prev);
		t_prev = t;
		s_prev = s;
		t = t_next;
		s = s_next;
		
		r_next = intx_mod(r_prev, r);
		q = intx_div(r_prev, r);
	}
	return (eucl_ret) {a: s, b: t, gcd: r};
}**/


/**bool primality_test_naive(intx_t n) {
	intx_print_h(n);
	intx_t one = intx_from_64(1);
	if(intx_cmp(n, one) <= 0) {
		free(one);
		return false;
	}
	free(one);

	intx_t i;
	intx_t mod = intx_from_64(0);
	for(i = intx_from_64(2); intx_cmp(n, i) > 0; intx_push32(i, 1)) {
		intx_mod(n, i, mod);
		if(intx_is_zero(mod)) return false;
	}
	free(i);
	return true;
}**/


intx_t fermat_mod(intx_t a, intx_t p) {
	intx_t one = intx_from_64(1);

	intx_t acc = intx_copy(a);
	intx_t res = intx_from_64(1);
	intx_t pow = intx_sub(intx_copy(p), one);
	
	while(!intx_is_zero(pow)) {
		if(pow[0] & 1) intx_div(intx_mul(res, acc), p, one, res);
		//intx_print_h(acc);
		intx_mul(acc, acc);
		//intx_print_h(acc);
		intx_div(acc, p, one, acc);
		//intx_print_h(acc);
		intx_rshift(pow, 1);
	}
	free(acc);
	free(pow);
	free(one);
	return res;
}


bool fermat_test(intx_t p, int k) {
	intx_t one = intx_from_64(1);
	intx_t trash = intx_from_64(0);
	//printf("Number is :\n");
	//intx_print_h(p);
	for(int i = 0; i < k; i++) {
		intx_t a = intx_rand(true);
		intx_rshift(a, INT_SIZE/2*32);
		intx_div(a, p, trash, a);
		if(intx_is_zero(a)) a[0] |= 1;
		//intx_print_h(a);
		intx_t mod = fermat_mod(a, p);
		//intx_print_h(mod);
		if(intx_cmp(mod, one) != 0) {
			free(one);
			free(mod);
			return false;
		}
		free(mod);
	}	
	free(one);
	free(trash);
	return true;
}


intx_t generate_prime(bool (* primality_test)(intx_t p, int k), int k) {
	intx_t p = intx_rand(true);
	intx_rshift(p, INT_SIZE*16);
	p[0] |= 1;
	while(!primality_test(p, k)) {
		/**free(p);
		p = intx_rand(true);
		intx_rshift(p, INT_SIZE*16);
		p[0] |= 1;**/
		intx_push32(p, 2);
	}
	return p;
}

