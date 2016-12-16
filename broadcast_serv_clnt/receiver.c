#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#define BUF_SIZE 30

void error_handling(const char* message);

int main(int argc,char* argv[])
{
    int sock;
    struct sockaddr_in addr;
    char message[BUF_SIZE];
    int str_len=0;

    sock=socket(AF_INET,SOCK_DGRAM,0);
    if(sock==-1)
        error_handling("socket() error");

    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(atoi(argv[1]));

    if(bind(sock,(struct sockaddr*)&addr,sizeof(addr))==-1)
            error_handling("bind() error");

    while(1)
    {
        str_len=recvfrom(sock,message,BUF_SIZE-1,0,NULL,NULL);
        if(str_len<=0)
            break;
        message[str_len]=0;
        fputs(message,stdout);
    }
    close(sock);
    return 0;
}

void error_handling(const char* message)
{
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}
