/*************************************************************************
 > File Name: file.h
 > Author: Guaji
 > Mail: rdwentao@163.com 
 > Created Time: Sat 13 Jun 2020 06:08:42 PM CST
 ************************************************************************/
#ifndef FILE_H
#define FILE_H

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
#define CLIENT_PATH "./resource/"
#define SERVER_PATH "./resource/"

int connect_to_Server(char* serverIP, int port) {
  struct sockaddr_in serverAddr;
  int clientSocket;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  serverAddr.sin_addr.s_addr = inet_addr(serverIP);
  if((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    printf("sock生成出错！\n");
    return -1;
  }
  if(connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
    printf("connect失败！\n");
    return -1;
  }
  return clientSocket;
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

int pack_send_File(int clientSocket, char name[]) {
  char send_buf[BUFFER_SIZE] = {0};
  char* pre_filename = (char*)malloc(sizeof(char)*100);
  strcpy(pre_filename,CLIENT_PATH);
  strcat(pre_filename,name);
  FILE *fp = fopen(pre_filename,"rb");
  strcpy(send_buf,name);
  send(clientSocket,(char*)send_buf,BUFFER_SIZE,0);
  printf("send file name: %s\n",send_buf);
  memset(send_buf,0,BUFFER_SIZE);
  int len_file, len_block;
  int totalBlock;
  fseek(fp,0,2);//将指针放到文件末尾
  len_file = ftell(fp); //统计文本的数据长度
  rewind(fp); //让指针指向文件开始位置
  printf("文件的总长度len_file:%d\n",len_file);
  totalBlock = len_file % BUFFER_SIZE == 0 ? len_file / BUFFER_SIZE : ((len_file / BUFFER_SIZE) + 1); //除最后一快的总块数
  printf("文件的总块数totalBlock:%d\n",totalBlock);
  sprintf(send_buf, "%d", totalBlock);
  send(clientSocket,(char*)send_buf,BUFFER_SIZE,0);

  for(int i = 1; i <= totalBlock; i++) {
    memset(send_buf,0,BUFFER_SIZE);
    len_block = fread(send_buf, 1, BUFFER_SIZE, fp);
    send_buf[len_block]='\0';
    printf("文件第%d块的长度len_block:%d\n",i,len_block);
    send(clientSocket,send_buf,BUFFER_SIZE,0);
  }
  fclose(fp);
  return 1;
}


int submit_sel_File(int clientSocket,char filename[]) {//提交单个文件
  //将文件内容发送给服务端
  char send_buf[BUFFER_SIZE] = {0};
  memset(send_buf,0,1024);
  strcpy(send_buf,"1");
  send(clientSocket,(char*)send_buf,1024,0);

  // printf("已找到您的文件:路径是%s\n",pre_filename);
  pack_send_File(clientSocket, filename);
  printf("文件上传成功！\n");
  return 1;
}


int submit_Files(int clientSocket,char send_path[]) {//提交多个文件，想法是提交某个文件夹下的所有文件
  char send_buf[BUFFER_SIZE] = {0};
  memset(send_buf,0,1024);
  int filenum = get_File_Num((char*)send_path);
  sprintf(send_buf, "%d", filenum);
  printf("send %d files\n", filenum);
  send(clientSocket, send_buf, sizeof(send_buf), 0);
  char pre_filename[100] = {0};
  struct dirent* ptr;
  DIR* path = NULL;
  path = opendir(send_path);
  while((ptr=readdir(path)) != NULL) {
    if(strcmp(ptr->d_name,".")==0||strcmp(ptr->d_name,"..")==0) {
      continue;
    }
    if(ptr->d_type==DT_REG) {
      strcpy(pre_filename,send_path);
      strcat(pre_filename,ptr->d_name);
      pack_send_File(clientSocket, ptr->d_name);
      printf("文件%s上传成功！\n",ptr->d_name);
    }
  }
  return 1;
}

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

char *receive_File(int client) {
  char filename[200] = {0};
  char recv_buf[BUFFER_SIZE] = {0};
  char *pathname =(char *)malloc(sizeof(char) * 200);
  memset(pathname,0,200);
  int cnt;
  int totalBlock, lenBlock;
  recv(client, recv_buf, BUFFER_SIZE, 0);//先接收客户端要传多少个文件
  int filenum = atoi(recv_buf);
  printf("客户端要发送%s个文件\n",recv_buf);
  for(int i = 1; i <= filenum; i++) {
    recv(client,recv_buf,BUFFER_SIZE, 0);//get file name
    printf("recv_file_name: %s\n", recv_buf);
    FILE* fp;
    strcpy(filename, recv_buf);
    strcpy(pathname, SERVER_PATH);
    strcat(pathname,filename);
    printf("pathname: %s\n",pathname);
    fp = fopen(pathname, "wb");
    if(fp != NULL)
      printf("成功创建文件%s\n",pathname);
    memset(recv_buf,0,BUFFER_SIZE);

    recv(client,recv_buf,BUFFER_SIZE, 0);
    printf("total:%s\n",recv_buf);
    totalBlock = atoi(recv_buf);
    printf("第%d个文件有%d个块\n",i,totalBlock);
    for(int j = 1; j <= totalBlock; j++) {
      memset(recv_buf,0,BUFFER_SIZE);
      lenBlock = 1024;
      printf("第%d个块长度为%d\n",j,lenBlock);
      if((recv(client,recv_buf,lenBlock ,0))!= -1)
	printf("..........接收成功..........\n");
      else
	printf("接收失败!\n");
      printf("recvbuf:%s\n",recv_buf );
      fwrite(recv_buf, 1, lenBlock, fp);
    }
    fclose(fp);
    char *p = (char*)"文件保存完毕";
    printf("%s\n",p);
  }
  // close(client);
  return pathname;
}

#endif
