/*********************************
 * Reviewer:  				
 * Author: Daniel Binyamin			
 * File: udp.c				
 * ******************************/

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXLINE 1024

int InitSocket(int port_num ,struct sockaddr_in * servaddr);
