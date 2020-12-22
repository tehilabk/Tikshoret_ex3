/*
	TCP/IP client
*/

#include <stdio.h>

#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 12345
#define SERVER_IP_ADDRESS "127.0.0.1"
#define LENGTH 65536

int main() {
    char buffer[LENGTH];
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    //socket- return socket or -1, AF_INET- struct of ipV4, SOCK_STREAM- struct of TCP
    if (sock == -1) {
        printf("Could not create socket : %d \n", errno);
    }
    printf("sender: open socket %d succesfuly \n", sock);

    struct sockaddr_in serverAddress; //sockaddr_in- socket internet address for ipV4 (family(ipv4), port, internet address)
    memset(&serverAddress, 0, sizeof(serverAddress)); //reset the adder

    serverAddress.sin_family = AF_INET; //ip and port dest
    serverAddress.sin_port = htons(SERVER_PORT); //the server port in byte
    int rval = inet_pton(AF_INET, (const char *) SERVER_IP_ADDRESS, &serverAddress.sin_addr);
    //convert to bit and return if failed
    if (rval <= 0) {
        printf("inet_pton() failed \n");
        return -1;
    }

    // Make a connection to the server with socket SendingSocket.
    int conn = connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    //return -1 if connect failed
    if (conn == -1) {
        printf("connect() failed with error code : %d \n", errno);
    }

    printf("sender: connected to server succesfuly %d \n", conn);

    /* Send File 5 times */
    int i = 0;
    while (i < 5) {

        char *file_name = "/home/user/Tikshoret_ex3/1mb.txt";
        char sbuffer[LENGTH];
        FILE *file = fopen(file_name, "r");
        if (file == NULL) {
            printf("ERROR:File %s not found.\n", file_name);
            return 1;
        }
        int sending;
        int stream;
        while ((stream = fread(sbuffer, sizeof(char), LENGTH, file)) > 0) {
            sending = send(sock, sbuffer, stream, 0);
            if (sending < 0) {
                printf("send() failed with error code : %d", errno);
                return 1;
            }
          
        }
        fclose(file);
        i++;
        printf("Sender Sent the file %d time success!\n", i);
        memset(&sbuffer, 0, sizeof(sbuffer)); //reset the buffer
    }
    //  sleep(5);
    /* change CC */

    char buf[LENGTH];
    socklen_t len;

    strcpy(buf, "reno");
    len = strlen(buf);


    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0) {
        perror("setsockopt");
        return -1;
    }
    printf("Change to reno \n");



    /* Send File 5 times */
    i = 0;

    while (i < 5) {
        char *file_name = "/home/user/Tikshoret_ex3/1mb.txt";
        char sbuffer[LENGTH];
        FILE *file = fopen(file_name, "r");
        if (file == NULL) {
            printf("ERROR: File %s not found.\n", file_name);
            return 1;
        }
        int stream;
        while ((stream = fread(sbuffer, sizeof(char), LENGTH, file)) > 0) {
            int sending = send(sock, sbuffer, stream, 0);
            if (sending < 0) {
                printf("send() failed with error code : %d", errno);
                return 1;
            }  
        }

        fclose(file);
        i++;
        printf("Sender Sent the file %d time success!\n", i);
        memset(&sbuffer, 0, sizeof(sbuffer)); //reset the buffer
    }

    sleep(3);


    close(sock);

    return 0;
}
