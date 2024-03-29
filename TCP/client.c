#ifdef _WIN32
	#define _WIN32_WINNT _WIN32_WINNT_WIN7
	#include <winsock2.h> //for all socket programming
	#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
	void OSInit( void )
	{
		WSADATA wsaData;
		int WSAError = WSAStartup( MAKEWORD( 2, 0 ), &wsaData ); 
		if( WSAError != 0 )
		{
			fprintf( stderr, "WSAStartup errno = %d\n", WSAError );
			exit( -1 );
		}
	}
	void OSCleanup( void )
	{
		WSACleanup();
	}
	#define perror(string) fprintf( stderr, string ": WSA errno = %d\n", WSAGetLastError() )
#else
	#include <sys/socket.h> //for sockaddr, socket, socket
	#include <sys/types.h> //for size_t
	#include <netdb.h> //for getaddrinfo
	#include <netinet/in.h> //for sockaddr_in
	#include <arpa/inet.h> //for htons, htonl, inet_pton, inet_ntop
	#include <errno.h> //for errno
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
	void OSInit( void ) {}
	void OSCleanup( void ) {}
#endif //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int initialization();
void execution( int internet_socket );
void cleanup( int internet_socket );

int main( int argc, char * argv[] )
{

	OSInit(); //Initialization
	int internet_socket = initialization(); //Initialization

	execution( internet_socket ); //Execution

	cleanup( internet_socket ); //Clean up
	OSCleanup(); //Clean up

	return 0;
}

int initialization() //--------------------------------------------------------------------------------------------------------
{
	struct addrinfo internet_address_setup;
	struct addrinfo * internet_address_result;
	memset( &internet_address_setup, 0, sizeof internet_address_setup );
	internet_address_setup.ai_family = AF_UNSPEC;
	internet_address_setup.ai_socktype = SOCK_STREAM;
	int getaddrinfo_return = getaddrinfo( "::1", "24042", &internet_address_setup, &internet_address_result );
	if( getaddrinfo_return != 0 )
	{
		fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
		exit( 1 );
	}

	int internet_socket = -1;
	struct addrinfo * internet_address_result_iterator = internet_address_result;
	while( internet_address_result_iterator != NULL )
	{
		internet_socket = socket( internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol );
		if( internet_socket == -1 )
		{
			perror( "socket" );
		}
		else
		{
			int connect_return = connect( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
			if( connect_return == -1 )
			{
				perror( "connect" );
				close( internet_socket );
			}
			else
			{
				break;
			}
		}
		internet_address_result_iterator = internet_address_result_iterator->ai_next;
	}

	freeaddrinfo( internet_address_result );

	if( internet_socket == -1 )
	{
		fprintf( stderr, "socket: no valid socket address found\n" );
		exit( 2 );
	}
	return internet_socket;
} //--------------------------------------------------------------------------------------------------------

void execution( int internet_socket ) //--------------------------------------WORK HERE----------------------------------------------------------
{
   printf("\n");
  char problem[10000];
   printf("Enter a problem example '5 + 7' \n");
   printf("\n");
  while (1){
   
   fgets(problem, sizeof(problem), stdin);
   printf("\n");
   printf("problem =  %s  \n", problem);
  if (strcmp(problem,"STOP") == 1) {
    //return;
    
     int number_of_bytes_send2 = 0;
     number_of_bytes_send2 = send( internet_socket, problem, strlen(problem), 0 );
     break;
  }
  printf ("Sending : %s\n",problem);
  int number_of_bytes_send = 0;
  number_of_bytes_send = send( internet_socket, problem, strlen(problem), 0 );
  
  if( number_of_bytes_send == -1 )
	{
		perror( "send" );
	}
  int number_of_bytes_received = 0;
  char buffer[1000];
  number_of_bytes_received = recv( internet_socket, buffer, ( sizeof buffer ) - 1, 0 );
  if( number_of_bytes_received == -1 )
  {
		perror( "recv" );
	}
	else
	{
		buffer[number_of_bytes_received] = '\0';
		printf( "Received : %s\n", buffer );
		printf("\n");
		
	}
  }
  int number_of_bytes_send = 0;
  number_of_bytes_send = send( internet_socket, "KTNXBYE",7, 0 );
} //--------------------------------------------------------------------------------------------------------

void cleanup( int internet_socket ) //--------------------------------------------------------------------------------------------------------
{
	int shutdown_return = shutdown( internet_socket, SD_SEND );
	if( shutdown_return == -1 )
	{
		perror( "shutdown" );
	}
	close( internet_socket );
} //--------------------------------------------------------------------------------------------------------