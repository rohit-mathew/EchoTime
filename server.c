#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>

/*
TODO
1. 
*/

int max(int a, int b)
{
	if(a>b)
	return a;
	else
	return b;
}
void* echo_server(void *connfd);
void* time_server(void *connfd);

int main(int argc, char **argv)
{
	int LISTENQ = 50, MAXLINE = 512;
	int echo_listenfd, time_listenfd;
	
	int connfd[1024];
	pthread_t thread[1024];
	
	int conn_counter=0;
	char buff[MAXLINE];
	long arg1, arg2;
	ssize_t n;
	
	int maxfdp1;
	fd_set rset;
	socklen_t clilen;
	time_t	ticks;
	struct sockaddr_in cliaddr, time_servaddr, echo_servaddr;
	socklen_t len;
	const int sockopt = 1;
	
	int flags;

	echo_listenfd = socket(AF_INET, SOCK_STREAM, 0);
	time_listenfd = socket(AF_INET, SOCK_STREAM, 0);
		
	bzero(&time_servaddr, sizeof(time_servaddr));
	time_servaddr.sin_family      = AF_INET;
	time_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	time_servaddr.sin_port        = htons(12563);
	
	bzero(&echo_servaddr, sizeof(echo_servaddr));
	echo_servaddr.sin_family      = AF_INET;
	echo_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	echo_servaddr.sin_port        = htons(7675);
	
	setsockopt(echo_listenfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));
	setsockopt(time_listenfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));

	bind(echo_listenfd, (struct sockaddr *) &echo_servaddr, sizeof(echo_servaddr));
	bind(time_listenfd, (struct sockaddr *) &time_servaddr, sizeof(time_servaddr));
	
	
	if((flags = fcntl(time_listenfd, F_GETFL,0)) < 0)
		printf("F_GETFL error in setting time listen socket to non blocking\n");
	
     	flags |= O_NONBLOCK;
     	
     	if((fcntl(time_listenfd, F_SETFL, flags)) < 0)
     		printf("F_SETFL error in setting time listen socket to non blocking\n");
 
 	if((flags = fcntl(echo_listenfd, F_GETFL,0)) < 0)
		printf("F_GETFL error in setting echo socket to non blocking\n");
	
     	flags |= O_NONBLOCK;
     	
     	if((fcntl(echo_listenfd, F_SETFL, flags)) < 0)
     		printf("F_SETFL error in setting echo socket to non blocking\n");
	
     	
	listen(echo_listenfd, LISTENQ);
	listen(time_listenfd, LISTENQ);
		
	maxfdp1 = max(echo_listenfd, time_listenfd) + 1;
	
	while(1)
	{
		FD_ZERO(&rset);
		FD_SET(echo_listenfd, &rset);
		FD_SET(time_listenfd, &rset);
		
		int k = select(maxfdp1, &rset, NULL, NULL, NULL);
		
		if(FD_ISSET(time_listenfd, &rset))
		{
			len = sizeof(cliaddr);
			connfd[conn_counter] = accept(time_listenfd, NULL, NULL);
			flags &= ~O_NONBLOCK;
			if(fcntl(connfd[conn_counter], F_SETFL, flags) < 0)
				printf("F_SETFL error");
			int iret1;
			iret1 = pthread_create( &thread[conn_counter], NULL, &time_server, (void*) connfd[conn_counter]);
			pthread_detach(thread[conn_counter]);
			conn_counter = conn_counter + 1;
		}
		if(FD_ISSET(echo_listenfd, &rset))
		{
			len = sizeof(cliaddr);
			connfd[conn_counter] = accept(echo_listenfd, (struct sockaddr *) &cliaddr, &len);
			flags &= ~O_NONBLOCK;
			if(fcntl(connfd[conn_counter], F_SETFL, flags) < 0)
				printf("F_SETFL error");
			int iret2;
			iret2 = pthread_create( &thread[conn_counter], NULL, &echo_server, (void*) connfd[conn_counter]);
			pthread_detach(thread[conn_counter]);
			conn_counter = conn_counter + 1;
		}
	}
}

void *time_server(void *connfd)
{
	time_t	ticks;	
	char buff[512];
	int ret;
	
	signal(SIGPIPE, SIG_IGN);
	
	while(1)
	{
		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		ret = write(connfd, buff, strlen(buff));
		if(ret < 1)
		{
			break;
		}
		sleep(5);
	}        
	close(connfd);
}

void *echo_server(void *connfd)
{
	int n;
	int MAXLINE = 512;
	char buff[MAXLINE];
	
	signal(SIGPIPE, SIG_IGN);	
	while(1) 
	{
		if ( (n = recv(connfd, buff, MAXLINE, 0)) < 1)
		{
			break;		
		}
		send(connfd, buff, n, 0);	
	}
	close(connfd);
}
