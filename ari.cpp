/*************************************************************************
 > File Name: ari.cpp
 > Author: Guaji
 > Mail: rdwentao@163.com 
 > Created Time: Tue 28 Apr 2020 04:47:17 PM CST
 ************************************************************************/
#include "ari.h"
#include <gmpxx.h>

void gcd_ex_abs(mpz_t x,mpz_t y,const mpz_t a,const mpz_t b)
{
	if(mpz_cmp_d(b,0)==0)
	{
		mpz_set_ui(x,1);
		mpz_set_ui(y,0);
	}
	else
	{
		mpz_t x1,y1,tmp;
		mpz_init(x1);
		mpz_init(y1);
		mpz_init(tmp);
		mpz_mod(tmp,a,b); //tmp=a%b
		gcd_ex_abs(x1,y1,b,tmp);
		mpz_set(x,y1); //x=y1
		mpz_fdiv_q(y,a,b); //y=a/b
		mpz_mul(y,y,y1); //y=a/b*y1
		mpz_sub(y,x1,y); //y=x1-a/b*y1
		mpz_clears(x1,y1,tmp,NULL);
	}
}

void gcd_ex(mpz_t x,mpz_t y,const mpz_t a,const mpz_t b)
{
	mpz_t tmpa,tmpb;
	mpz_init_set(tmpa,a);
	mpz_init_set(tmpb,b);
	if(mpz_cmp_d(tmpa,0)>0&&mpz_cmp_d(tmpb,0)>0)
	{
		gcd_ex_abs(x,y,tmpa,tmpb);
	}
	else if(mpz_cmp_d(tmpa,0)<0&&mpz_cmp_d(tmpb,0)>0)
	{
		mpz_neg(tmpa,tmpa);
		gcd_ex_abs(x,y,tmpa,tmpb);
		mpz_neg(x,x);
	}
	else if(mpz_cmp_d(tmpa,0)>0&&mpz_cmp_d(tmpb,0)<0)
	{
		mpz_neg(tmpb,tmpb);
		gcd_ex_abs(x,y,tmpa,tmpb);
		mpz_neg(y,y);
	}
	else
	{
		mpz_neg(tmpa,tmpa);
		mpz_neg(tmpb,tmpb);
		gcd_ex_abs(x,y,tmpa,tmpb);
		mpz_neg(x,x);
		mpz_neg(y,y);
	}
	mpz_clear(tmpa);
	mpz_clear(tmpb);
}

void invert(mpz_t e,mpz_t a,mpz_t p) //扩展欧几里得求逆元 
{
	mpz_t y;
	mpz_init(y);
	gcd_ex(e,y,a,p);
	mpz_clear(y);
	mpz_add(e,e,p);
	mpz_mod(e,e,p);
}

void fast_pow_mod(mpz_t d,const mpz_t x,const mpz_t n,const mpz_t m)
{
	mpz_t tmp,tmpx,tmpn; //创建x和n的副本，避免被修改 
	mpz_init(tmp);
	mpz_init(tmpx);
	mpz_init(tmpn);
	mpz_set_ui(d,1);
	mpz_set(tmpx,x);
	mpz_set(tmpn,n);
	while(mpz_cmp_d(tmpn,0)>0) //n>0
	{
		mpz_mod_ui(tmp,tmpn,2);
		if(mpz_cmp_d(tmp,1)==0) //n%2==1
		{
			mpz_mul(d,d,tmpx);
			mpz_mod(d,d,m); //d=d*x mod m
		}
		mpz_fdiv_q_ui(tmpn,tmpn,2); //n/=2
		mpz_mul(tmpx,tmpx,tmpx);
		mpz_mod(tmpx,tmpx,m); //x=x*x mod m
	}
	mpz_clear(tmp);
	mpz_clear(tmpx);
	mpz_clear(tmpn);
}

void gcd(mpz_t r,const mpz_t a,const mpz_t b)
{
	if(mpz_cmp_d(b,0)==0)
		mpz_set(r,a);
	else
	{
		mpz_t tmp,ans;
		mpz_init(tmp);
		mpz_init(ans);
		mpz_mod(tmp,a,b);
		gcd(ans,b,tmp);
		mpz_set(r,ans);
		mpz_clear(tmp);
		mpz_clear(ans);
	}
}

bool isprime(const mpz_t n)
{
	const unsigned int times=50; //重复检测的次数 
	if(mpz_cmp_d(n,2)==0) //2是素数 
		return true;
	if(mpz_cmp_d(n,1)==0) //1不是素数 
		return false;
	if(mpz_even_p(n)) //偶数不是素数 
		return false;
	bool flag=true; //是否是素数 
	mpz_t q,k,a,tmp,exp,ans,count;
	mpz_init(q);
	mpz_sub_ui(q,n,1); //q=n-1
	mpz_init_set_ui(k,0); //k=0
	mpz_init(a);
	mpz_init(tmp);
	mpz_init(exp);
	mpz_init(ans);
	mpz_init(count);
	while(mpz_even_p(q)) //求k和q使得n-1=q*2^k,q为奇数 
	{
		mpz_fdiv_q_ui(q,q,2);
		mpz_add_ui(k,k,1);
	}
	gmp_randstate_t state;
	gmp_randinit_default(state);
	for(unsigned int i=0;i<times;i++)
	{
		bool maybe=false; //是否可能是素数，如果是则继续下一轮检测，如果否直接退出 
		gmp_randseed_ui(state,time(NULL));
		mpz_sub_ui(tmp,n,2);
		mpz_urandomm(a,state,tmp);
		mpz_add_ui(a,a,1); //产生一个[1,n-1]的随机数a 
		fast_pow_mod(tmp,a,q,n); //求a^q mod n 
		if(mpz_cmp_d(tmp,1)==0) //可能是素数，继续下一轮检测 
			continue;
		mpz_set(exp,q);
		for(mpz_set_ui(count,0);mpz_cmp(count,k)<0;mpz_add_ui(count,count,1))
		{
			fast_pow_mod(ans,a,exp,n); //ans=a^exp mod n
			mpz_sub_ui(tmp,n,1); //tmp=n-1
			if(mpz_cmp(ans,tmp)==0) //a^exp mod n == n-1，可能是素数，继续下一轮检测 
			{
				maybe=true;
				break;	
			}
			mpz_mul_ui(exp,exp,2); //exp*=2;
		}
		if(!maybe)
		{
			flag=false;
			break;
		}
	}
	mpz_clears(q,k,a,tmp,exp,ans,count,NULL);
	gmp_randclear(state);
	if(flag==true)
		return true;
	else
		return false;
}

