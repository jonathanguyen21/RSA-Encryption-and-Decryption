#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <gmp.h>
#include "numtheory.h"
#include "randstate.h"
/*
int main(void) {
	randstate_init(613);
	//GCD TEST
	mpz_t a, b, d;
	mpz_inits(a, b, d, NULL);
	mpz_set_ui(a, 10000);
	mpz_set_ui(b, 500);
	gcd(d,a,b);
	gmp_printf("gcd(%Zd, %Zd) = %Zd\n", a, b, d);	
	mpz_clears(a, b, d, NULL);

	//pow_mod TEST
        mpz_t output, base, exponent, mod;
        mpz_inits(output, base, exponent, mod, NULL);
        mpz_set_ui(base, 192949128172018);
        mpz_set_ui(exponent, 196669481948819);
	mpz_set_ui(mod, 762044608432007);
        pow_mod(output, base, exponent, mod);
        gmp_printf("pow_mod(%Zd, %Zd, %Zd) = %Zd\n", base, exponent, mod, output);
        mpz_clears(output, base, exponent, mod, NULL);

	//mod_inverse TEST
        mpz_t o, bases, n;
        mpz_inits(o, bases, n, NULL);
        mpz_set_ui(bases, 31);
        mpz_set_ui(n, 56);
        mod_inverse(o,bases,n);
        gmp_printf("mod_inverse(%Zd, %Zd) = %Zd\n", bases, n, o);
        mpz_clears(o, bases, n, NULL);
	
	//is_prime TEST
	mpz_t num;
        mpz_inits(num, NULL);
        mpz_set_ui(num, 1000);
        uint64_t iters = 40;
	bool truefalse = is_prime(num, iters);
        gmp_printf("is_prime(%Zd, %lu) = %d\n", num, iters, truefalse);
        mpz_clears(num, NULL);

	//make_prime TEST
	mpz_t p;
        mpz_inits(p, NULL);
        uint64_t bits = 20;
        iters = 40;
        make_prime(p, bits, iters);
        gmp_printf("make_prime(%lu, %lu) = %Zd\n", bits, iters, p);
        mpz_clears(p, NULL);

	randstate_clear();
	return 0;
}
*/

//greatest common divisor of a and b
void gcd(mpz_t d, mpz_t a, mpz_t b) {
	mpz_t t, t1, t2; //TEMP VALUE
	mpz_inits(t, t1, t2, NULL);

	mpz_set(t2, a);
	mpz_set(t1, b);
	while (mpz_get_ui(t1) != 0) {
		mpz_set(t, t1);
		mpz_mod(t1, t2, t1);
		mpz_set(t2, t);
	}
	mpz_set(d, t2);
	mpz_clears(t,t1, t2, NULL);
}

//modulus inverse of a and n
void mod_inverse(mpz_t o, mpz_t a, mpz_t n) {
	mpz_t r, r1, t1, q, temp;
	mpz_inits(r, r1, t1, q, temp, NULL);

	mpz_set(r, n);
	mpz_set(r1, a);
	mpz_set_ui(o, 0);
	mpz_set_ui(t1, 1);

	while (mpz_get_ui(r1) != 0) {

		mpz_tdiv_q(q, r, r1);
		mpz_set(temp, r);
		mpz_set(r, r1);
		mpz_mul(r1, q, r1);
		mpz_sub(r1, temp, r1);

		mpz_set(temp, o);
		mpz_set(o, t1);
	        mpz_mul(t1, q, t1);
                mpz_sub(t1, temp, t1);
	}

	if (mpz_get_ui(r) > 1) {
		mpz_set_ui(o, 0);
		return;
	}
	if (mpz_get_si(o) < 0) {
		mpz_add(o, o, n);
	}
	mpz_clears(r, r1, t1, q, temp, NULL);
}

//power mod using shortcut of a^d % n
void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {
	mpz_t p, d2, odd, v;
	mpz_inits(p, d2, odd, v, NULL);
	
	mpz_set_ui(v, 1);
	mpz_set(p, a);
	mpz_set(d2, d);

	while (mpz_get_ui(d2) > 0) {
		mpz_mod_ui(odd, d2, 2);
		if (mpz_get_ui(odd) != 0) { //IF odd is ODD
			mpz_mul(v, v, p);
			mpz_mod(v, v, n);
		}

		mpz_mul(p, p, p);
		mpz_mod(p, p, n);
		mpz_fdiv_q_ui(d2, d2, 2);
	}
	
	mpz_set(o,v);
	mpz_clears(p, d2, odd, v, NULL);
}

//checks if n is prime
bool is_prime(mpz_t n, uint64_t iters) {
	//if statements for error cases
	if (mpz_get_ui(n) == 0 || mpz_get_ui(n) == 1) {
		return false;
	}
	if (mpz_get_ui(n) == 2 || mpz_get_ui(n) == 3) {
		return true;
	}
	if (mpz_get_ui(n) % 2 == 0) {
		
		return false;
	}


	mpz_t s, r, a, y, j, odd, random, rng, sminusone, two;
	mpz_inits(s, r, a, y, j, odd, random, rng, sminusone, two, NULL);
	

	mpz_sub_ui(r, n, 1);
	mpz_set(random, r);
	mpz_sub_ui(rng, random, 2);
	mpz_mod_ui(odd, r, 2);
	if (mpz_get_ui(odd) != 0) {
		mpz_set_ui(s, 0);
	}
	else { //dividing n-1 by 2 until it is an odd number
		
		while (mpz_get_ui(odd) == 0) {

			mpz_fdiv_q_ui(r, r, 2);
			mpz_add_ui(s, s, 1);
			mpz_mod_ui(odd, r, 2);
		}
	}
	
	mpz_sub_ui(sminusone, s, 1);

	//random variable = n - 1
	for (uint64_t i = 0; i < iters; i++) {
		
		mpz_urandomm(a, state, rng);
		mpz_add_ui(a, a, 2);
		pow_mod(y, a, r, n);

		if (mpz_get_ui(y) != 1 && mpz_get_ui(y) != mpz_get_ui(random)) {
			mpz_set_ui(j, 1);
			
			
			mpz_set_ui(two, 2);
			while ((mpz_get_ui(j) <= mpz_get_ui(sminusone)) && (mpz_get_ui(y) != mpz_get_ui(random))) {
				pow_mod(y, y, two, n);
				if (mpz_get_ui(y) == 1) {
					mpz_clears(s, r, a, y, j, odd, random, rng, sminusone, two, NULL);
					return false;
				}
				mpz_add_ui(j, j, 1);
			}
			if (mpz_get_ui(y) != mpz_get_ui(random)) {
				mpz_clears(s, r, a, y, j, odd, random, rng, sminusone, two, NULL);
				return false;
			}
		}
	}
	
	mpz_clears(s, r, a, y, j, odd, random, rng, sminusone, two, NULL);
	return true;
}

//makes a prime number
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {

	mpz_urandomb(p, state, bits);
	
	//if p is even add one to make it odd and check if its prime
	if (mpz_get_ui(p) % 2 == 0) {
		mpz_add_ui(p, p, 1);
	}
	while (is_prime(p, iters) == false || mpz_sizeinbase(p, 2) < bits) {
		mpz_urandomb(p, state, bits);
	}
}
