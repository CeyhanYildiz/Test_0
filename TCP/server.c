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
int connection( int internet_socket );
void execution( int internet_socket );
void cleanup( int internet_socket, int client_internet_socket );

int main( int argc, char * argv[] )
{
	OSInit(); //Initialization
	int internet_socket = initialization(); //Initialization
	int client_internet_socket = connection( internet_socket ); //Connection
  
  fd_set readfds;
  struct timeval timeout;
  timeout.tv_sec = 5; // 5 second timeout
  timeout.tv_usec = 0;
  
	execution( client_internet_socket ); //Execution

	cleanup( internet_socket, client_internet_socket ); //Clean up
	OSCleanup(); //Clean up

	return 0;
}

int initialization()//--------------------------------------------------------------------------------------------------------
{
	struct addrinfo internet_address_setup;
	struct addrinfo * internet_address_result;
	memset( &internet_address_setup, 0, sizeof internet_address_setup );
	internet_address_setup.ai_family = AF_UNSPEC;
	internet_address_setup.ai_socktype = SOCK_STREAM;
	internet_address_setup.ai_flags = AI_PASSIVE;
	int getaddrinfo_return = getaddrinfo( NULL, "24042", &internet_address_setup, &internet_address_result );
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
			int bind_return = bind( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
			if( bind_return == -1 )
			{
				perror( "bind" );
				close( internet_socket );
			}
			else
			{
				int listen_return = listen( internet_socket, 1 );
				if( listen_return == -1 )
				{
					close( internet_socket );
					perror( "listen" );
				}
				else
				{
					break;
				}
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

int connection( int internet_socket ) //--------------------------------------------------------------------------------------------------------
{
	struct sockaddr_storage client_internet_address;
	socklen_t client_internet_address_length = sizeof client_internet_address;
	int client_socket = accept( internet_socket, (struct sockaddr *) &client_internet_address, &client_internet_address_length );
	if( client_socket == -1 )
	{
		perror( "accept" );
		close( internet_socket );
		exit( 3 );
	}
	return client_socket;
} //--------------------------------------------------------------------------------------------------------

void execution( int internet_socket ) //---------------------------------WORK HERE-------------------------------------------------
{  
  while (1) {
  int number_of_bytes_received = 0;
  char buffer[10000];
  number_of_bytes_received = recv( internet_socket, buffer, ( sizeof buffer ) - 1, 0 );


  if( number_of_bytes_received == -1 )
	{
		perror( "recv" );
	}
	else
	{
	  if (strcmp(buffer,"STOP") == 1) {
    printf("Received : Client asked to  : %s",buffer);
    break;
  }
  printf("Client asked to solve : %s",buffer);
  printf("\n");
	  char str[1000];
    int num1 , num2 ,result;
    char op;
     printf("\n");
		buffer[number_of_bytes_received] = '\0';
		//printf( "Received : %s", buffer );
		 printf("\n");
		sscanf(buffer, "%d %c %d", &num1, &op, &num2);
		switch (op) {
    case '+':
     result = num1 + num2;  // perform addition
      printf("Solution : %d + %d = %d\n", num1, num2, result);
      break;
    case '-':
     result = num1 - num2;  // perform subtraction
      printf("Solution : %d - %d = %d\n", num1, num2, result);  
      break;
    case '*':
    case 'x':
     result = num1 * num2;  // perform multiplication
      printf("Solution : %d * %d = %d\n", num1, num2, result);  
      break;
    case '/':
     result = num1 / num2;  // perform division
      printf("Solution : %d / %d = %d\n", num1, num2, result);  
      break;
    default:
      printf("Invalid operator\n");  // handle invalid operator
      break;
       //strcpy(str, "Invalid operator");
  }
  int number_of_bytes_send = 0;
      sprintf(str, "%d", result);
      sprintf(str, "%d", result);
  	number_of_bytes_send = send( internet_socket, str, ( sizeof result ), 0 );
     printf("Sending : result = %d\n", result);
     num1 = 0;
     num2 = 0;
     result = 0;
     	if( number_of_bytes_send == -1 )
	{
		perror( "send" );
	}
	}
	
	
 }
} //--------------------------------------------------------------------------------------------------------

void cleanup( int internet_socket, int client_internet_socket ) //--------------------------------------------------------------------------------------------------------
{
	int shutdown_return = shutdown( client_internet_socket, SD_RECEIVE );
	if( shutdown_return == -1 )
	{
		perror( "shutdown" );
	}
	close( client_internet_socket );
	close( internet_socket );
} //--------------------------------------------------------------------------------------------------------