#ifdef _WIN32
	#define _WIN32_WINNT _WIN32_WINNT_WIN7
	#include <winsock2.h> //for all socket programming
	#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
	#include <time.h>
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
	int OSInit( void ) {}
	int OSCleanup( void ) {}
#endif //----------------------------------------cecking if its windows or linux-------------------------------------

int initialization();
void execution( int internet_socket );
void execution2( int internet_socket );
void execution3( int internet_socket );
void cleanup( int internet_socket );
//--------------------------NIET AANRAKEN---------------------------------------------
// ----------------------------Work Place-------------------------------------
int initialization2();
int connection( int internet_socket );
void execution4( int internet_socket );
void cleanup2( int internet_socket, int client_internet_socket );
// ----------------------------Work Place-------------------------------------
int main( int argc, char * argv[] )
{
       
   printf("\n");
	 printf("Starting UTP-server\n");
	 printf("\n");
    OSInit(); //Initialization
    int internet_socket = initialization(); //Initialization
    
    DWORD timeout = 1 * 3000;
    setsockopt(internet_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout); 
    
    execution( internet_socket );//Execution
    execution2( internet_socket );//Execution -- CECKPOINT// don't mess up whit the code above !!!  ceckpoint
   execution3( internet_socket );//Execution -- CECKPOINT
    
    cleanup( internet_socket ); //Clean up
    OSCleanup(); //Clean up
	 printf("UTP-server is Closed\n");
	 printf("\n");

//--------------------------NIET AANRAKEN---------------------------------------------
// ----------------------------Work Place-------------------------------------
	 printf("Starting TCP-server\n");
	 printf("\n");
	OSInit(); //Initialization
	int internet_socket2 = initialization2(); //Initialization
	int client_internet_socket = connection( internet_socket2 ); //Connection
  
  fd_set readfds;
  struct timeval timeout2;
  timeout2.tv_sec = 5; // 5 second timeout
  timeout2.tv_usec = 0;
  
	execution4( client_internet_socket ); //Execution

	cleanup2( internet_socket2, client_internet_socket ); //Clean up
	OSCleanup(); //Clean up


// ----------------------------Work Place-------------------------------------
//--------------------------NIET AANRAKEN---------------------------------------------
	return 0;
}

int initialization() //-------------------initialization-----------------------NIET AANRAKEN------------------------------------------------------------------------------
{
	struct addrinfo internet_address_setup;
	struct addrinfo * internet_address_result;
	memset( &internet_address_setup, 0, sizeof internet_address_setup );
	internet_address_setup.ai_family = AF_UNSPEC;
	internet_address_setup.ai_socktype = SOCK_DGRAM;
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
				close( internet_socket );
				perror( "bind" );
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
} //-------------------initialization----NIET AANRAKEN---------------------------------------------

void execution( int internet_socket ) // ik stuur een string inplaats van byte's //--------------------------NIET AANRAKEN---------------------------------------------
{
 	int number_of_bytes_received = 0;
 	char buffer[1000];
	struct sockaddr_storage client_internet_address;
	socklen_t client_internet_address_length = sizeof client_internet_address;
	number_of_bytes_received = recvfrom( internet_socket, buffer, ( sizeof buffer ) - 1, 0, (struct sockaddr *) &client_internet_address, &client_internet_address_length );
	
	if( number_of_bytes_received == -1 )
  {
		perror( "recvfrom" );
	}
	else
	{
		buffer[number_of_bytes_received] = '\0';
		printf("\n");
		printf( "Received : %s\n", buffer );
    printf("\n");
    
    int random_number = 0;
    int count = 1;
    srand(time(NULL));
    
    do {
    random_number = rand() % 100 + 1 ;
    char bericht[100];
    sprintf(bericht, "%d",random_number);
	  printf( "Sending : Number %2d = %d \n",count, random_number );
	  int number_of_bytes_send = 0;
	  number_of_bytes_send = sendto( internet_socket, bericht, strlen(bericht), 0, (struct sockaddr *) &client_internet_address, client_internet_address_length );
    count++;
    	
    if( number_of_bytes_send == -1 )
      {
        perror( "sendto" );
      }
      
    } while (count <= 42); // sends 42 random numbers using string 
  } 
  printf("\n");
  
  for ( int i = 0 ; i <= 2 ; i++ ){
  int biggest_number_of_bytes_received = 0;
  char biggest_number_buffer[1000];
  biggest_number_of_bytes_received = recvfrom( internet_socket, biggest_number_buffer , ( sizeof biggest_number_buffer )- 1, 0, (struct sockaddr *) &client_internet_address, &client_internet_address_length );
  
  if( biggest_number_of_bytes_received == -1 )
  {
		perror( "recvfrom" );
  }
  else
	{
		buffer[number_of_bytes_received] = '\0';
		printf( "Received : Biggest number = %s\n", biggest_number_buffer );
    printf("\n");
  }
 } 
} //--------------------------NIET AANRAKEN---------------------------------------------

