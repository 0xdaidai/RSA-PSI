#ifndef CLIENT_H
#define CLIENT_H

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
#define CLIENT_PATH "./client_text/"

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

int pack_send_File(int clientSocket, FILE* fp) {
  char send_buf[BUFFER_SIZE] = {0};
  int len_file, len_block;
  int totalBlock;
  fseek(fp,0,2);//将指针放到文件末尾
  len_file = ftell(fp); //统计文本的数据长度
  rewind(fp); //让指针指向文件开始位置
  printf("文件的总长度len_file:%d\n",len_file);
  totalBlock = len_file % BUFFER_SIZE == 0 ? len_file / BUFFER_SIZE : ((len_file / BUFFER_SIZE) + 1); //除最后一快的总块数
  printf("文件的总块数totalBlock:%d\n",totalBlock);
  sprintf(send_buf, "%d", totalBlock);
  send(clientSocket,(char*)send_buf,sizeof(send_buf),0);
  for(int i = 1; i <= totalBlock; i++) {
    len_block = fread(send_buf, 1, BUFFER_SIZE, fp);
    send_buf[len_block]='\0';
    char temp[100] = {0};
    sprintf(temp, "%d", len_block);
    printf("文件第%d块的长度len_block:%s\n",i,temp);
    send(clientSocket,(char*)temp,sizeof(temp),0);
    printf("第%d块的内容:%s\n",i,send_buf);
    send(clientSocket,send_buf,len_block,0);
  }
  fclose(fp);
}

int submit_File(int clientSocket) {//提交单个文件
  //将文件内容发送给服务端
  FILE *fp;
  char send_buf[BUFFER_SIZE] = {0};
  char filename[100] = {0};
  char* pre_filename = (char*)malloc(sizeof(char)*100);
  while(1){
    strcpy(pre_filename,CLIENT_PATH);
    printf("请输入你要提交的文件名: ");
    scanf("%[^\n]%*c",filename);
    strcat(pre_filename,filename);
    fp = fopen(pre_filename, "rb");
    if(fp == NULL) {
      printf("您提供的文件不存在，请重新输入！\n");
      continue;
    }
    break;
  }
  printf("已找到您的文件:路径是%s\n",pre_filename);
  pack_send_File(clientSocket, fp);
  printf("文件上传成功！\n");
  close(clientSocket);
}

int submit_Files(int clientSocket) {//提交多个文件，想法是提交某个文件夹下的所有文件
  FILE* fp;
  char filename[100] = {0}, pre_filename[100] = {0};
  struct dirent* ptr;
  DIR* path = NULL;
  char send_buf[BUFFER_SIZE] = {0};
  path = opendir((char*)CLIENT_PATH);
  while((ptr=readdir(path)) != NULL) {
    if(strcmp(ptr->d_name,".")==0||strcmp(ptr->d_name,"..")==0) {
      continue;
    }
    if(ptr->d_type==DT_REG) {
      printf("%s\n",ptr->d_name);
      strcpy(pre_filename,CLIENT_PATH);
      strcat(pre_filename,ptr->d_name);
      fp = fopen(pre_filename, "rb");
      pack_send_File(clientSocket, fp);
      printf("文件%s上传成功！\n",ptr->d_name);
    }
  }
}

/*
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
}*/

#endif
