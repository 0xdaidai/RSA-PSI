#ifndef SERVER_H
#define SERVER_H

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#define SERVER_PORT 6666
#define BUFFER_SIZE 1024
#define SERVER_PATH "./server_text/"

int init_Server(int port) {
  int serverSocket;
  struct sockaddr_in server_addr;
  //socket初始化
  if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    return -1;
  }
  //服务端地址类型，地址及端口初始化
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("connect");
    return -1;
  }
  //设置服务器上的socket为监听状态
  if(listen(serverSocket, 5) < 0) {
    perror("listen");
    return -1;
  }
  return serverSocket;
}

int get_File_Num(char* pathname) {
  struct dirent* ptr;
  DIR* path = NULL;
  path = opendir(pathname);
  int cnt = 0;
  while((ptr=readdir(path)) != NULL) {
    if(strcmp(ptr->d_name,".")==0||strcmp(ptr->d_name,"..")==0)
      continue;
    if(ptr->d_type==DT_REG)
      cnt++;
  }
  return cnt;
}

int receive_File(int client) {
  char recv_buf[BUFFER_SIZE] = {0};
  char filename[200] = {0}, pathname[200] = {0};
  int cnt;
  int totalBlock, lenBlock;
  recv(client, recv_buf, sizeof(recv_buf), 0);//先接收客户端要传多少个文件
  //printf("%d\n",(int)strlen(recv_buf));
  int filenum = atoi(recv_buf);
  printf("客户端要发送%d个文件\n",filenum);
  for(int i = 1; i <= filenum; i++) {
    //遍历指定目录下文件的个数，以确定新接收文件的名字
    cnt = get_File_Num((char*)SERVER_PATH);//得到服务器目录下的文件个数，方便编号命名
    FILE* fp;
    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%d", cnt+1);
    strcpy(pathname, SERVER_PATH);
    strcat(pathname,filename);
    printf("pathname: %s\n",pathname);
    fp = fopen(pathname, "wb");
    if(fp != NULL)
      printf("成功创建文件%s\n",pathname);
    recv(client,recv_buf,sizeof(recv_buf), 0);
    totalBlock = atoi(recv_buf);
    printf("第%d个文件有%d个块\n",i,totalBlock);
    for(int j = 1; j <= totalBlock; j++) {
      char temp[100] = {0};
      recv(client,temp,sizeof(temp),0);
      lenBlock = atoi(temp);
      printf("第%d个块长度为%d\n",j,lenBlock);
      if((recv(client,recv_buf,lenBlock ,0))!= -1)
			   printf("..........接收成功..........\n");
		  else
			   printf("接收失败!\n");
		  fwrite(recv_buf, 1, lenBlock, fp);
    }
    fclose(fp);
    char *p = (char*)"文件保存完毕";
    printf("%s\n",p);
  }
  close(client);
}

void KKL_Implement() {}

/*
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
*/
#endif
