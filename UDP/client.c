#ifdef _WIN32
	#define _WIN32_WINNT _WIN32_WINNT_WIN7
	#include <winsock2.h> //for all socket programming
	#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
	#include <stdint.h>

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
#endif //----------------------------------------cecking if its windows or linux-------------------------------------

int initialization( struct sockaddr ** internet_address, socklen_t * internet_address_length );
void execution( int internet_socket, struct sockaddr * internet_address, socklen_t internet_address_length );
void execution2( int internet_socket, struct sockaddr * internet_address, socklen_t internet_address_length );
void execution3( int internet_socket, struct sockaddr * internet_address, socklen_t internet_address_length );
void cleanup( int internet_socket, struct sockaddr * internet_address );

int initialization_TCP ();
void execution_TCP( int internet_socket );
void cleanup_TCP( int internet_socket );

int main( int argc, char * argv[] )
{
   printf("\n");
	 printf("Starting UTP-server\n");
	 printf("\n");
   OSInit(); //Initialization
    struct sockaddr * internet_address = NULL; //Initialization
    socklen_t internet_address_length = 0; //Initialization
    int internet_socket = initialization( &internet_address, &internet_address_length ); //Initialization
    
    // DWORD timeout = 1 * 1000;
    //setsockopt(internet_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout); 
    
    //execution( internet_socket, internet_address, internet_address_length ); //Execution
    //execution2( internet_socket, internet_address, internet_address_length ); //Execution  -- CECKPOINT   // don't mess up whit the code above !!! ceckpoint
    //execution3( internet_socket, internet_address, internet_address_length ); //Execution  -- CECKPOINT
    
    cleanup( internet_socket, internet_address ); //Clean up
    OSCleanup(); //Clean up
    
   printf("UTP-client is Closed\n");
	 printf("\n");
	 printf("Starting TCP-Client\n");
	 printf("\n");
	 
   OSInit(); //Initialization
   int internet_socket_TCP = initialization_TCP(); //Initialization
   execution_TCP( internet_socket_TCP ); //Execution
   cleanup_TCP( internet_socket_TCP ); //Clean up
	return 0;
}

