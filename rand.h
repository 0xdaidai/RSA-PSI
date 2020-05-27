#ifndef RAND_H_INCLUDED
#define RAND_H_INCLUDED

#include <gmp.h>
#include<time.h>

void seed_init(gmp_randstate_t state) {
    unsigned long int cur = 0;
    //create random *cur* according to *time and clock*
    cur = ((time(0) % 1000) + 10000) * ((clock() % 1000) + 10000);
    //initialize state
    gmp_randinit_default(state);
    gmp_randseed_ui(state, cur);
}

void generator_rand(mpz_t random, int randlen, gmp_randstate_t state) {
    mpz_t min;
    mpz_init_set_ui(min, 1);
    mpz_set_ui(random, 1);
    mpz_mul_2exp(min, min, randlen - 1);
    //if you want a random from 0 to (2^randlen)-1, use the following instead of while-loop
    //mpz_urandomb(random, state, randlen);
    //while-loop generate a random from 2^(randlen-1) to (2^randlen)-1, which means a randlen-bits random
    while (mpz_cmp(random, min) <= 0)
        mpz_urandomb(random, state, randlen);
    mpz_clears(min, NULL);
}

void generator_prime(mpz_t prime, int randlen, gmp_randstate_t state) {
    mpz_t min, base;
    mpz_init_set_ui(min, 1);
    mpz_init_set_ui(base, 1);
    mpz_mul_2exp(min, min, randlen - 1);
    //if you want a random from 0 to (2^randlen)-1, use the following instead of while-loop
    //mpz_urandomb(base, state, randlen);
    while (mpz_cmp(base, min) <= 0)
        mpz_urandomb(base, state, randlen);
    mpz_nextprime(prime,base);
    mpz_clears(min, base, NULL);
}


#endif // RAND_H_INCLUDED
