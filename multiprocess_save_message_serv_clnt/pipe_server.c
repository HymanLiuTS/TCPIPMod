#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#define BUF_SIZE 1024


void error_handling(const char* message);

int main(int argc,char* argv[])
{
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_addr,clnt_addr;
    int clnt_addr_sz,str_len;
    char buf[BUF_SIZE];
    pid_t pid;
    int fds[2];
    FILE* pf;

    if(argc!=2)
    {
        printf("Usage %s <port>",argv[0]);
        exit(1);
    }

    serv_sock=socket(AF_INET,SOCK_STREAM,0);
    if(serv_sock==-1)
    {
        error_handling("socket() error");
    }

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
        error_handling("bind() error");

    if(listen(serv_sock,5)==-1)
        error_handling("listen() error");
    pipe(fds);
    pid = fork();
    if(pid==0)
    {
        close(serv_sock);
        pf=fopen("logs.txt","a+");
        if(pf==NULL) return 0;
        while(1)
        {
            memset(buf,0,BUF_SIZE);
            str_len=read(fds[0],buf,BUF_SIZE);
            if(!strcmp(buf,"q\n"))
            {
                puts("Client disconnected...");
                fclose(pf);
                break;
            }
            fwrite(buf,sizeof(char),str_len,pf);    
        }
    }
    else
    {
        while(1)
        {
            clnt_addr_sz=sizeof(clnt_addr);
            clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_sz);
            if(clnt_sock==-1)
            {
                error_handling("accept() error");
            }
            else
            {
                puts("Client connect succeed!");
            }

            pid=fork();

            if(pid==0)
            {
                while(1)
                {
                    str_len = read(clnt_sock,buf,BUF_SIZE);
                    if(str_len<=0)
                    {
                        write(fds[1],"q\n",2);
                        close(clnt_sock);
                        break;
                    }
                    write(clnt_sock,buf,str_len);
                    write(fds[1],buf,str_len);
                }
            }
            else
            {
                close(clnt_sock);
            }
         }
    }
    close(serv_sock);
    return 0;
}

void error_handling(const char* message)
{
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

