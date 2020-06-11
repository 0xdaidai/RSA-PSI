/*************************************************************************
 > File Name: main.c
 > Author: Guaji
 > Mail: rdwentao@163.com 
 > Created Time: Fri 12 Jun 2020 03:07:53 AM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include <gmp.h>
#include "server.h"
#include "init.h"
#include "rand.h"
#include "bloom.h"
#include "phase.h"

int main(){
    Private a;
    Public b;
    mpz_inits(a.n, a.d, b.e, b.n, NULL);
    gmp_randstate_t state;
    seed_init(state);
    rsa_init(a.n, b.e, a.d, state);
    mpz_set(b.n, a.n);
    struct bloom bf;

    _server_base(&a, &b);
    char send_buf[BUFFER_SIZE] = {0};
    int clientSocket = connect_to_Server((char*)"127.0.0.1",SERVER_PORT);
    if(clientSocket == -1) return 0;
    printf("已连接...\n");

    strcpy(send_buf, "1");
    send(clientSocket, send_buf, sizeof(send_buf), 0);
    submit_File(clientSocket);

}
