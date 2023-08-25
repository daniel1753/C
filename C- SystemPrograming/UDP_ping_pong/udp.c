/*********************************
 * Reviewer:  				
 * Author: Daniel Binyamin			
 * File: udp.c				
 * ******************************/

#include "udp.h"


int InitSocket(int port_num ,struct sockaddr_in * servaddr)
{
    int sockfd;

    /* Creating socket file descriptor */
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        return -1;
    }

    memset(servaddr, 0, sizeof(servaddr));

    /* Filling server information */
    servaddr->sin_family    = AF_INET; /* IPv4 */
    servaddr->sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr->sin_port = htons(port_num);
    
    return sockfd;

}



/* void Send(int sockfd,const char* massage, size_t mass_len, int Flag_Client)
{
   
       sendto(sockfd, massage, mass_len, 
        MSG_CONFIRM, sock_addr,len);
}


void Recv(int sockfd, int Flag_Client)
{

   struct sockaddr *sock_addr;

    if(Flag_Client)
    {
        sock_addr = &servaddr;
    }
    else
    {
        sock_addr = &cliaddr;
    }

    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);
    buffer[n] = '\0';
    printf("Server :%s\n" ,buffer);

    close(sockfd);
}



ServerRecv()
{
    len = sizeof(cliaddr);  
   
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, ( struct sockaddr *) &cliaddr,
                &len);
    buffer[n] = '\0';
    printf("Client : %s\n", buffer);
} */