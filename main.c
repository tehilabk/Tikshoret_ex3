#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <time.h>

#define MAX_LINE 1048576
#define LINSTENPORT 7788
#define SERVERPORT 8877
#define LENGTH 1048576

void writefile(int sockfd, char[], struct sockaddr_in);
clock_t start, end;
double cpu_time_used=0;
double avg=0;
ssize_t total = 0;

int main(int argc, char *argv[]) {
    char buf[256];
    socklen_t len;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Can't allocate sockfd");
        exit(1);
    }

    struct sockaddr_in clientaddr, serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);

    if (bind(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) == -1) {
        perror("Bind Error");
        exit(1);
    }

    if (listen(sockfd, LINSTENPORT) == -1) {
        perror("Listen Error");
        exit(1);
    }
    int clientSocket;
    socklen_t addrlen = sizeof(clientaddr);
    clientSocket = accept(sockfd, (struct sockaddr *) &clientaddr, &addrlen);
    if (clientSocket == -1) {
        perror("Connect Error");
        exit(1);
    }

    char filename[LENGTH] = {0};
    if (recv(clientSocket, filename, LENGTH, 0) == -1) {
        perror("Can't receive filename");
        exit(1);
    }

    writefile(clientSocket, filename, clientaddr);
    printf("avg time of cubic %f \n",avg/5);
    avg=0;

    strcpy(buf, "reno");
    len = strlen(buf);
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
    {
        perror("setsockopt");
        return -1;
    }

    writefile(clientSocket, filename, clientaddr);
    printf("avg time of reno %f \n",avg/5);
    avg=0;

    close(sockfd);
    close(clientSocket);
    return 0;
}

void writefile(int sockfd, char filename[], struct sockaddr_in clientaddr) {
    ssize_t n;
    char buff[1000] = {0};
    int j = 0;
    start=clock();
    while ((n = recv(sockfd, buff, MAX_LINE, 0)) > 0) {
        char addr[INET_ADDRSTRLEN];
        char str[8];
        sprintf(str, "%d", j);
        FILE *fp = fopen(strncat(str, filename, 7), "wb");

        if (fp == NULL) {
            perror("Can't open file");
            exit(1);
        }

        printf("Start receive file: %s from %s\n", filename,
               inet_ntop(AF_INET, &clientaddr.sin_addr, addr, INET_ADDRSTRLEN));
        printf("------> n: %zd \n",n);
        total += n;
        if (n == -1) {
            perror("Receive File Error");
            exit(1);
        }
        if (fwrite(buff, sizeof(char), n, fp) != n) {
            perror("Write File Error");
            exit(1);
        }
        memset(buff, 0, MAX_LINE);
        if (total == 1048576) {
            end=clock();
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
            start=clock();
            printf("Receive Success, NumBytes = %ld,time = %f \n", total,cpu_time_used);
            avg+=cpu_time_used;
            total = 0;
            j++;
            fclose(fp);
            n=0;
        }
        if(j==5){
            return;
        }

    }


}