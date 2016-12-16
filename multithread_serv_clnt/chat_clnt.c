#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<pthread.h>
#define BUF_SIZE 30
void read_handling(void* param)
{
   char buff[BUF_SIZE];
   int str_len;
   int sock=*((int*)param);
   while(1)
   {
     str_len = read(sock,buff,BUF_SIZE-1);
     if(str_len<=0)
            break;
     buff[str_len]=0;
     printf("the message from server:%s\n",buff);
   }
}

void write_handling(void* param)
{
   char buf[BUF_SIZE];
   int sock=*((int*)param);
   while(1)
   {
        fputs("input:",stdout);
        fgets(buf,BUF_SIZE,stdin);
        if(!strcmp(buf,"q\n")||
           !strcmp(buf,"Q\n"))
                exit(1);
        write(sock,buf,strlen(buf));
   }
}


int main(int argc,char* argv[])
{
    int sock;
    struct sockaddr_in addr;
    sock=socket(AF_INET,SOCK_STREAM,0);
    pthread_t tid1,tid2;
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(argv[1]);
    addr.sin_port=htons(atoi(argv[2]));
    
    connect(sock,(struct sockaddr*)&addr,sizeof(addr));
    pthread_create(&tid1,NULL,(void*)write_handling,&sock);
    pthread_create(&tid2,NULL,(void*)read_handling,&sock);
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    close(sock);
    return 0;
}
