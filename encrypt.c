#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <gmp.h>
#include <time.h>
#include <limits.h>
#include <sys/stat.h>
#include <string.h>
#include "randstate.h"
#include "rsa.h"
#include "numtheory.h"

#define OPTIONS "i:o:n:vh"

int main(int argc, char **argv) {

	int inputexist = 0;
	int outputexist = 0;
	int publicexist = 0;
	int verbose = 0;
	FILE *input;
	FILE *output;
	FILE *public;

	int opt = 0;
	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {

		switch (opt) {
			case 'i': //opens input file for reading and gives error msg if file doesnt exist
				input = fopen(optarg, "r");
				if (input == NULL) {
					fprintf(stderr, "%s: Couldn't open %s to read plaintext: No such file or directory\n", argv[0], optarg);
					return 1;
				}
				inputexist = 1;
				break;
			case 'o': //opens output file for writing and gives error msg on error
				output = fopen(optarg, "w");
				if (output == NULL) {
                                        fprintf(stderr, "%s: Couldn't open %s to write plaintext: No such file or directory\n", argv[0], optarg);
                                        return 1;
                                }
				outputexist = 1;
				break;
			case 'n': //opens public key file for reading and gives error msg if doesnt exist
				public = fopen(optarg, "r");
				if (public == NULL) {
                                        fprintf(stderr, "%s: couldn't open %s to read public key.\n", argv[0], optarg);
					fprintf(stderr, "Usage: %s [options]\n", argv[0]);
					fprintf(stderr, "  %s encrypts an input file using the specified public key file,\n", argv[0]);
					fprintf(stderr, "  writing the result to the specified output file.i\n");
					fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
					fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
					fprintf(stderr, "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n");
					fprintf(stderr, "    -v          : Enable verbose output.\n");
					fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
                                        return 1;
                                }
				publicexist = 1;
				break;
			case 'v': //enables verbose msg
				verbose = 1;
				break;
			case 'h': //help msg and exits program
				fprintf(stderr, "Usage: %s [options]\n", argv[0]);
				fprintf(stderr, "  %s encrypts an input file using the specified public key file,\n", argv[0]);
				fprintf(stderr, "  writing the result to the specified output file.\n");
				fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
				fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
				fprintf(stderr, "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n");
				fprintf(stderr, "    -v          : Enable verbose output.\n");
				fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 0;
				break;
			default: //help msg if wrong arg and exits program
				fprintf(stderr, "Usage: %s [options]\n", argv[0]);
                                fprintf(stderr, "  %s encrypts an input file using the specified public key file,\n", argv[0]);
                                fprintf(stderr, "  writing the result to the specified output file.\n");
                                fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
                                fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
                                fprintf(stderr, "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n");
                                fprintf(stderr, "    -v          : Enable verbose output.\n");
                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 1;
		}

	}

	//default files if not specified
	if (inputexist == 0) {
		input = stdin;
		if (input == NULL) {
			fprintf(stderr, "%s: Couldn't open stdin to read plaintext: No such file or directory\n", argv[0]);
			return 1;
		}
	}
	if (outputexist == 0) {
		output = stdout;
		if (output == NULL) {
			fprintf(stderr, "%s: Couldn't open stdout to write plaintext: No such file or directory\n", argv[0]);
                        return 1;
		}
	}
	if (publicexist == 0) {
		public = fopen("rsa.pub", "r");
		if (public == NULL) {
			fprintf(stderr, "%s: couldn't open rsa.pub to read public key.\n", argv[0]);
                        fprintf(stderr, "Usage: %s [options]\n", argv[0]);
                        fprintf(stderr, "  %s encrypts an input file using the specified public key file,\n", argv[0]);
                        fprintf(stderr, "  writing the result to the specified output file.i\n");
                        fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
                        fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
                        fprintf(stderr, "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n");
                        fprintf(stderr, "    -v          : Enable verbose output.\n");
                        fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
			return 1;
		}
	}
	char username[LOGIN_NAME_MAX];
	mpz_t n, e, s, user;
	mpz_inits(n, e, s, user, NULL);

	rsa_read_pub(n, e, s, username, public);

	if (verbose == 1) {
		fprintf(stderr, "username: %s\n", username);
		gmp_fprintf(stderr, "user signature (%lu bits): %Zd\n", mpz_sizeinbase(s, 2), s);
		gmp_fprintf(stderr, "n - modulus (%lu bits): %Zd\n", mpz_sizeinbase(n, 2), n);
		gmp_fprintf(stderr, "e - public exponent (%lu bits): %Zd\n", mpz_sizeinbase(e, 2), e);

	}

	mpz_set_str(user, username, 62);
	
	//Verifies signature
	if (rsa_verify(user, s, e, n) == false) {
		gmp_fprintf(stderr, "Couldn't verify signature.\n");
		return 1;
	}

	//encrypts file if signature is correct
	rsa_encrypt_file(input, output, n, e);

	mpz_clears(n, e, s, user, NULL);
	fclose(input);
	fclose(output);
	fclose(public);
	return 0;
}