void execution2( int internet_socket ) // ik stuur een string inplaats van byte's //--------------------------NIET AANRAKEN---------------------------------------------
{

 	int number_of_bytes_received = 0;
 	char buffer[1000];
	struct sockaddr_storage client_internet_address;
	socklen_t client_internet_address_length = sizeof client_internet_address;
	
    printf("\n");
  
    int random_number = 0;
    int count = 1;
    srand(time(NULL));
    
    do {
    random_number = rand() % 1000 + 1 ;
    char bericht[100];
    sprintf(bericht, "%d",random_number);
	  printf( "Sending : Number %2d = %d \n",count, random_number );
	  int number_of_bytes_send = 0;
	  number_of_bytes_send = sendto( internet_socket, bericht, strlen(bericht), 0, (struct sockaddr *) &client_internet_address, client_internet_address_length );
    count++;
    	
    if( number_of_bytes_send == -1 )
      {
        perror( "sendto" );
      }
      
    } while (count <= 42); // sends 42 random numbers using string 
     
  printf("\n");
  for ( int i = 0 ; i <= 2 ; i++ ){
  int biggest_number_of_bytes_received = 0;
  char biggest_number_buffer[1000];
  biggest_number_of_bytes_received = recvfrom( internet_socket, biggest_number_buffer , ( sizeof biggest_number_buffer )- 1, 0, (struct sockaddr *) &client_internet_address, &client_internet_address_length );
  
  if( biggest_number_of_bytes_received == -1 )
  {
		perror( "recvfrom" );
  }
  else
	{
		buffer[number_of_bytes_received] = '\0';
		printf( "Received : Biggest number = %s\n", biggest_number_buffer );
    printf("\n");
  }
 } 
} //--------------------------NIET AANRAKEN---------------------------------------------

void execution3( int internet_socket ) //--------------------------NIET AANRAKEN---------------------------------------------
{
	int number_of_bytes_received = 0;
	char buffer[1000];
	struct sockaddr_storage client_internet_address;
	socklen_t client_internet_address_length = sizeof client_internet_address;
	int number_of_bytes_send = 0;
	 number_of_bytes_send = sendto( internet_socket, "OK!", 4, 0, (struct sockaddr *) &client_internet_address, client_internet_address_length );
	 printf("Sending : OK! \n");
	if( number_of_bytes_send == -1 )
	{
		perror( "sendto" );
	}
} //--------------------------NIET AANRAKEN---------------------------------------------

void cleanup( int internet_socket ) //--------------------------NIET AANRAKEN---------------------------------------------
{
	 printf("\n");
	 printf("Closing UDP-server\n");
	 printf("\n");
	close( internet_socket );
} //--------------------------NIET AANRAKEN---------------------------------------------



// ----------------------------Work Place-------------------------------------


int initialization2()//--------------------------------------------------------------------------------------------------------
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

void execution4( int internet_socket ) //---------------------------------WORK HERE-------------------------------------------------
{  
  printf("\n");

int x = 0;
  while (1) {
  int number_of_bytes_received = 0;
  char buffer[100];
  number_of_bytes_received = recv( internet_socket, buffer, ( sizeof buffer ) - 1, 0 );


  if( number_of_bytes_received == -1 )
	{
		perror( "recv" );
	}
	else
	{
	  if (strcmp(buffer,"STOP") == 1 || strcmp(buffer,"OK") == 1 && x >= 1)  {
	  printf("\n\n");
    printf("Received : Client asked to %s",buffer);
    int number_of_bytes_send2 = 0;
     number_of_bytes_send2 = send( internet_socket, "OK",50, 0 );
    break;
  }
  x++;
  printf("Client asked to solve : %s",buffer);
  //printf("\n");
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
  }
  int number_of_bytes_send = 0;
      sprintf(str, "%d", result);
  	number_of_bytes_send = send( internet_socket, str, ( sizeof str ), 0 );
     printf("Sending : result = %d\n", result);
     num1 = 0;
     num2 = 0;
     result = 0;
     	if( number_of_bytes_send == -1 )
	{
		perror( "send" );
	}
	}
	 printf("\n");
	  printf("\n");
 }
 int number_of_bytes_received3 = 0;
  char buffer2[8];
  number_of_bytes_received3 = recv( internet_socket, buffer2, ( sizeof buffer2 ) - 1, 0 );
	printf("Received : %s",buffer2);
} //--------------------------------------------------------------------------------------------------------

void cleanup2( int internet_socket, int client_internet_socket ) //--------------------------------------------------------------------------------------------------------
{
	int shutdown_return = shutdown( client_internet_socket, SD_RECEIVE );
	if( shutdown_return == -1 )
	{
		perror( "shutdown" );
	}
	close( client_internet_socket );
	close( internet_socket );
} //--------------------------------------------------------------------------------------------------------









// ----------------------------Work Place-------------------------------------
































/*     code voor hton maar ik kreeg het niet te werken met de client maar normaal gezien moets deze code werken


void execution( int internet_socket )
{
	int number_of_bytes_received = 0;
	char buffer[1000];
	struct sockaddr_storage client_internet_address;
	socklen_t client_internet_address_length = sizeof client_internet_address;
	number_of_bytes_received = recvfrom( internet_socket, buffer, ( sizeof buffer ) - 1, 0, (struct sockaddr *) &client_internet_address, &client_internet_address_length );
	if( number_of_bytes_received == -1 )
	{
		perror( "recvfrom" );
	}
	else
	{
		buffer[number_of_bytes_received] = '\0';
		      
		printf("\n");
		printf( "Received : %s\n", buffer );
	} // Recieved GO!
	// now Send 42 random numbers 
	
      srand(time(NULL)); // to make every things more random
      printf("\n");
      int count = 1; 
      
      // start do while loop 
      
	    int random_num = rand() % 126 + 1; // generate a random number between 1 and 126
	    printf("Sending Number %d = %d \n", count , random_num);  // Human taal
      uint32_t network_number = htonl(random_num); //Human taal naar Computer Taal
      printf("Sending Number %d = %d TEST \n\n", count , network_number);  //Computer Taal printen om te testen


*/