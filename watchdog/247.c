/*********************************
 * Reviewer: 			
 * Author: Daniel Binyamin			
 * File: 247.c					
 * ******************************/
#include <unistd.h> /* sleep */

#include "wd.h" /* MMI , DNR */

int main(int argc, char *argv[])
{
    int status = 1;
    admin_t *admin;
    int i =0;
    int sleep_time = 15;

    /* init  */
    printState("247 main",getpid(), getppid());
    if( !(admin = MMI(argc, argv[0])))
    {
        return 1;
    }


    while(sleep_time > 0)
    {
        sleep_time = sleep(sleep_time);
    }

    printState("247: >>>>>>>>>>DNR activated>>>>>>>>>>\n", getpid(), getppid());

    DNR(admin);
    
    printf("247: ended --------------------------------\n");


    return 0;
}