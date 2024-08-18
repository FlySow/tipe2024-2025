#include "intx_t.h"
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include "arithmetic.h"

int main() {
	intx_t a = intx_from_64(1);
	intx_print_h(a);
	intx_t b = intx_from_64(-1);
	intx_print_h(b);
	/**eucl_ret res = extended_euclidean(a, b);
	intx_print_h(res.a);
	intx_print_h(res.b);
	intx_print_h(res.gcd);
	intx_t gcd = intx_add(intx_mul(res.a, a), intx_mul(res.b, b));
	intx_print_h(gcd);**/
	intx_t res_add = intx_add(a, b);
	intx_print_h(res_add);
	/**free(res.a);
	free(res.b);
	free(res.gcd);**/
	free(a);
	free(b);
	free(res_add);
}
