/*************************************************************************
 > File Name: test.cpp
 > Author: Guaji
 > Mail: rdwentao@163.com 
 > Created Time: Tue 28 Apr 2020 04:53:14 PM CST
 ************************************************************************/

#include <stdio.h>
#include <gmpxx.h>
#include "ari.h"
int main(){
    mpz_t x;
    mpz_init_set_str(x,"123155416495484621",10);
    printf("%d\n",isprime(x));
    return 0;
}
