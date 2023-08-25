/*********************************
 * Reviewer:  				
 * Author: Daniel Binyamin			
 * File: tcp.c				
 * ******************************/


#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> 
#include <arpa/inet.h> 

#define PORT 5001
#define MAXLINE 1024
#define MAX 80
#define SA struct sockaddr

int InitSocket(int port_num ,struct sockaddr_in * servaddr,  int addr);
void s_talk(int connfd);
void c_talk(int sockfd);

