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
        int privexist = 0;
        int verbose = 0;
        FILE *input;
        FILE *output;
        FILE *priv;
	
	int opt = 0;
        while ((opt = getopt(argc, argv, OPTIONS)) != -1) {

                switch (opt) {
                        case 'i': //opens input file for reading and gives error msg if doesnt exist
                                input = fopen(optarg, "r");
                                if (input == NULL) {
                                        fprintf(stderr, "%s: Couldn't open %s to read ciphertext: No such file or directory\n", argv[0], optarg);
                                        return 1;
                                }
                                inputexist = 1;
                                break;
                        case 'o': //opens or creates output file for writing and gives error msg if error
                                output = fopen(optarg, "w");
                                if (output == NULL) {
                                        fprintf(stderr, "%s: Couldn't open %s to write deciphered text: No such file or directory\n", argv[0], optarg);
                                        return 1;
                                }
                                outputexist = 1;
                                break;
                        case 'n': //opens private key file for reading and gives error msg if doesnt exist
                                priv = fopen(optarg, "r");
                                if (priv == NULL) {
                                        fprintf(stderr, "%s: couldn't open %s to read private key.\n", argv[0], optarg);
                                        return 1;
                                }
                                privexist = 1;
                                break;
                        case 'v': //enables verbose msg
                                verbose = 1;
                                break;
                        case 'h': //gives help msg and exits program
                                fprintf(stderr, "Usage: %s [options]\n", argv[0]);
                                fprintf(stderr, "  %s decrypts an input file using the specified private key file,\n", argv[0]);
                                fprintf(stderr, "  writing the result to the specified output file.\n");
                                fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
                                fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
                                fprintf(stderr, "    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n");
                                fprintf(stderr, "    -v          : Enable verbose output.\n");
                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
                                return 0;
                                break;
 			default: //gives help msg on invalid arg
				fprintf(stderr, "Usage: %s [options]\n", argv[0]);
                                fprintf(stderr, "  %s decrypts an input file using the specified private key file,\n", argv[0]);
                                fprintf(stderr, "  writing the result to the specified output file.\n");
                                fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
                                fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
                                fprintf(stderr, "    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n");
                                fprintf(stderr, "    -v          : Enable verbose output.\n");
                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
                                return 1;
                             
 		}

        }

	//default files if no file is specified
	if (inputexist == 0) {
                input = stdin;
                if (input == NULL) {
                        fprintf(stderr, "%s: Couldn't open stdin to read ciphertext: No such file or directory\n", argv[0]);
                        return 1;
                }
        }
        if (outputexist == 0) {
                output = stdout;
                if (output == NULL) {
                        fprintf(stderr, "%s: Couldn't open stdout to write deciphered text: No such file or directory\n", argv[0]);
                        return 1;
                }
        }
	if (privexist == 0) {
                priv = fopen("rsa.priv", "r");
                if (priv == NULL) {
			fprintf(stderr, "%s: couldn't open rsa.priv to read private key.\n", argv[0]);
			return 1;
		}
	}
	
	mpz_t n, d;
	mpz_inits(n, d, NULL);
	
	rsa_read_priv(n, d, priv);
	
	//verbose msg
	if (verbose != 0) {
		gmp_fprintf(stderr, "n - modulus (%lu bits): %Zd\n", mpz_sizeinbase(n, 2), n);
		gmp_fprintf(stderr, "d - private exponent (%lu bits): %Zd\n", mpz_sizeinbase(d, 2), d);
	}
	
	//decrypts file
	rsa_decrypt_file(input, output, n, d);

	fclose(input);
	fclose(output);
	fclose(priv);
	mpz_clears(n, d, NULL);
	return 0;
}
