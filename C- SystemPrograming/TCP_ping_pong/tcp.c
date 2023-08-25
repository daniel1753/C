/*********************************
 * Reviewer:  				
 * Author: Daniel Binyamin			
 * File: tcp.c				
 * ******************************/

#include "tcp.h"


int InitSocket(int port_num ,struct sockaddr_in * servaddr ,int addr)
{
    int sockfd;

    /* Creating socket file descriptor */
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("socket creation failed");
        return -1;
    }
    else
    {    
        printf("Socket successfully created..\n");
    }
    memset(servaddr,0, sizeof(*servaddr));

    /* Filling server information */
    servaddr->sin_family = AF_INET; /* IPv4 */
    servaddr->sin_addr.s_addr = htonl(addr) ;
    servaddr->sin_port = htons(port_num);
    return sockfd;

}

void s_talk(int connfd)
{
    char buff[80];
    int i=0;
    for(;i < 10000000;++i)
    {
        bzero(buff, 80);

        read(connfd, buff, strlen(buff)+1);

        printf("server :**********%s\n", buff);
        bzero(buff, 80);

        write(connfd, "Pong", strlen("Pong") +1);
    }
}

void c_talk(int sockfd)
{
    char buff[MAXLINE]= {'\0'};
    int status = 1;
    
    while(status<= 3)
    {
        sleep(5);
        write(sockfd, "Ping", sizeof("Ping"));
            
        memset(buff,0,MAXLINE);
        read(sockfd, buff, 79);
        printf("server :**********%s , %d\n", buff, status);
        ++status;
    }
}


