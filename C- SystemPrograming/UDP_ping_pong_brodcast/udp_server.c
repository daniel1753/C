/*********************************
 * Reviewer:  		
 * Author: Daniel Binyamin			
 * File: udp_server.c					
 * ******************************/

#include "udp.h"

#define PORT 5001

int main() 
{
    char buffer[MAXLINE] = {'\0'};
    struct sockaddr_in servaddr, cliaddr; 
    int sockfd = 0;
    int n;
    socklen_t len ;
    const char* Pong = "daniel1:pong";

    memset(&cliaddr, 0, sizeof(cliaddr));

/*     if((sockfd = InitSocket(PORT,&servaddr , 2)) == -1)
    {
        perror("socket creation failed");
        return -1;
    } */

     /* Creating socket file descriptor */
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);  

   /*  Bind the socket with the server address */
    if ( (bind(sockfd, (const struct sockaddr *)&servaddr, 
            sizeof(servaddr))) < 0 )
    {
        perror("bind failed");
        return -1;
    }

    printf("server....\n");
    printf("reached recvfrom sokfd: %d\n", sockfd);

    while (1)
    {
    
        if( (n = recvfrom(sockfd, (char*)buffer, MAXLINE, 
                    MSG_WAITALL, (struct sockaddr*)&cliaddr,
                    &len)) != -1)
        {
            printf("%s\n", buffer);
            printf("printed!\n");
        }
            printf("printed2!\n");

    }

        sendto(sockfd, Pong, strlen(Pong),
            MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
                sizeof(cliaddr));


   
    close(sockfd);
    
 
       
    return 0;
}
