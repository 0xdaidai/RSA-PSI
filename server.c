/*************************************************************************
 > File Name: server.c
 > Author: Guaji
 > Mail: rdwentao@163.com 
 > Created Time: Wed 13 May 2020 05:03:05 PM CST
 ************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gmp.h>
#include "elements.h"
#include "tools.h"
#include "bloom.h"


int main(int argc, char *argv[])
{
    int fd, new_fd, numbytes;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    long long int bufsize = BUFSIZ;
    unsigned char buf[bufsize];
    char *buf_pos = buf;
    char *serverID = "127.0.0.1";

    // sercer.addr = '127.0.0.1',12345
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    inet_pton(AF_INET,serverID,&server_addr.sin_addr);
    socklen_t struct_len = sizeof(struct sockaddr_in);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    while(bind(fd, (struct sockaddr *)&server_addr, struct_len) == -1);
    printf("Bind Success!\n");
    while(listen(fd, 10) == -1); // allow 10 connections
    printf("Listening....\n");
    printf("Ready for Accept,Waitting...\n");
    new_fd = accept(fd, (struct sockaddr *)&client_addr, &struct_len);
    printf("Get the Client.\n");
    numbytes = send(new_fd,"Welcome to my server\n",21,0); 
    //Successly get establish a link
    struct data data;
    struct dataHead head;

    mpz_t n,e,d;
    mpz_inits(n,e,d,NULL);
    rsa_init(n, e, d);
    char *num_str = (char*)malloc(sizeof(char)*512);
    memset(num_str, 0, sizeof(char)*512);

    mpz_get_str(num_str,16,n);
    memset(data.msg, 0, 1024);
    printf("dataLen = %d\n", data.head.dataLen);
    data.head.dataType = STRDATA;
    // memcpy(data.msg, &num_str, strlen(num_str)*sizeof(char));
    strcpy(data.msg, num_str);
    printf("data:%s\n",data.msg);
    data.type = N;
    data.head.dataLen = sizeof(data);
    bufsize =sizeof(data);
    printf("bufsize = %d\n",bufsize );
    // buf.resize(bufsize);
    memset(buf, 0, BUFSIZ);
    memcpy(buf, &data, bufsize);
    numbytes = send(new_fd, buf, bufsize, 0);
    printf("send n\n");

    mpz_get_str(num_str,16,e);
    memset(data.msg, 0, 1024);
    printf("dataLen = %d\n", data.head.dataLen);
    data.head.dataType = STRDATA;
    // memcpy(data.msg, &num_str, strlen(num_str)*sizeof(char));
    strcpy(data.msg, num_str);
    printf("data:%s\n",data.msg);
    data.type = E;
    data.head.dataLen = sizeof(data);
    bufsize =sizeof(data);
    printf("bufsize = %d\n",bufsize );
    // buf.resize(bufsize);
    // memset(buf, 0, BUFSIZ);
    // memcpy(buf, &data, bufsize);
    // numbytes = send(new_fd, buf, bufsize, 0);
    // numbytes = send(new_fd, &data, sizeof(data), 0);
    numbytes = send(new_fd, &data, sizeof(data), 0);
    printf("send e\n");

    while((numbytes = recv(new_fd, buf,BUFSIZ,0))<0);
    memcpy(&head, buf, sizeof(head));
    if(head.dataType == ACK)
        printf("base over\n");

    struct bloom bloom;
    unsigned char **X;
    bloom_init(&bloom, 1000000, 0.01);
    unsigned int Ns = 100; //Ns is what?
    mpz_t ins_x;
    mpz_init(ins_x);
    unsigned char *uchar_tmp;

    for(int i = 0; i< Ns; i++){
        uchar_tmp = X[i];
        OS2IP(ins_x, uchar_tmp, nLen);
        mpz_powm(ins_x,ins_x,d,n);
        uchar_tmp = I2OSP(ins_x, nLen);
        bloom_add(&bloom, uchar_tmp, nLen);
    }

    memset(&data,0, sizeof(data));
    memcpy(&data.msg, &bloom, sizeof(bloom));
    data.type = BF;
    data.head.dataType = STRDATA;
    data.head.dataLen = sizeof(data);
    numbytes = send(new_fd, &data, sizeof(data), 0);

    //finish
    mpz_clears(n,e,d,NULL);
    close(new_fd);
    close(fd);
    return 0;
}
