#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<pthread.h>
#define BUF_SIZE 30
#define CLNT_SIZE 30

int clnt_socks[CLNT_SIZE];
int clntcount=0;
pthread_mutex_t mt;

void* clnt_handling(void* param)
{
    char buf[BUF_SIZE];
    int i,str_len;
    int clnt_sock=*((int*)param);
    while(1)
    {
        str_len=read(clnt_sock,buf,BUF_SIZE-1);
        if(str_len<=0)
            break;
        buf[str_len]=0;
        write(clnt_sock,buf,BUF_SIZE);
    }
    
    close(clnt_sock);
    pthread_mutex_lock(&mt);
    for(i=0;i<CLNT_SIZE-1;i++)
    {
        if(clnt_socks[i]==0) break;
        if(clnt_socks[i]==clnt_sock)
        {
            clnt_socks[i]=clnt_socks[i+1];
            clnt_socks[i+1]=0;
        }
    }
    pthread_mutex_unlock(&mt);
}


int main(int argc,char* argv[])
{
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_addr,clnt_addr;
    int clnt_addr_sz;
    pthread_t tid;

    if(argc!=2)
    {
        printf("Usage %s <port>\n",argv[0]);
        exit(1);
    }
    pthread_mutex_init(&mt,NULL);
    serv_sock=socket(AF_INET,SOCK_STREAM,0);

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    
    listen(serv_sock,5);
    while(1)
    {
        clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_sz);
        pthread_mutex_lock(&mt);
        clnt_socks[clntcount++]=clnt_sock;
        pthread_mutex_unlock(&mt);

        pthread_create(&tid,NULL,clnt_handling,(void*)&clnt_sock);
        pthread_detach(tid);
        printf("%s Connected\n",inet_ntoa(clnt_addr.sin_addr));
    }
    close(serv_sock);
    pthread_mutext_destroy(&mt);
    return 0;
}
