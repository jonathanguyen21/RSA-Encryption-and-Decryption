#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <gmp.h>
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"
/*
int main(void) {
	randstate_init(613);
	//rsa_make_pub TEST
	mpz_t p, q, n, e, d;
	mpz_inits(p, q, n, e, d, NULL);
	uint64_t nbits = 50;
	uint64_t iters = 50;
	rsa_make_pub(p, q, n, e, nbits, iters);
	gmp_printf("rsa_make_pub(%lu, %lu) e = %Zd, p = %Zd, q = %Zd, n = %Zd\n", nbits, iters, e, p, q, n);
	
	
	//rsa_write_pub TEST
	char username[] = "boob";
	FILE *pbfile = fopen("text.txt", "w");
	rsa_write_pub(n, e, q, username, pbfile);
	fclose(pbfile);
	
	
	//rsa_read_pub TEST
	mpz_set_ui(n, 10);
	pbfile = fopen("text.txt", "r");
	rsa_read_pub(n, e, q, username, pbfile);
	printf("user: %s\n", username);
	fclose(pbfile);

	//rsa_make_priv TEST
	rsa_make_priv(d, e, p, q);
	gmp_printf("rsa_make_priv(%Zd, %Zd, %Zd) = %Zd\n", e, p, q, d);
	
	//rsa_encrypt TEST
	mpz_t m, c;
	mpz_inits(m, c, NULL);
	mpz_set_ui(m, 132324);
	rsa_encrypt(c, m, e, n);
	gmp_printf("rsa_encrypt(%Zd, %Zd, %Zd) = %Zd\n", m, e, n, c);

	//rsa verify TEST
	mpz_t s;
	mpz_inits(s, NULL);
	mpz_set_ui(s, 192949128172018);
	mpz_set_ui(m, 161358104791037);
	mpz_set_ui(e, 196669481948819);
	mpz_set_ui(n, 762044608432007);
	printf("RSA verify = %s\n", rsa_verify(m, s, e, n) ? "true" : "false");

	mpz_set_ui(n, 764062864999873);
	mpz_set_ui(e, 350094851733757);
	mpz_set_ui(d, 192543875130593);

	//ENCRYPT FILE TEST
	gmp_printf("n = %Zd e = %Zd d = %Zd\n", n, e, d);
	FILE *infile = fopen("infile.txt", "r");
	FILE *outfile = fopen("outfile.txt", "w");
	rsa_encrypt_file(infile, outfile, n, e);
	fclose(infile);
        fclose(outfile);

	//DECRYPT FILE TEST
	FILE *decrypted = fopen("decrypted.txt", "w");
	outfile = fopen("outfile.txt", "r");
	
	rsa_decrypt_file(outfile, decrypted, n, d);	


	fclose(decrypted);
	fclose(outfile);

	mpz_clears(p, q, n, e, d, m, c, s, NULL);
	randstate_clear();
	return 0;

}
*/

//makes public modulus n and public exponent e
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {

	uint64_t pbits = (random() % (((3 * nbits) / 4) - (nbits / 4))) + (nbits / 4);
	uint64_t qbits = nbits - pbits;
	//makes primes of size bits
	make_prime(p, pbits, iters);
	make_prime(q, qbits, iters);

	mpz_t totientp, totientq, totientn, gcdpq, lcm, coprime;
	mpz_inits(totientp, totientq, totientn, gcdpq, lcm, coprime, NULL);
	
	//n is the product of primes p and q
	mpz_mul(n, p, q);
	mpz_sub_ui(totientp, p, 1);
	mpz_sub_ui(totientq, q, 1);
	mpz_mul(totientn, totientp, totientq);
	gcd(gcdpq, totientp, totientq);
	mpz_cdiv_q(lcm, totientn, gcdpq);

	while (true) {

		mpz_urandomb(e, state, nbits);
		gcd(coprime, e, lcm);
		if (mpz_get_ui(coprime) == 1) {
			break;
		}

	}	
	mpz_clears(totientp, totientq, totientn, gcdpq, lcm, coprime, NULL);

}

