#include <stdio.h>
#include <string.h>

/*
** Hello RTEMS!
*/
int hello_command( int argc, char *argv[])
{
   printf("Hello RTEMS!\n");
   printf("Create your own command here!\n");

   return(0);
}

