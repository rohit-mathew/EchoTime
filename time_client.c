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
#include <signal.h>
#include <wait.h>

int main(int argc, char **argv)
{
	int sockfd, n;
	int MAXLINE = 512;
	char recvline[MAXLINE + 1];
	struct sockaddr_in servaddr;
	
	int fd = atoi(argv[1]);
	char buf[1024];
	
	signal(SIGPIPE, SIG_IGN);
	
	sprintf(buf,"%s","\ntime_client : Started\n");
	write(fd, buf, sizeof(buf));

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		sprintf(buf,"%s","\ntime_client : Unable to create socket. Exitting.\n");
		write(fd, buf, sizeof(buf));
		close(fd);
		exit(0);
	}
	else
	{
		printf("time client : Created a socket \n");
		sprintf(buf,"%s","\ntime client : Created a socket\n");
		write(fd, buf, sizeof(buf));
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(12563);
	servaddr.sin_addr.s_addr = inet_addr(argv[2]);

	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		sprintf(buf,"%s","\ntime client : Unable to connect to server. Please check server.\n");
		write(fd, buf, sizeof(buf));
		exit(0);
	}
	else
	{
		sprintf(buf,"%s","\ntime client : Connected to server\n");
		write(fd, buf, sizeof(buf));
	}
		

	while (1) 
	{
		if( (n = read(sockfd, recvline, MAXLINE)) > 0)
		{
			write(1, recvline, n); 
		}
		else
		{
			sprintf(buf,"%s","\ntime_client: ERROR : Server terminated prematurely. Exitting.\n");
			write(fd, buf, sizeof(buf));
			break;
		}
	}
	close(fd);
	close(sockfd);
	exit(1);
}
