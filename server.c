/*************************************************************************
 > File Name: main.c
 > Author: Guaji
 > Mail: rdwentao@163.com 
 > Created Time: Fri 12 Jun 2020 03:07:53 AM CST
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
    Private a;
    Public b;
    mpz_inits(a.n, a.d, b.e, b.n, NULL);
    gmp_randstate_t state;
    seed_init(state);
    rsa_init(a.n, b.e, a.d, state);
    mpz_set(b.n, a.n);
    struct bloom bf;


    FILE *fp;
    char *filename;
    int serverSocket;
    struct sockaddr_in clientAddr;
    int addr_len = sizeof(clientAddr);
    int client;
    char send_buf[BUFFER_SIZE] = {0}, recv_buf[BUFFER_SIZE] = {0};
    int iDataNum;
    
    serverSocket = init_Server(SERVER_PORT);
    if(serverSocket < 0) {
        printf("服务器初始化失败！\n");
        return 0;
    }
    printf("服务器初始化成功，监听端口：%d\n", SERVER_PORT);
    //等待用户连接
    client = accept(serverSocket, (struct sockaddr*)&clientAddr, (socklen_t*)&addr_len);
    if(client < 0) {
        printf("服务端accept失败，程序退出。\n");
        return 0;
    }
    
    printf("客户已连接：\n");
    printf("IP is %s\n", inet_ntoa(clientAddr.sin_addr));//inet_ntoa ip地址转换函数，将网络字节序IP转换为点分十进制IP
    printf("Port is %d\n", htons(clientAddr.sin_port));


    _server_base(&a, &b);
    printf("base up\n");
    submit_sel_File(client,"key-n.pub");
    submit_sel_File(client,"key-e.pub");

    _server_setup(&bf, &a, 1000, 0.001, "./resource/x.txt");
    printf("1\n");
    // send bf
    _bloom_write(&bf,"./resource/bloom");
    submit_sel_File(client,"bloom");

    // while(1){
    printf("等待消息...\n");
    //将客户端发送过来的消息写入文件
    filename = receive_File(client);
    printf("get lambda\n\n\n");
    _server_online(&a,1000,filename,"./resource/B.res");
    submit_sel_File(client,"B.res");
    printf("over\n");

    // }
    mpz_clears(a.n, a.d, b.e, b.n, NULL);
}
