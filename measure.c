/*
    TCP/IP-server
*/

#include<stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netinet/tcp.h>
#include <time.h>

#define SERVER_PORT 12345  //The port that the server listens
#define LENGTH 65536
#define FILESIZE 1048576
  
int main()
{

signal(SIGPIPE, SIG_IGN); // on linux to prevent crash on closing socket

    // Open the listening (server) socket
    int sock;  
	sock = socket(AF_INET , SOCK_STREAM , 0 );

    if(sock == -1) 
    {
        printf("Could not create listening socket : %d" ,errno);
    }

     printf ("measure: open socket %d succesfuly /n", sock);

	// Reuse the address if the server socket on was closed
	// and remains for 45 seconds in TIME-WAIT state till the final removal.

    int enableReuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0)
    {
         printf("setsockopt() failed with error code : %d" , errno);
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);  //network order

      
    // Bind the socket to the port with any IP at this port
    if (bind (sock, (struct sockaddr *)&serverAddress , sizeof(serverAddress)) == -1)
    {
        printf("Bind failed with error code : %d" , errno);

	// TODO: close the socket
        return -1;
    }
      
    printf("Bind() success\n");
  
    // Make the socket listening; actually mother of all client sockets.
    if (listen(sock, 500) == -1) //a Maximum size of queue connection requests
											//number of concurrent connections 
    {
	printf("listen() failed with error code : %d \n",errno);
	// TODO: close the socket
        return -1;
    }
      
    //Accept and incoming connection
    printf("Waiting for incoming TCP-connections...\n");

    int clientSocket; 
    struct sockaddr_in clientAddress;  
    socklen_t addressLen = sizeof(clientAddress);

   
    memset(&clientAddress, 0, sizeof(clientAddress));
    addressLen = sizeof(clientAddress);

    time_t startC, endC;
    clientSocket = accept(sock, (struct sockaddr *)&clientAddress, &addressLen);
    if (clientSocket == -1)
    {
        printf("accept failed with error code : %d",errno);
	   // TODO: close the sockets
           return -1;
    	}
    
    printf ("sender connected succsesfuly \n");
    startC = time(NULL);
    char filename[LENGTH];

    int stream =0;
    int i =0;
    while (i<5){
    while (stream = recv(clientSocket, filename, LENGTH, 0))  {
      if (stream == -1){
           printf("accept failed with error code : %d",errno);
	   // TODO: close the sockets
           return -1;
      }

        }
        i++;
     printf ("measur recived %d times \n", i);
  
    }
    endC = time(NULL);

    printf ("The time of cubic is %.4f\n", (difftime(endC, startC))); 


    /* change CC */

    char buf[LENGTH];
    socklen_t len;

    strcpy(buf, "reno");
    len = strlen(buf);
  

    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0) {
        perror("setsockopt"); 
        return -1;
    }
    printf ("Change to reno \n");
    
    time_t startR, endR;

    startR = time(NULL);

    stream =0;
     i =0;
    while (i<5){
    while (stream = recv(clientSocket, filename, LENGTH, 0))  {
      if (stream == -1){
           printf("accept failed with error code : %d",errno);
	   // TODO: close the sockets
           return -1;
      }

        }
        i++;
     printf ("measur recived %d times \n", i);
  
    }
    endR = time(NULL);
    float timeCaount = difftime(endR, startR);
    printf ("The time of reno is %.4f \n", timeCaount); 

    // TODO: All open clientSocket descriptors should be kept
    // in some container and closed as well.
    close(sock);              
    return 0;
}