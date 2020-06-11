/*************************************************************************
 > File Name: server_test.c
 > Author: Guaji
 > Mail: rdwentao@163.com 
 > Created Time: Fri 12 Jun 2020 03:13:42 AM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include "server.h"

int main() {
  FILE *fp;
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
  printf("等待消息...\n");
  //将客户端发送过来的消息写入文件
  receive_File(client);
  KKL_Implement();//实现服务器端KKL协议的接口
  close(serverSocket);
  return 0;
}
