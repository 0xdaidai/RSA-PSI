/*************************************************************************
 > File Name: elements.h
 > Author: Guaji
 > Mail: rdwentao@163.com 
 > Created Time: Wed 13 May 2020 06:51:49 PM CST
 ************************************************************************/
#ifndef ELEMENTS_H_INCLUDED
#define ELEMENTS_H_INCLUDED
#define nLen 1024/8

enum datatype{
    ACK,STRDATA,FIN
};

enum datawhat{
    N,E,LAMBDA,BF
};

struct dataHead{
    long long int dataLen;
    enum datatype dataType;
};

struct data{
    struct dataHead head;
    enum datawhat type;
    char msg[1024*10];
    int tmp[];
};

#endif
