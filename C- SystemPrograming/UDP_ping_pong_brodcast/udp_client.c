/*********************************
 * Reviewer:  				
 * Author: Daniel Binyamin			
 * File: UDP_Client.c				
 * ******************************/

#include "udp.h"
#define PORT 5001


int main() 
{

    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;   
    int sockfd = 0;
    int n , premmition =1;
    socklen_t len;
    const char* Ping = "daniel1:ping";


    if((sockfd = InitSocket(PORT,&servaddr , 1)) == -1)
    {
        perror("socket creation failed");
        return -1;
    }

  /*  if((setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST, (void*)&premmition, 
                    sizeof(premmition)))==-1)
    {
        perror("setsockopt creation failed");
        return -1;

    } */

    printf("client....\n");
    sendto(sockfd, Ping, strlen(Ping),
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
            sizeof(servaddr));

    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);
    buffer[n] = '\0';
    printf("%s\n", buffer);

           


    
    return 0;
}