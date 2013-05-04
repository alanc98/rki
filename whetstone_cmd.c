#include <stdio.h>
#include <string.h>

extern int whetstonemain(int argc, char **argv);

/*
** Whetstone Command
*/
int whetstone_command( int argc, char *argv[])
{
   int n;
   char *args[]= {"whetstone", "5", "" };

   printf("Running Whetstone Command!\n");
   while( strlen(args[n]) ) n++;
   whetstonemain(n, args);
   printf("Completed Whetstone\n");
   
   return(0);
}

