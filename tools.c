/*************************************************************************
 > File Name: tools.cpp
 > Author: Guaji
 > Mail: rdwentao@163.com 
 > Created Time: Sun 17 May 2020 05:56:52 PM CST
 ************************************************************************/

#include <gmp.h>
#include <stdlib.h>
#include "tools.h"

/*///////////////////////////////
I2OSP change mpz_t into bytearray
:x: a big number
:xLen: the length of bytearray
:return: a bytearray 'x_array'
*/
/////////////////////////////////
unsigned char *I2OSP(mpz_t x, int xLen)
{
    mpz_t tmp, base;
    int i = 0;
    mpz_inits(tmp, base, NULL);
    mpz_ui_pow_ui(tmp, 256, xLen);
    if (mpz_cmp(x, tmp) >= 0){
        printf("I2OSP: x overflow");
        exit(-1);
    }
    mpz_ui_pow_ui(base, 256, xLen - 1);
    unsigned char *x_array = (unsigned char *)malloc(sizeof(unsigned char) * xLen);
    for (i = 0; i < xLen; i++)
    {
        mpz_tdiv_q(tmp, x, base);
        x_array[i] = mpz_get_ui(tmp);
        mpz_tdiv_q_ui(base, base, 256);
    }
    mpz_clears(tmp, base, NULL);
    return x_array;
}

/*///////////////////////////////////////////////////
OS2OP change bytearray into mpz_t
:x: stores the result
:x_array: the bytearray which is needed to be changed
:xLen: the length of x_array
:return: no return, result is stored in 'x'
*/
/////////////////////////////////////////////////////
void OS2IP(mpz_t x, unsigned char *x_array, int xLen)
{
    mpz_t base;
    mpz_init_set_ui(base, 1);
    mpz_set_ui(x, 0);
    int i = 0;
    for (i = 0; i < xLen; i++)
    {
        mpz_addmul_ui(x, base, x_array[xLen - 1 - i]);
        mpz_mul_ui(base, base, 256);
    }
    mpz_clears(base, NULL);
    return;
}

/*///////////////////////////////////////////////////
 * init gmp_random_state
 */
/////////////////////////////////////////////////////
void init_state(gmp_randstate_t state){
    unsigned long int cur;
    cur = ((time(0) % 1000) + 10000) * ((clock() % 1000) + 10000);
    gmp_randinit_default(state);
    gmp_randseed_ui(state, cur);
}

/*/////////////////////////////////////////
array_append is used to link two bytearrays
:src1: the first byte[]
:Len1: the length of src1
:src2: the second byte[]
:Len2: the length of src2
:return: the final byte[]
*/
///////////////////////////////////////////
unsigned char *array_append(unsigned char *src1,long long int Len1,unsigned char *src2,long long int Len2)
{
    unsigned char *dest = (unsigned char *)malloc(sizeof(unsigned char) * (Len1 + Len2));
    int pos = 0;
    for (pos = 0; pos < Len1; pos++)
        dest[pos] = src1[pos];
    for (pos = 0; pos < Len2; pos++)
        dest[Len1 + pos] = src2[pos];
    return dest;
}

void rsa_init(mpz_t n, mpz_t e, mpz_t d )
{
    gmp_randstate_t state;
    init_state(state);
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
