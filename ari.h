/*************************************************************************
 > File Name: ari.h
 > Author: Guaji
 > Mail: rdwentao@163.com 
 > Created Time: Tue 28 Apr 2020 04:49:22 PM CST
 ************************************************************************/

#ifndef ARI_H
#define ARI_H
#include <gmpxx.h>
void invert(mpz_t e,mpz_t a,mpz_t p);
void gcd_ex(mpz_t x,mpz_t y,const mpz_t a,const mpz_t b);
void fast_pow_mod(mpz_t d,const mpz_t x,const mpz_t n,const mpz_t m);
bool isprime(const mpz_t n);
void gcd(mpz_t r,const mpz_t a,const mpz_t b);
#endif
