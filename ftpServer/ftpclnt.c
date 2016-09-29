#include<stdio.h>

#include<stdlib.h>

#include<string.h>

#include<sys/socket.h>

#include<arpa/inet.h>

#define BUF_SIZE 1024

void error_handling(char* message);



int main(int argc,char* argv[])

{

	int sock;

	struct sockaddr_in addr;

	FILE* fp;

	char buf[BUF_SIZE];

	int str_len;

	if(argc!=3)

	{

		printf("Usage %s <address> <port>\n",argv[0]);

		exit(1);

	}



	sock=socket(AF_INET,SOCK_STREAM,0);

	

	memset(&addr,0,sizeof(addr));

	addr.sin_family=AF_INET;

	addr.sin_addr.s_addr=inet_addr(argv[1]);

	addr.sin_port=htons(atoi(argv[2]));



	if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))==-1)

		error_handling("connect() error");



	fp=fopen("1copy.txt","w+");

	if(fp!=NULL)

	{

		while((str_len=read(sock,buf,BUF_SIZE))>0)

		{

			fwrite(buf,sizeof(char),str_len,fp);

		}

		write(sock,"Thank You!",11);

		fclose(fp);

	}

	close(sock);

	return 0;

}



void error_handling(char* message)

{

	fputs(message,stderr);

	fputc('\n',stderr);

	exit(1);

}