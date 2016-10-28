#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<string.h>
#define BUF_SIZE 1024

int main()
{
    int sock;
    struct sockaddr_un addr;
    int str_len;
    char buf[BUF_SIZE];
    char* path="sock";
    sock=socket(AF_UNIX,SOCK_STREAM,0);

    addr.sun_family=AF_UNIX;
    strncpy(addr.sun_path,path,strlen(path));
    
    connect(sock,(struct sockadd*)&addr,sizeof(addr));

    while(1)
    {
        fputs("input:",stdout);
        fgets(buf,BUF_SIZE,stdin);
        if(!strcmp(buf,"q\n")||!strcmp(buf,"Q\n"))
            break;
        write(sock,buf,BUF_SIZE);
        str_len=read(sock,buf,BUF_SIZE-1);
        buf[str_len]=0;
        printf("the message from serv:%s\n",buf);

    }
    close(sock);
    return 0;
}
