# ifndef _INIT_H_
# define _INIT_H_

#include <gmp.h>

void rsa_init(mpz_t n, mpz_t e, mpz_t d, gmp_randstate_t state)
{
    //*min* = 2^2023, be used to create 1024-bit prime
    //add 1 every time to *base* in order to find the nearest prime around *base*
    mpz_t min, base, tmp, p, q, phi_n, p_1, q_1;
    mpz_init_set_ui(tmp, 1);
    mpz_inits(phi_n, min, base, p, q, p_1, q_1, NULL);
    //min = 2^511
    mpz_mul_2exp(min, tmp, 511);

    //create a 512-bit *base*
    mpz_set_ui(base, 1);
    while (mpz_cmp(base, min) <= 0)
        mpz_urandomb(base, state, 512);
    //find prime from *base*
    mpz_nextprime(p, base);
    mpz_sub_ui(p_1, p, 1);

    //create a 512-bit *base*
    mpz_set_ui(base, 1);
    while (mpz_cmp(base, min) <= 0)
        mpz_urandomb(base, state, 512);
    //find prime from *base*
    mpz_nextprime(q, base);
    mpz_sub_ui(q_1, q, 1);

    //calculate n=p*q
    mpz_mul(n, p, q);
    //calculate phi_n
    mpz_mul(phi_n, p_1, q_1);
    //calculate e
    mpz_urandomm(e, state, phi_n);
    mpz_gcd(tmp, e, phi_n);
    while (mpz_cmp_ui(tmp, 1) != 0)
    {
        mpz_urandomm(e, state, phi_n);
        mpz_gcd(tmp, e, phi_n);
    }
    //calculate d
    mpz_invert(d, e, phi_n);
    mpz_clears(min, base, tmp, p, q, phi_n, p_1, q_1, NULL);
    return;
}

# endif
