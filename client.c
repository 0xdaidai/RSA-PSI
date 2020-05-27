/*************************************************************************
 > File Name: client.c
 > Author: Guaji
 > Mail: rdwentao@163.com 
 > Created Time: Wed 13 May 2020 05:39:41 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "elements.h"

struct data data;

int main(int argc,char *argv[])
{
    int sockfd,numbytes;
    char buf[BUFSIZ];
    struct sockaddr_in their_addr;
    memset(&their_addr,0,sizeof(struct sockaddr_in));
    printf("break!");
    while((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1);
    printf("We get the sockfd~\n");
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(12345);
    their_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    
    while(connect(sockfd,(struct sockaddr*)&their_addr,sizeof(struct sockaddr)) == -1);
    printf("Get the Server~Cheers!\n");

    while((numbytes = recv(sockfd, buf, BUFSIZ,0))<sizeof(data.head));//接收服务器端信息  
    buf[numbytes]='\0';  
    printf("%s",buf);
    printf("begin\n");
    numbytes=recv(sockfd,buf,BUFSIZ,0);  
    memcpy(&data, buf, numbytes);
    printf("%s\n",  data.msg );

    // memset(&data, 0, sizeof(data));
    // memset(buf, 0, BUFSIZ);
    printf("now bufsize is %ld\n",  sizeof(buf) );
    while((numbytes = recv(sockfd, buf, BUFSIZ,0))<sizeof(data.head));//接收服务器端信息  
    memcpy(&data, buf, numbytes);
    printf("%s\n",  data.msg );
    return 0;
}
