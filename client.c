/*************************************************************************
 > File Name: client.c
 > Author: Guaji
 > Mail: rdwentao@163.com 
 > Created Time: Sat 13 Jun 2020 07:02:18 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include <gmp.h>
#include "init.h"
#include "rand.h"
#include "bloom.h"
#include "phase.h"
#include "file.h"

int main(){
    Public b;mpz_inits(b.n,b.e,NULL);
    struct bloom bf;

    char send_buf[BUFFER_SIZE] = {0};
    char file_buf[1024];
    char *filename;
    int clientSocket = connect_to_Server((char*)"39.96.57.195",SERVER_PORT);
    if(clientSocket == -1) return 0;
    printf("已连接到服务器...\n");

    memset(file_buf,0,1024);
    FILE *fp = fopen("./resource/pub-n.key","rb");
    fseek(fp,0,SEEK_END); 
    int tt = ftell(fp);
    fseek(fp,0,SEEK_SET);
    fread(file_buf,1,tt,fp);
    printf("b.n:%s\n", file_buf);       
    mpz_set_str(b.n,file_buf,16);

    memset(file_buf,0,1024);
    fp = fopen("./resource/pub-e.key","rb");
    fseek(fp,0,SEEK_END); 
    tt = ftell(fp);
    fseek(fp,0,SEEK_SET);
    fread(file_buf,1,tt,fp);
    printf("b.e:%s\n", file_buf);       
    mpz_set_str(b.e,file_buf,16);

    printf("base qian\n");
    _client_base(&b, 1000, "./resource/r.txt");
    printf("base\n");

    filename = receive_File(clientSocket);
    _bloom_read(&bf,filename);

    _client_online_send(&b, 1000, "./resource/y.txt", "./resource/r.txt", "./resource/a.txt");
    printf("1\n");
    submit_sel_File(clientSocket,"a.txt");
    filename = receive_File(clientSocket);
    _client_online_receive(&b, 1000, &bf, filename, "./resource/r.txt", "./resource/y.txt", "./resource/s.txt");

    mpz_clears(b.n,b.e,NULL);
    return 0;

}
