PROGS =	client server echo_client time_client

# -D_REENTRANT
#-l socket
#Make this to Badr's format

all:	${PROGS}

client:	client.o echo_client time_client
		gcc -Wall -o client client.o 

server:	server.o
		gcc -Wall -o server server.o -lpthread

echo_client: echo_client.o
		gcc -Wall -o echo_client echo_client.o 
		
time_client: time_client.o
		gcc -Wall -o time_client time_client.o 
 
clean:
		rm -f ${PROGS} 
		rm -f *~
		rm -f *.o
