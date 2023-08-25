/*********************************
 * Reviewer:  		
 * Author: Daniel Binyamin			
 * File: udp_server.c					
 * ******************************/

#include "udp.h"

#define PORT 5000

int main() 
{
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;   
    int sockfd = 0;
    int n;
    socklen_t len;
    const char* Pong = "pong";

    memset(&cliaddr, 0, sizeof(cliaddr));

    if((sockfd = InitSocket(PORT,&servaddr)) == -1)
    {
        perror("socket creation failed");
        return -1;
    }

   /*  Bind the socket with the server address */
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        return -1;
    }


    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, (struct sockaddr *) &cliaddr,
                &len);
    buffer[n] = '\0';
    printf("%s\n", buffer);

    sendto(sockfd, Pong, strlen(Pong),
        MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
            sizeof(cliaddr));
           

   
    close(sockfd);
    
 
       
    return 0;
}