int initialization( struct sockaddr ** internet_address, socklen_t * internet_address_length ) //---initialization----------------------------------------------------------------------------------
{
	struct addrinfo internet_address_setup;
	struct addrinfo * internet_address_result;
	memset( &internet_address_setup, 0, sizeof internet_address_setup );
	internet_address_setup.ai_family = AF_UNSPEC;
	internet_address_setup.ai_socktype = SOCK_DGRAM;
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
			*internet_address_length = internet_address_result_iterator->ai_addrlen;
			*internet_address = (struct sockaddr *) malloc( internet_address_result_iterator->ai_addrlen );
			memcpy( *internet_address, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
			break;
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
} //-----------------------------------------------------------------------------------------------initialization-----------------------------------------------------------------


void execution( int internet_socket, struct sockaddr * internet_address, socklen_t internet_address_length )
{
	int count = 1;
	int biggest_number = 0;
	int number_of_bytes_send = 0; 
	int count_of_biggest_number = 0; 
	number_of_bytes_send = sendto( internet_socket, "GO!", 3, 0, internet_address, internet_address_length );
	printf("\n");
	printf("Sending : GO!\n");
	if( number_of_bytes_send == -1 )
	{
		perror( "sendto" );

	}
	printf("\n");
  
  do {
  int number_of_bytes_received = 0;
  char buffer[1000];
  memset(buffer,0 , sizeof(buffer));
  number_of_bytes_received = recvfrom( internet_socket, buffer, ( sizeof buffer ) - 1, 0, internet_address, &internet_address_length );
  int random_number = atoi(buffer);
  
  if(random_number >= biggest_number)
  {
    biggest_number = random_number; 
    count_of_biggest_number = count;
  }
  
  if( number_of_bytes_received == -1 )
	{
		perror( "recvfrom" );
		return ;
	}
	else
	{
		buffer[number_of_bytes_received] = '\0'; 
		printf( "Received : Number %2d = %d \n",count, random_number ); 
  
  count++;
  }
  } while (count <= 42);
  
  printf("\n");
  for ( int i = 0; i <= 2 ; i++){
  printf("Sending : Biggerst number is %d = %d \n",count_of_biggest_number,biggest_number);
  printf("\n");
  // don't mess up whit the code above !!! ceckpoint
   usleep(1000000); // sleep for 1 second (1000000 microseconds)
  int biggest_number_send = 0;
  char bericht_biggest_number[100];
  sprintf(bericht_biggest_number, "%d",biggest_number);
  biggest_number_send = sendto( internet_socket, bericht_biggest_number, strlen(bericht_biggest_number), 0, internet_address, internet_address_length );
  }
 }
 
void execution2( int internet_socket, struct sockaddr * internet_address, socklen_t internet_address_length )
{
  
	int count = 1;
	int biggest_number = 0;
	
	int number_of_bytes_send = 0; 
	int count_of_biggest_number = 0; 
	printf("\n");
  
  do {
  int number_of_bytes_received = 0;
  char buffer[1000];
  memset(buffer,0 , sizeof(buffer));
  number_of_bytes_received = recvfrom( internet_socket, buffer, ( sizeof buffer ) - 1, 0, internet_address, &internet_address_length );
  int random_number = atoi(buffer);
  
  if(random_number >= biggest_number)
  {
    biggest_number = random_number; 
    count_of_biggest_number = count;
  }
  
  if( number_of_bytes_received == -1 )
	{
		perror( "recvfrom" );
		return ;
	}
	else
	{
		buffer[number_of_bytes_received] = '\0'; 
		printf( "Received : Number %2d = %d \n",count, random_number ); 
  
  count++;
  }
  } while (count <= 42);
  
  printf("\n");
  for ( int i = 0; i <= 2 ; i++){
  printf("Sending : Biggerst number is %d = %d \n",count_of_biggest_number,biggest_number);
  printf("\n");
  // don't mess up whit the code above !!! ceckpoint
   usleep(1000000); // sleep for 1 second (1000000 microseconds)
  int biggest_number_send = 0;
  char bericht_biggest_number[100];
  sprintf(bericht_biggest_number, "%d",biggest_number);
  biggest_number_send = sendto( internet_socket, bericht_biggest_number, strlen(bericht_biggest_number), 0, internet_address, internet_address_length );
  }
    // don't mess up whit the code above !!!  ceckpoint
    // don't mess up whit the code above !!!  ceckpoint
    // don't mess up whit the code above !!!  ceckpoint

 } 

void execution3( int internet_socket, struct sockaddr * internet_address, socklen_t internet_address_length )
{
	int number_of_bytes_send = 0;
	int number_of_bytes_received = 0;
	char buffer[1000];
	number_of_bytes_received = recvfrom( internet_socket, buffer, ( sizeof buffer ) - 1, 0, internet_address, &internet_address_length );
	if( number_of_bytes_received == -1 )
	{
		perror( "recvfrom" );
	}
	else
	{
		buffer[number_of_bytes_received] = '\0';
		printf( "Received : %s\n", buffer );
	}
}

void cleanup( int internet_socket, struct sockaddr * internet_address )
{
	 printf("\n");
	 printf("Closing UDP-client\n");
	 printf("\n");
	free( internet_address );
	close( internet_socket );
}
int initialization_TCP() //--------------------------------------------------------------------------------------------------------
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
}	
	void execution_TCP( int internet_socket_TCP ) //--------------------------------------WORK HERE----------------------------------------------------------
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
     number_of_bytes_send2 = send( internet_socket_TCP, problem, strlen(problem), 0 );
     break;
  }
  printf ("Sending : %s\n",problem);
  int number_of_bytes_send = 0;
  number_of_bytes_send = send( internet_socket_TCP, problem, strlen(problem), 0 );
  
  if( number_of_bytes_send == -1 )
	{
		perror( "send" );
	}
  int number_of_bytes_received = 0;
  char buffer[1000];
  number_of_bytes_received = recv( internet_socket_TCP, buffer, ( sizeof buffer ) - 1, 0 );
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
  number_of_bytes_send = send( internet_socket_TCP, "KTNXBYE",7, 0 );
} 

void cleanup_TCP( int internet_socket_TCP ) 
{
	int shutdown_return = shutdown( internet_socket_TCP, SD_SEND );
	if( shutdown_return == -1 )
	{
		perror( "shutdown" );
	}
	close( internet_socket_TCP );
}
