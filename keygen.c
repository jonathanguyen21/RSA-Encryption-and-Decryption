#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <gmp.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include "randstate.h"
#include "rsa.h"
#include "numtheory.h"

#define OPTIONS "b:i:n:d:s:vh"

int main(int argc, char **argv) {

	uint64_t bits = 1024;
	uint64_t iterations = 50;
	int pbexist = 0; //initiate default pbfile if value is 0
	int pvexist = 0; //initiate default pvfile if value is 0
	uint64_t seed = time(NULL);
	int verbose = 0;
	FILE *pbfile;
	FILE *pvfile;

	char *ptr;
	int opt = 0;
	while ((opt = getopt(argc, argv, OPTIONS)) != -1 ) {

		switch (opt) {

			case 'b': //bits and if out of range give error message
				if (strtoul(optarg, &ptr, 10) < 50 || strtoul(optarg, &ptr, 10) > 4096) {
					fprintf(stderr, "%s: Number of bits must be 50-4096, not %lu.\n", argv[0], strtoul(optarg, &ptr, 10));
					gmp_fprintf(stderr, "Usage: %s [options]\n", argv[0]);
                               		gmp_fprintf(stderr, "  %s generates a public / private key pair, placing the keys into the public and private\n", argv[0]);
                                	gmp_fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
                                	gmp_fprintf(stderr, "  the program options.\n");
                                	gmp_fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
                                	gmp_fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
                                	gmp_fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
                                	gmp_fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
                                	gmp_fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
                                	gmp_fprintf(stderr, "    -v          : Enable verbose output.\n");
                                	gmp_fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
					return 1;
				}
				else {
					bits = strtoul(optarg, &ptr, 10);
				}
				break;
			case 'i': // iterations and if out of range give error msg
				if (strtoul(optarg, &ptr, 10) < 1 || strtoul(optarg, &ptr, 10) > 500) {
                                        fprintf(stderr, "%s: Number of iterations must be 1-500, not %lu.\n", argv[0], strtoul(optarg, &ptr, 10));
                                        gmp_fprintf(stderr, "Usage: %s [options]\n", argv[0]);
                                        gmp_fprintf(stderr, "  %s generates a public / private key pair, placing the keys into the public and private\n", argv[0]);
                                        gmp_fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
                                        gmp_fprintf(stderr, "  the program options.\n");
                                        gmp_fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
                                        gmp_fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
                                        gmp_fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
                                        gmp_fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
                                        gmp_fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
                                        gmp_fprintf(stderr, "    -v          : Enable verbose output.\n");
                                        gmp_fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
					return 1;
                                }

				iterations = strtoul(optarg, &ptr, 10);
				break;
			case 'n': //creates or opens public key file and gives error msg if error
				pbfile = fopen(optarg, "w+");
				pbexist = 1;
				if (pbfile == NULL) {
					fprintf(stderr, "%s: Can't open file \"%s\"\n", argv[0], optarg);
					return 1;
				}
				break;
			case 'd': //creates or opens private key file and gives error msg if error
				pvfile = fopen(optarg, "w+");
				pvexist = 1;
				if (pvfile == NULL) {
					fprintf(stderr, "%s: Can't open file \"%s\"\n", argv[0], optarg);
					return 1;
				}
				break;
			case 's': //sets seed
				seed = strtoul(optarg, &ptr, 10);
				break;
			case 'v': //enables verbose output
				verbose = 1;
				break;
			case 'h': //prints help msg and exits program
				gmp_fprintf(stderr, "Usage: %s [options]\n", argv[0]);
				gmp_fprintf(stderr, "  %s generates a public / private key pair, placing the keys into the public and private\n", argv[0]);
				gmp_fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
				gmp_fprintf(stderr, "  the program options.\n");
				gmp_fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
				gmp_fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
				gmp_fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
				gmp_fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
				gmp_fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
				gmp_fprintf(stderr, "    -v          : Enable verbose output.\n");
				gmp_fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 0;
				break;
			default: //help msg when wrong argument is passed
                                gmp_fprintf(stderr, "Usage: %s [options]\n", argv[0]);
                                gmp_fprintf(stderr, "  %s generates a public / private key pair, placing the keys into the public and private\n", argv[0]);
                                gmp_fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
                                gmp_fprintf(stderr, "  the program options.\n");
                                gmp_fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
                                gmp_fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
                                gmp_fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
                                gmp_fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
                                gmp_fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
                                gmp_fprintf(stderr, "    -v          : Enable verbose output.\n");
                                gmp_fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 1;


		}
	}
	
	//default files if files are not specified
	if (pbexist == 0) {
		pbfile = fopen("rsa.pub", "w+");
		if (pbfile == NULL) {
			fprintf(stderr, "%s: Can't open file \"rsa.pub\"\n", argv[0]);
			return 1;
		}
	}
	if (pvexist == 0) {
		pvfile = fopen("rsa.priv", "w+");
		if (pvfile == NULL) {
			fprintf(stderr, "%s: Can't open file \"rsa.priv\"\n", argv[0]);
			return 1;
		}
	}

	
	int filenumber = fileno(pvfile);
	fchmod(filenumber, 0600);

	randstate_init(seed);
	
	//create private and public keys and outputs them to correct files
	mpz_t p, q, n, e, d, s, username;
	mpz_inits(p, q, n, e, d, s, username, NULL);
	rsa_make_pub(p, q, n, e, bits, iterations);
	rsa_make_priv(d, e, p, q);
	char user[strlen(getenv("USER")) + 1];
	strcpy(user, getenv("USER"));
	mpz_set_str(username, user, 62);
	rsa_sign(s, username, d, n);
	rsa_write_pub(n, e, s, user, pbfile);
	rsa_write_priv(n, d, pvfile);
	
	if (verbose != 0) {

		fprintf(stderr, "username: %s\n", user);
		gmp_fprintf(stderr, "user signature (%lu bits): %Zd\n", mpz_sizeinbase(s, 2), s);
		gmp_fprintf(stderr, "p (%lu bits): %Zd\n", mpz_sizeinbase(p, 2), p);
		gmp_fprintf(stderr, "q (%lu bits): %Zd\n", mpz_sizeinbase(q, 2), q);
		gmp_fprintf(stderr, "n - modulus (%lu bits): %Zd\n", mpz_sizeinbase(n, 2), n);
		gmp_fprintf(stderr, "e - public exponent (%lu bits): %Zd\n", mpz_sizeinbase(e, 2), e);
		gmp_fprintf(stderr, "d - private exponent (%lu bits): %Zd\n", mpz_sizeinbase(d, 2), d);
	}

	fclose(pbfile);
	fclose(pvfile);
	mpz_clears(p, q, n, e, d, s, username, NULL);
	randstate_clear();
	return 0;
}
