/*************************************************************************
 > File Name: client_test.c
 > Author: Guaji
 > Mail: rdwentao@163.com 
 > Created Time: Fri 12 Jun 2020 03:15:11 AM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include "client.h"

int main() {
  int choice;//选择上传单个文件还是多个
  char send_buf[BUFFER_SIZE] = {0};
  int clientSocket = connect_to_Server((char*)"127.0.0.1",SERVER_PORT);
  if(clientSocket == -1) return 0;
  printf("已连接到服务器...\n");
  printf("请选择上传单个文件还是多个文件，单个文件输入0,多个文件输入1：");
  while(1) {
    scanf("%d",&choice);
    getchar();
    if(choice == 0 || choice == 1)
      break;
    else
      printf("请正确输入0或1");
  }
  if(choice == 0) {
    strcpy(send_buf, "1");
    send(clientSocket, send_buf, sizeof(send_buf), 0);
    submit_File(clientSocket);
  } else {
    int filenum = get_File_Num((char*)CLIENT_PATH);
    sprintf(send_buf, "%d", filenum);
    send(clientSocket, send_buf, sizeof(send_buf), 0);
    submit_Files(clientSocket);
  }
  //shutdown(clientSocket, SHUT_WR);
  //recv(clientSocket, recv_buf, strlen(recv_buf), MSG_DONTWAIT);
  //printf("%s\n",recv_buf);
  return 0;
}
