#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<signal.h>
#define BUF_SIZE 1024

void error_handling(char* message);

void child_handling(int sig)
{
	pid_t pid;
	int status;
	if(sig==SIGCHLD)
	{
		pid=waitpid(-1,&status,WNOHANG);
		if(WIFEXITED(status))
		{
			printf("child process %d exit.\n",pid);
			//printf("child process %d exit.\n",WEXITSTATUS(status));
		}
	}
}

int main(int argc,char* argv[])
{
	int serv_sock,clnt_sock;
	struct sockaddr_in serv_addr,clnt_addr;
	int clnt_addr_sz;
	char buf[BUF_SIZE];
	int str_len,state;
	pid_t pid;
	struct sigaction sig;
	
	if(argc!=2)
	{
		printf("Usage %s <port>\n",argv[0]);
		exit(1);
	}

	serv_sock=socket(AF_INET,SOCK_STREAM,0);
	if(serv_sock==-1)
		error_handling("socket error");

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
		error_handling("bind() error");

	if(listen(serv_sock,5)==-1)
		error_handling("listen() error");

	sig.sa_handler=child_handling;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags=0;
	state=sigaction(SIGCHLD,&sig,0);
	while(1)
	{
		clnt_addr_sz=sizeof(clnt_addr);
		clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_sz);
		if(clnt_sock==-1)
			continue;
		else
			puts("new client connected...");
		pid = fork();
		if(pid==-1)
		{
			close(clnt_sock);
			continue;
		}
		if(pid==0)
		{
			while(1)
			{
				str_len=read(clnt_sock,buf,sizeof(buf));
				if(str_len<=0)
					break;
				write(clnt_sock,buf,str_len);
			}
			close(clnt_sock);
			puts("client disconnected...");
			return 0;
		}
		else
			close(clnt_sock);
	}
	close(serv_sock);
	return 0;
}


void error_handling(char* message)
{
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}