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
#include <sys/signal.h>
#include <sys/signal.h>
#include <wait.h>

static void sigchld_handle(int sig)
{
	int status;
	pid_t pid;
	while((pid = waitpid(-1, &status, WNOHANG)) > 0);

	printf("\nSIGCHILD received by client parent.\n");
	printf("Parent (PID = %d) : signal %d child %d status 0x%.4X\n", (int)getpid(), sig, (int)pid, status);
	printf("client : sigchild_handler : Forked child has terminated completely. Please continue \n");
}

int main(int argc, char **argv)
{
	
	int selection;
	int result;
	char *inputptr;
	struct sockaddr_in input;
	struct hostent *hostdomain;
	int fd[2];
	
	//SIGCHLD code
	struct sigaction chldsa;
	sigemptyset(&chldsa.sa_mask);
	chldsa.sa_flags = SA_RESTART;
	chldsa.sa_handler = sigchld_handle;
	sigaction(SIGCHLD, &chldsa, NULL);
	
	pid_t echo_child_pid, time_child_pid;
	
	if (argc != 2)
	{
		
		printf("client : The correct usage: ./client <IPaddress> \n");
		printf("client : Please try again. \n");
		exit(0);
	}
	else
	{
		inputptr = argv[1];
		result = inet_pton(AF_INET, inputptr, &(input.sin_addr));
		if(result)
		{
			hostdomain = gethostbyaddr( (char *)&input.sin_addr.s_addr, sizeof(input.sin_addr.s_addr), AF_INET);
			printf("The domain name for IP address %s is : %s \n", argv[1], hostdomain->h_name);
		}
		else
		{
			char domain_name[128];
			int i;
			struct in_addr **addr_list;
			hostdomain = gethostbyname(inputptr);
			addr_list = (struct in_addr **)hostdomain->h_addr_list;
			
			printf("The IP address(s) for the domain name, %s, is as follows:\n");
			for(i = 0; addr_list[i] != NULL; i++) 
			{
				printf("%s \n", inet_ntoa(*addr_list[i]));
			}
		}
	}
	
	
	while(1)
	{
		pipe(fd);
		fflush(stdin);
		printf("\nclient : Please choose 1. Echo 2. Time 3. Quit \n");
		scanf("%d", &selection); //TODO String causing error
		if(selection == 1)
		{
			printf("client : You have chosen to make a echo request \n");
			if( (echo_child_pid = fork()) != -1)
			{
				if(echo_child_pid == 0)
				{
					//In child
					close(fd[0]);
					char writeFD[10];
					sprintf(writeFD, "%d", fd[1]); 

					if ( (execlp("xterm", "xterm", "-e", "./echo_client", writeFD, (char *) 0)) < 0) //TODO Pass ip address as parameter
					{  
						printf("\nclient : Error, Creation of a xterm window failed. \n");
						break;
					}
				}
				else
				{
					//In Server
					close(fd[1]);
					char buf[1024];
					while(1)
					{
						if( (read(fd[0], buf, sizeof(buf))) > 0)
							printf("%s", buf);
						else
							break;
					}
					close(fd[0]);				
				}
			}
			else
			{
				printf("client->echo : Unable to fork off a child.\n");
				continue;
			}
	
		}
		else if(selection == 2)
		{
			printf("client : You have chosen to make a time request \n");
			if( (time_child_pid = fork()) != -1)
			{
				if(time_child_pid == 0)
				{
					//In child
					close(fd[0]);
					char writeFD[10];
					sprintf(writeFD, "%d", fd[1]); 
					
					if ( (execlp("xterm", "xterm", "-e", "./time_client", writeFD, (char *) 0)) < 0)  //TODO Pass ip address
					{  
						printf("\nclient : Error, Creation of a xterm window failed. \n");
						break;
					}
				}
				else
				{
					//In Server
					close(fd[1]);
					char buf[1024];
					while(1)
					{
						if( (read(fd[0], buf, sizeof(buf))) > 0)
							printf("%s", buf);
						else
							break;
					}
					close(fd[0]);
				}
			}
			else
			{
				printf("client->time : Unable to fork off a child.\n");
				continue;
			}
		}
		else if(selection == 3)
		{
			printf("client : You have chosen to exit. \n");
			exit(1);
		}	
		else
		{
			printf("client : Please enter either 1, 2 or 3. \n");
			continue;
		}
		
	
		
	}
}
