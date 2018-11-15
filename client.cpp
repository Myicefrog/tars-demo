#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int sockfd = -1;

	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( -1 == sockfd ) {
        	perror( "sock created" );
        	exit( -1 );
   	 }
    
	struct sockaddr_in server;
	memset( &server, 0, sizeof( struct sockaddr_in ) );
	server.sin_family = AF_INET;
	server.sin_port = 9877;
	server.sin_addr.s_addr = inet_addr( "127.0.0.1" );
    
    	int res = -1;    
    	res = connect( sockfd, (struct sockaddr*)&server, sizeof( server ) );
    	if( -1 == res )
	{
        	perror( "sock connect" );
        	exit( -1 );
    	}

    	char sendBuf[1024] = { 0 };
    	char recvBuf[1024] = { 0 };
    	while( fgets( sendBuf, sizeof( sendBuf ), stdin ) != NULL ) 
	{
        	write( sockfd, sendBuf, sizeof( sendBuf ) );
        	read( sockfd, recvBuf, sizeof( recvBuf ) );
        	fputs( recvBuf, stdout );
        	memset( sendBuf, 0, sizeof( sendBuf ) );
        	memset( recvBuf, 0, sizeof( recvBuf ) );
   	}

    	close( sockfd );

    	return 0;
}
