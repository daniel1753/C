/*********************************
 * Reviewer: 			
 * Author: Daniel Binyamin			
 * File: wd.c					
 * ******************************/
#define _POSIX_C_SOURCE 200112L       /* sigaction */


#include <stdlib.h> /* envirmant varble   */
#include <stdio.h> /* sprintf   */

#include "wd.h" 


int main(int argc, char *argv[])
{
    int status=1;
    char str_pid[7] ={0};
    admin_t * admin = {0};
    printState("WD main",getpid(), getppid());
    printf("wd main: argv = %s sent to the proc \n", argv[1]);
    sprintf(str_pid, "%d", getpid());
    
    printf("wd main: str_pid = %s\n", str_pid);
    printState("WD main: 2 ",getpid(), getppid());
    setenv(ENV_NAME,str_pid, 1);

    admin = MMI(argc, argv[1]);
    DNR(admin);
   
    return 0;
}