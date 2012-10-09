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
	int sockfd;
	struct sockaddr_in	servaddr;
	int MAXLINE = 512;	
	char recvline[MAXLINE], sendline[MAXLINE];
	
	int fd = atoi(argv[1]);
	char buf[1024];
	
	signal(SIGPIPE, SIG_IGN);
	
	sprintf(buf,"%s","\necho_client : Started\n");
	write(fd, buf, sizeof(buf));
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd == -1)
	{
		sprintf(buf,"%s","\necho_client : Unable to create socket. Exitting.\n");
		write(fd, buf, sizeof(buf));
		close(fd);
		exit(0);
	}
	else
	{
		printf("echo_client : Created Socket : %d \n", sockfd);
		sprintf(buf,"%s","\necho_client : Created Socket\n");
		write(fd, buf, sizeof(buf));
	}


	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(7675);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //TODO

	if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1)
	{
		write(fd, buf,sizeof(buf));

		sleep(5);
		exit(0);
	}
	else
	{
		sprintf(buf,"%s","\necho_client : Successfully connected to server\n");
		write(fd, buf, sizeof(buf));
		printf("echo_client : Successfully connected to server \n");
		printf("Anything you type will be echoed \n");
	}

	while (fgets(sendline, MAXLINE, stdin) != NULL) 
	{
		write(sockfd, sendline, strlen(sendline));

		if (read(sockfd, recvline, MAXLINE) < 1)
		{
			printf("echo_client: ERROR : server terminated prematurely. Exitting");
			sprintf(buf,"%s","\necho_client: server terminated prematurely. Exitting\n");
			write(fd, buf, sizeof(buf));
			break;
		}
		else
			write(1, recvline, strlen(sendline)); 
	}
	close(fd);
	close(sockfd);
	exit(1);
}
