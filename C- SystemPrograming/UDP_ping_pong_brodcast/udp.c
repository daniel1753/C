/*********************************
 * Reviewer:  				
 * Author: Daniel Binyamin			
 * File: udp.c				
 * ******************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "udp.h"

int stohi(char *ip){
	char c;
	unsigned int integer;
	int val;
	int i,j=0;
	c = *ip;
	for (j=0;j<4;j++) {
		if (!isdigit(c)){  
			return (0);
		}
		val=0;
		for (i=0;i<3;i++) {
			if (isdigit(c)) {
				val = (val * 10) + (c - '0');
				c = *++ip;
			} else
				break;
		}
		if(val<0 || val>255){
			return (0);	
		}	
		if (c == '.') {
			integer=(integer<<8) | val;
			c = *++ip;
		} 
		else if(j==3 && c == '\0'){
			integer=(integer<<8) | val;
			break;
		}
			
	}
    	if(c != '\0'){
		return (0);	
	}
	return (htonl(integer));
    
}

int InitSocket(int port_num ,struct sockaddr_in *servaddr,int flag)
{
    int sockfd;
    int n , premmition =1;

    /* Creating socket file descriptor */
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        return -1;
    }

    memset(servaddr, 0, sizeof(*servaddr));

    servaddr->sin_family = AF_INET; 
    servaddr->sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr->sin_port = htons(port_num);  
    
    return sockfd;

}