//writes public keys signature and user to file
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
	gmp_fprintf(pbfile, "%Zx\n", n);
	gmp_fprintf(pbfile, "%Zx\n", e);
	gmp_fprintf(pbfile, "%Zx\n", s);
	gmp_fprintf(pbfile, "%s\n", username);
}

//reads public keys from file
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
	gmp_fscanf(pbfile, "%Zx\n", n);
	gmp_fscanf(pbfile, "%Zx\n", e);
	gmp_fscanf(pbfile, "%Zx\n", s);
	gmp_fscanf(pbfile, "%s\n", username);
}

//makes private keys
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
	mpz_t totientp, totientq, totientn, lcm, gcdpq;
	mpz_inits(totientp, totientq, totientn, lcm, gcdpq, NULL);

	mpz_sub_ui(totientp, p, 1);
	mpz_sub_ui(totientq, q, 1);
	mpz_mul(totientn, totientp, totientq);
	gcd(gcdpq, totientp, totientq);
	mpz_cdiv_q(lcm, totientn, gcdpq);

	mod_inverse(d, e, lcm);
	mpz_clears(totientp, totientq, totientn, lcm, gcdpq, NULL);
}

//writes private keys to file
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
	gmp_fprintf(pvfile, "%Zx\n", n);
        gmp_fprintf(pvfile, "%Zx\n", d);
}

//read private keys from file
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
	gmp_fscanf(pvfile, "%Zx\n", n);
        gmp_fscanf(pvfile, "%Zx\n", d);
}

//encrypts m using public mod n and public exponent e
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
	pow_mod(c, m, e, n);
}

//encrypts a file and outputs ciphertext
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
	mpz_t m, c;
        mpz_inits(m, c, NULL);

	uint8_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
	void* mem_p = malloc(k * sizeof(uint8_t));
	uint8_t * arr = (uint8_t *) mem_p;
	arr[0] = 0xFF; //prepend space
	uint8_t* array = &arr[1];
	
	// while loop until end of file to encrypt all blocks
	while(1) {
		if (feof(infile)) {
			break;
		}
		uint8_t j = fread(array, sizeof(uint8_t), k-1, infile);
		mpz_import(m, j + 1, 1, sizeof(arr[0]), 1, 0, arr);
		rsa_encrypt(c, m, e, n);
		gmp_fprintf(outfile, "%Zx\n", c);
	}
	mpz_clears(m, c, NULL);
	free(mem_p);
}

//decrypts c using private key d and pub mod n
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
	pow_mod(m, c, d, n);
}

//decrypts file and outputs decrypted text
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
	uint8_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
	void* mem_p = malloc(k * sizeof(uint8_t));
        uint8_t * arr = (uint8_t *) mem_p;
	
	mpz_t c, m;
	mpz_inits(c, m, NULL);
	
	size_t j;
	uint8_t* array = &arr[1];	

	//goes through file until end of file to decrypt all blocks
	while(1) {
		if (feof(infile)) {
			break;
		}
		gmp_fscanf(infile, "%Zx\n", c);
		rsa_decrypt(m, c, d, n);
		mpz_export(arr, &j, 1, sizeof(arr[0]), 1, 0, m);
		fwrite(array, sizeof(uint8_t), j-1, outfile);	
	}

	mpz_clears(c, m, NULL);
	free(mem_p);
}

//creates signature
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
	pow_mod(s, m, d, n);
}

//verifies signature
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
	mpz_t t;
	mpz_inits(t, NULL);

	pow_mod(t, s, e, n);
	if (mpz_get_ui(t) == mpz_get_ui(m)) {
		mpz_clears(t, NULL);
		return true;
	}
	else {
		mpz_clears(t, NULL);
		return false;
	}
}
