/*
** RTEMS Kernel Image shell command setup
**
*/
#include "rki_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <bsp.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#include <rtems/shell.h>
#include <rtems/bdbuf.h>
#include <rtems/error.h>

/*
** External functions
*/

int rtems_init_network(char *EthernetAddress,
                       char *IPAddress,
                       char *HostName,
                       char *NetMask,
                       char *GateWay,
                       char *NameServer );

void rtems_start_network_services(void);
int hello_command( int argc, char *argv[]);
int task_command( int argc, char *argv[]);
int dhrystone_command( int argc, char *argv[]);
int whetstone_command( int argc, char *argv[]);
int shd_test_command( int argc, char *argv[]);
int sensehat_HTS221_test_command(int argc, char *argv[]);


/*
** Net Setup Command
*/
int net_setup_command( int argc, char *argv[])
{
   /*
   ** Check args
   **   Ethernet address
   **   IP Address
   **   Netmask
   */
   if (argc != 7)
   {
       printf("Command Usage: netsetup <EthernetAddr> <IPaddress> <Hostname> <Netmask> <Gateway> <Nameserver>\n");
       return(-1);
   }
   else
   {
      printf ("Ethernet Address =  %s\n", argv[1]);
      printf ("IP Address =  %s\n", argv[2]);
      printf ("Host Name =  %s\n", argv[3]);
      printf ("Netmask =  %s\n", argv[4]);
      printf ("Gateway =  %s\n", argv[5]);
      printf ("Nameserver =  %s\n", argv[6]);

      rtems_init_network( argv[1], argv[2], argv[3], argv[4], argv[5], argv[6] );

      #ifdef RKI_INCLUDE_NETWORK_SERVICES
         /*
         ** Start network services
         */
         rtems_start_network_services();
      #endif
   }
   return(0);
}

/*
**
** Start the RTEMS Shell.
*/
void shell_start ( void )
{

   rtems_status_code sc;

   printf ("Starting shell....\n\n");

   sc = rtems_shell_init ("shell0", 20 * 1024, 100, "/dev/console", 0, 1,NULL);
   if (sc != RTEMS_SUCCESSFUL)
   {
      printf ("error: starting shell: %s (%d)\n", rtems_status_text(sc), sc);
   }
}

/*
**
** Run the /shell-init script.
*/
void shell_init_script (void)
{
  rtems_status_code sc;
  printf ("Running /shell-init.\n\n");
  sc = rtems_shell_script ("initscr",
                            60 * 1024,    /* Stack size */
                            20,           /* Priority */
                            RKI_SHELL_INIT, /* the Script file to run */
                            "stdout",      /* Where to redirect the output */
                            0,            /* Run once and exit */
                            1,            /* Wait ? */
                            1);           /* Verbose/echo */

  if (sc != RTEMS_SUCCESSFUL)
    printf ("error: running shell script: %s (%d)\n", rtems_status_text (sc), sc);
}

/*
** function to start the shell and add new commands.
*/
int rtems_add_local_cmds(void)
{
   /*
   ** Add commands
   */
   rtems_shell_add_cmd("netsetup","misc","setup and start networking",net_setup_command);

   rtems_shell_add_cmd("hello","misc","Say hello RTEMS!",hello_command);

   rtems_shell_add_cmd("taskdemo","misc","run a set of tasks",task_command);

   rtems_shell_add_cmd("dhrystone","misc","Run the Dhrystone Benchmark",dhrystone_command);

   rtems_shell_add_cmd("whetstone","misc","Run the Whetstone Benchmark",whetstone_command);

   rtems_shell_add_cmd("sensed","misc","Test the Sense HAT Display",shd_test_command);

   rtems_shell_add_cmd("senseh", "misc", "Test the Sense HAT LTS221 Humidity/Temp Sensor", sensehat_HTS221_test_command);

   shell_init_script();

   /*
   ** Setup the shell
   */
   shell_start ();

   return(0);

}
