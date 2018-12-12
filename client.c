#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include<stdlib.h>
typedef struct sockaddr *sockaddrp;
int sockfd;

void *recv_other(void *arg)
{
     char buf[255]= {};
     while(1)
     {
	bzero(buf,sizeof(buf));
        int ret = recv(sockfd,buf,sizeof(buf),0);
         if(0 > ret)
         {
             perror("recv");
             return NULL;
         }
	 if(strcmp("getdir",buf)==0)
	 {
		 getcwd(buf,255);
		 //printf("%s\n",buf);
    		 send(sockfd,buf,strlen(buf),0);
	 }
	 else
         	printf("%s\n",buf);
     }
}

int main(int argc,char **argv)
{
    
    //建立socket對象
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(0 > sockfd)
    {
        perror("socket");
        return -1;
    }
 //   printf("%d\n",sockfd);
 
    //準備連接地址
    struct sockaddr_in addr = {AF_INET};
    addr.sin_port = htons(atoi("8080"));
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t addr_len = sizeof(addr);

    //連接
    int ret = connect(sockfd,(sockaddrp)&addr,addr_len);
    if(0 > ret)
    {
        perror("connect");
        return -1;
    }

    //發送名字
    char buf[255] = {};
    char name[255] = {};
    printf("請輸入您的昵稱：");
//    scanf("%s",name);
   fgets(name,255,stdin);
   name[strlen(name)-1]='\0';
//    printf("name:%s\n",name);
    ret = send(sockfd,name,strlen(name),0);
    if(0 > ret)
    {
        perror("connect");
        return -1;
    }

     //創建接收子線程
    pthread_t tid;
    ret = pthread_create(&tid,NULL,recv_other,NULL);
 
    if(0 > ret)
    {
        perror("pthread_create");
        return -1;
    }
    //循環發送
    int i=0; 
    while(1)
    {
//    	printf("in:");	    
//      scanf("%s",buf);
//      printf("buf:%s\n",buf);
 	fgets(buf,255,stdin);
//	printf("buf1:%s\n",buf);
	buf[strlen(buf)-1]='\0';
//	printf("buf2:%s\n",buf);
     	int ret = send(sockfd,buf,strlen(buf),0);
        if(0 > ret)
        {
            perror("send");
            return -1;
        }
 
        //輸入quit退出
        if(0 == strcmp("q",buf))
         {
            printf("%s,您已經退出了聊天室\n",name);
             return 0;
         }
 
    }
 
}
