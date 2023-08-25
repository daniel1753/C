/*********************************
 * Reviewer:  				
 * Author: Daniel Binyamin			
 * File: UDP_Client.c				
 * ******************************/

#include "udp.h"
#define PORT 5002

int main() 
{

    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;   
    int sockfd = 0;
    int n;
    socklen_t len;
    const char* Ping = "ping";


    if((sockfd = InitSocket(PORT,&servaddr)) == -1)
    {
        perror("socket creation failed");
        return -1;
    }

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