#How to build and run keygen.c  
1. Open terminal
2. Go into the same directory as keygen.c
3. Compile using Makefile by entering "make" into the terminal
4. Run program with "./keygen [OPTIONS]..."

#Command-line options accepted in keygen.c  
- -b _bits_: specifies the minimum bits needed for the public modulus n(default: 1024).
- -i _iters_: specifies the number of Miller-Rabin iterations for testing primes (default: 50).
- -n _pbfile_: specifies the public key file (default: rsa.pub).
- -d _pvfile_: specifies the private key file (default: rsa.priv).
- -s _seed_: specifies the random seed for the random state initialization (default: the seconds since the UNIX epoch, given by the time (NULL))
- -v: enables verbose output.
- -h: displays program synopsis and usage.

#How to build and run encrypt.c  
1. Open terminal
2. Go into the same directory as encrypt.c
3. Compile using Makefile by entering "make" into the terminal
4. Run program with "./encrypt [OPTIONS]..."

#Command-line options accepted in encrypt.c
- -i _infile_: specifies the input file to encrypt (default: stdin)
- -o _outfile_: specifies the output file to encrypt (default: stdout).
- -n _keyfile_: specifies the file containing the public key (default: rsa.pub).
- -v: enables verbose output.
- -h: displays program synopsis and usage.

#How to build and run decrypt.c  
1. Open terminal
2. Go into the same directory as decrypt.c
3. Compile using Makefile by entering "make" into the terminal
4. Run program with "./decrypt [OPTIONS]..."

#Command-line options accepted in decrypt.c
- -i _infile_: specifies the input file to decrypt (default: stdin).
- -o _outfile_: specifies the output file to decrypt (default: stdout).
- -n _keyfile_: specifies the file containing the private key (default: rsa.priv).
- -v: enables verbose output.
- -h: displays program synopsis and usage.
