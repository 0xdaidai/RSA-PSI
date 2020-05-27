/*************************************************************************
 > File Name: tools.h
 > Author: Guaji
 > Mail: rdwentao@163.com 
 > Created Time: Sun 17 May 2020 05:58:32 PM CST
 ************************************************************************/

#ifndef TOOLS_H
#define TOOLS_H
#include <gmp.h>
#include <stdlib.h>

unsigned char *I2OSP(mpz_t x, int xLen);
void OS2IP(mpz_t x, unsigned char *x_array, int xLen);
void init_state(gmp_randstate_t state);
unsigned char *array_append(unsigned char *src1,long long int Len1,unsigned char *src2,long long int Len2);
void rsa_init(mpz_t n, mpz_t e, mpz_t d);
#endif
