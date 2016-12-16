#include<stdio.h>

#include<stdlib.h>

#include<string.h>

#include<sys/socket.h>

#include<arpa/inet.h>

#define BUF_SIZE 100



void error_handling(const char* message);

int main(int argc,char* argv[])

{

	int sock;

	struct sockaddr_in serv_addr;

	int str_len;

	char buf[BUF_SIZE];

	int recv_len=0;
        
	if(argc!=3)
	{
		printf("Usage %s <address> <port>\n",argv[0]);
		exit(1);
	}


	//创建socket

	sock=socket(AF_INET,SOCK_STREAM,0);

	if(sock==-1)

		error_handling("socket error");

	

	//准备地址

	memset(&serv_addr,0,sizeof(serv_addr));

	serv_addr.sin_family=AF_INET;

	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);

	serv_addr.sin_port=htons(atoi(argv[2]));



	

	//链接

	if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)

		error_handling("connect error");

	

	while(1)

	{

		memset(buf,0,BUF_SIZE);

		fputs("请输入数据:",stdout);

		fgets(buf,BUF_SIZE,stdin);

		if(!strcmp(buf,"q\n")||!strcmp(buf,"Q/n"))

			break;

		str_len=write(sock,buf,strlen(buf));



		while(recv_len<str_len)

			recv_len+=read(sock,buf,BUF_SIZE-1);

		buf[str_len]=0;

		printf("服务器传回信息:%s\n",buf);

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
