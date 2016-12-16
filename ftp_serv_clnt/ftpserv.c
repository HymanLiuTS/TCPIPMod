#include<stdio.h>

#include<stdlib.h>

#include<string.h>

#include<sys/socket.h>

#include<arpa/inet.h>



#define BUF_SIZE 1024





void error_handling(char*  message);

int main(int argc,char* argv[])

{

	int serv_sock,clnt_sock;

	struct sockaddr_in serv_addr,clnt_addr;

	int clnt_addr_sz;

    FILE* fp;

	char buf[1024];

	int str_len;



	if(argc!=2)

	{

		printf("Usage %s <port>\n",argv[0]);

		exit(1);

	}



	serv_sock=socket(AF_INET,SOCK_STREAM,0);

	if(serv_sock==-1)

		error_handling("socket() error");



	memset(&serv_addr,0,sizeof(serv_addr));

	serv_addr.sin_family=AF_INET;

	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);

	serv_addr.sin_port=htons(atoi(argv[1]));



	if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)

		error_handling("bind() error");



	if(listen(serv_sock,5)==-1)

		error_handling("listen() error");



	clnt_addr_sz=sizeof(clnt_addr);

	if((clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_sz))==-1)

		error_handling("accept() error");

	

	fp=fopen("1.txt","rb");

	if(fp!=NULL)

	{

		while((str_len=fread(buf,sizeof(char),BUF_SIZE,fp))>0)

		{

			write(clnt_sock,buf,str_len);

		}		

		shutdown(clnt_sock,SHUT_WR);

		memset(buf,0,sizeof(buf));

		read(clnt_sock,buf,BUF_SIZE);

		printf("The message from Client:%s\n",buf);

		fclose(fp);

	}

	close(clnt_sock);

	close(serv_sock);

	return 0;

}



void error_handling(char* message)

{

	fputs(message,stderr);

	fputc('\n',stderr);

	exit(1);

}