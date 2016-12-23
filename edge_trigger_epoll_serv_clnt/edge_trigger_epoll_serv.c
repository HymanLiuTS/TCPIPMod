#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<errno.h>
#include<fcntl.h>

#define BUF_SIZE 30
#define EPOLL_SIZE 20

void error_handler(const char* message);

int main(int argc,char* argv[])
{
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_addr,clnt_addr;
    int clnt_addr_sz;

    char buf[BUF_SIZE];
    int str_len;

    int ep_fd,ep_cnt,i,flag;
    struct epoll_event event;
    struct epoll_event* pevents;
    
    if(argc!=2)
    {
        printf("Usage %s <port>\n",argv[0]);
        exit(1);
    }

    serv_sock=socket(AF_INET,SOCK_STREAM,0);

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
        error_handler("bind() error");

    if(listen(serv_sock,5)==-1)
        error_handler("listen() error");

    ep_fd=epoll_create(EPOLL_SIZE);
    event.events=EPOLLIN;
    event.data.fd=serv_sock;
    pevents=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);
    epoll_ctl(ep_fd,EPOLL_CTL_ADD,serv_sock,&event);

    while(1)
    {
        ep_cnt=epoll_wait(ep_fd,pevents,EPOLL_SIZE,-1);
        for(i=0;i<ep_cnt;i++)
        {
            if(serv_sock==pevents[i].data.fd)
            {
                clnt_addr_sz=sizeof(clnt_addr);
                clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_sz);
                //将套接字设置成非阻塞模式
                flag=fcntl(clnt_sock,F_GETFL,0);
                fcntl(clnt_sock,F_SETFL,flag|O_NONBLOCK);

                event.events=EPOLLIN|EPOLLET;
                event.data.fd=clnt_sock;
                epoll_ctl(ep_fd,EPOLL_CTL_ADD,clnt_sock,&event);
            }
            else
            {
                while(1)
                {
                    str_len=read(pevents[i].data.fd,buf,BUF_SIZE);
                    if(str_len==0)
                    {
                        epoll_ctl(ep_fd,EPOLL_CTL_DEL,pevents[i].data.fd,NULL);
                        close(pevents[i].data.fd);
                        break;
                    }
                    else if(str_len<0)
                    {
                        if(errno==EAGAIN)//标准输入里没有数据
                            break;
                    }
                    else
                    {
                        write(pevents[i].data.fd,buf,str_len);
                    }

                }
            }
        }
    }

    close(serv_sock);
    close(ep_fd);

    return 0;
}
void error_handler(const char* message)
{
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}
