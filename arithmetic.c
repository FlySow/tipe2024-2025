#include "arithmetic.h"
#include <stdlib.h>
#include <stdio.h>


eucl_ret extended_euclidean(intx_t a, intx_t b) {
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
		
		printf("s & t prev\n");
		intx_print_h(s_prev);
		intx_print_h(t_prev);
		printf("s & t\n");
		intx_print_h(s);
		intx_print_h(t);
		printf("q\n");
		intx_print_h(q);
		printf("Res x s & t\n");
		intx_print_h(s_mul);
		intx_print_h(t_mul);
		intx_t s_next = intx_sub(s_prev, s_mul);
		intx_t t_next = intx_sub(t_prev, t_mul);
		printf("Res sub s & t\n");
		intx_print_h(s_next);
		intx_print_h(t_next);
		free(s_mul);
		free(t_mul);
		free(s_prev);
		free(t_prev);
		t_prev = t;
		s_prev = s;
		t = t_next;
		s = s_next;
		
		r_next = intx_mod(r_prev, r);
		intx_t q = intx_div(r_prev, r);
	}
	return (eucl_ret) {a: s, b: t, gcd: r};
}



