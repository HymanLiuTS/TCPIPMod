#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define BUF_SIZE 100 

void error_handling(char* message);

int main(int argc,char* argv[])
{
	int serv_sock,clnt_sock;
	struct sockaddr_in serv_addr,clnt_addr;
	int clnt_addr_sz;
	int str_len;
	char buf[BUF_SIZE];

	if(argc!=2)
	{
		printf("Usage %s <port>\n",argv[0]);
		exit(1);
	}

	//创建socket
	serv_sock=socket(AF_INET,SOCK_STREAM,0);
	if(serv_sock==-1)
		error_handling("socket error");

	//填充地址信息
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	//socket和ip地址的绑定
	if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
		error_handling("bind error");

	//开启监听
	if(listen(serv_sock,5)==-1)
		error_handling(" listen error");

	//接受数据
	while(1)
	{
		clnt_addr_sz=sizeof(clnt_addr);
		clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_sz);
		if(clnt_sock==-1)
			error_handling("accept error");
		
		str_len=read(clnt_sock,buf,BUF_SIZE);
		write(clnt_sock,buf,str_len);
	}

	return 0;
}

void error_handling(char* message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}

