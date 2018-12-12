#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
 
typedef struct sockaddr *sockaddrp;
  
//存儲客戶端地址的結構體數組
struct sockaddr_in src_addr[50];
socklen_t src_len = sizeof(src_addr[0]);

 
 
//連接後記錄confd數組
int confd[50] = {};
 

//設置連接人數
int count = 0;
char namearr[50][50]={};

 
void *broadcast(void *indexp)
{
    int index = *(int *)indexp;
    char buf_rcv[255] = {};
    char buf_snd[255] = {};
    char buf_dir1[255]={};
    char buf_dir2[255]={};
    char filename[255]={};
    //第一次讀取用戶姓名
    char name[20] = {};
    int who=0,f3=0,f4=1;
    int ret = recv(confd[index],name,sizeof(name),0);
    if(0 > ret)
    {
        perror("recv");
        close(confd[index]);
        return NULL;
    }
    strcpy(namearr[index],name);
//   printf("name:%s\n",namearr[index]);
 
    while(1)
    {
        bzero(buf_rcv,sizeof(buf_rcv));
        bzero(buf_snd,sizeof(buf_snd));
        recv(confd[index],buf_rcv,sizeof(buf_rcv),0);

         //判斷是否退出
         if(0 == strcmp("q",buf_rcv))
         {
            sprintf(buf_snd,"%s已經退出聊天室",name);
            for(int i = 0;i <= count;i++)
            {
                if(i == index || 0 == confd[i])
                {
                    continue;
                }
 
                send(confd[i],buf_snd,strlen(buf_snd),0);
            }
            confd[index] = -1;
            pthread_exit(0);                  
         }
//	 else if(strcmp("y",buf_rcv)==0||strcmp("n",buf_rcv)==0)
//		 continue;
	 else if(strcmp("f1",buf_rcv)==0)
	{
		for(int i=0;i<=count;i++)
		{
			if(i==index||0==confd[i]||confd[i]==-1)
			{
				continue;
			}
			sprintf(buf_snd,"%s",namearr[i]);
//			sprintf(buf_snd,"ggg");
//			printf("buf_snd:%s",buf_snd);
			send(confd[index],buf_snd,strlen(buf_snd),0);
		}
		continue;
	}
	 else if(strcmp("f3",buf_rcv)==0||strcmp("f4",buf_rcv)==0)
	{
		if(strcmp("f3",buf_rcv)==0)
			f3=1;
		else
			f3=0;
		sprintf(buf_snd,"who u want to send");
		send(confd[index],buf_snd,strlen(buf_snd),0);

        	bzero(buf_rcv,sizeof(buf_rcv));
        	bzero(buf_snd,sizeof(buf_snd));
        	recv(confd[index],buf_rcv,sizeof(buf_rcv),0);

		int j=0;
		for(j=0;j<=count;++j)
		{
			if(strcmp(namearr[j],buf_rcv)==0)
			{
				who=j;
				break;
			}
		}
		if(j>count)
		{
			sprintf(buf_snd,"unexist");
			send(confd[index],buf_snd,strlen(buf_snd),0);
		}
		else
		{
			if(f3==1)
			{
				sprintf(buf_snd,"send %s:",namearr[who]);
				send(confd[index],buf_snd,strlen(buf_snd),0);

        			bzero(buf_rcv,sizeof(buf_rcv));
        			bzero(buf_snd,sizeof(buf_snd));
        			recv(confd[index],buf_rcv,sizeof(buf_rcv),0);

				sprintf(buf_snd,"%s:%s",name,buf_rcv);
                		send(confd[who],buf_snd,strlen(buf_snd),0);
			}
			else if(f3==0)
			{
				sprintf(buf_snd,"%s want to send a file\ny or n:",name);
				send(confd[who],buf_snd,strlen(buf_snd),0);

        			bzero(buf_rcv,sizeof(buf_rcv));
        			bzero(buf_snd,sizeof(buf_snd));
        			recv(confd[who],buf_rcv,sizeof(buf_rcv),0);

				if(strcmp("y",buf_rcv)==0)
				{

        				bzero(buf_dir1,sizeof(buf_dir1));
					sprintf(buf_snd,"getdir");
					send(confd[index],buf_snd,strlen(buf_snd),0);
					recv(confd[index],buf_dir1,sizeof(buf_dir1),0);
					chdir(buf_dir1);
					//printf("%s\n",buf_dir1);

					bzero(buf_snd,sizeof(buf_snd));
					bzero(filename,sizeof(filename));
					sprintf(buf_snd,"send %s a file:",namearr[who]);
					send(confd[index],buf_snd,strlen(buf_snd),0);
        				recv(confd[index],filename,sizeof(filename),0);

					FILE *fp1=fopen(filename,"r");
					char file[255][255]={};
					int line=0;
					while(fgets(file[line],255,fp1)!=NULL)
					{
						++line;
					}
					fclose(fp1);
        				bzero(buf_dir2,sizeof(buf_dir2));
        				bzero(buf_rcv,sizeof(buf_rcv));
        				bzero(buf_snd,sizeof(buf_snd));
					sprintf(buf_snd,"getdir");
					send(confd[who],buf_snd,strlen(buf_snd),0);
					recv(confd[who],buf_dir2,sizeof(buf_dir2),0);

					chdir(buf_dir2);
					FILE *fp2=fopen(filename,"w");
					for(int i=0;i<line;++i)
					{
						fwrite(file[i],1,strlen(file[i]),fp2);
					}
//					fflush(fp2);
					fclose(fp2);
					bzero(buf_snd,sizeof(buf_snd));
					sprintf(buf_snd,"recieve %s",filename);
					send(confd[who],buf_snd,strlen(buf_snd),0);
				}
				else
				{
					sprintf(buf_snd,"%s do not accept",namearr[who]);
					send(confd[index],buf_snd,strlen(buf_snd),0);
				}
			}
		}
	} 
	else
	{
        	sprintf(buf_snd,"%s:%s",name,buf_rcv);	//tell other clients
        	printf("%s\n",buf_snd);
        	for(int i = 0;i <= count;i++)
        	{
            		if(i == index || 0 == confd[i])
            		{
                		continue;
            		}
            		send(confd[i],buf_snd,sizeof(buf_snd),0);
       	 	}
	}
    }
}

int main(int argc,char **argv)
{
   printf("聊天服務器端開始運行\n");
   //創建通信對象
   int sockfd = socket(AF_INET,SOCK_STREAM,0);
   if(0 > sockfd)
   {
       perror("socket");
       return -1;
   }
    //準備地址
    struct sockaddr_in addr = {AF_INET};
     addr.sin_port = htons(atoi("8080"));
     addr.sin_addr.s_addr = inet_addr("127.0.0.1");
     socklen_t addr_len = sizeof(addr);
 
     //綁定
     int ret = bind(sockfd,(sockaddrp)&addr,addr_len);
     if(0 > ret)
     {
         perror("bind");
         return -1;
     }
     //設置最大排隊數
     listen(sockfd,50);
     int index = 0;

     while(count <= 50)
     {
        confd[count] = accept(sockfd,(sockaddrp)&src_addr[count],&src_len);
//	printf("count:%d\n",count);
	++count;
        //保存此次客戶端地址所在下標方便後續傳入
         index = count-1;
 
         pthread_t tid;
         int ret = pthread_create(&tid,NULL,broadcast,&index);
         if(0 > ret)
        {
             perror("pthread_create");
             return -1;
         }
     }
}
