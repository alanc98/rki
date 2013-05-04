/*
 * RTEMS configuration/initialization
 *
 * Based on code by W. Eric Norum and others
 *
 * This program may be distributed and used for any purpose.
 * I ask only that you:
 *	1. Leave this author information intact.
 *	2. Document any changes you make.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 */

#include "rki_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

#include "rtems_config.h"

#ifdef RKI_INCLUDE_TARFS
   /*
   ** Tar file symbols
   */
   extern int _binary_tarfile_start;
   extern int _binary_tarfile_size;
   #define TARFILE_START _binary_tarfile_start
   #define TARFILE_SIZE _binary_tarfile_size
#endif

/*
** External functions
*/
int allocate_disk_ram ( void );
int rtems_auto_init_network( void );
int rtems_add_local_cmds(void);
int setup_filesystems (void);
void start_network_services(void);

/*
** RTEMS Startup Task
*/
rtems_task Init (rtems_task_argument ignored)
{
   int status;

   printf("\n\n");
   printf("RTEMS Kernel Image Booting\n");
   printf( "\n\n*** RTEMS Info ***\n" );
   printf("%s", _Copyright_Notice );
   printf("%s\n\n", _RTEMS_version );
   printf("\n");
   printf(" BSP Ticks Per Second = %d\n",(int)rtems_clock_get_ticks_per_second());
   printf( "*** End RTEMS info ***\n\n" );

#ifdef RKI_INCLUDE_NVRAMDISK
   /*
   ** Allocate the RAM from the heap for the disks
   ** This is done early here to grab a large block of memory
   ** before it gets fragmented.
   */
   status = allocate_disk_ram();
   if ( status != 0 )
   {
      printf("Could not allocate RAM for disks\n");
   }
#endif

#ifdef RKI_INCLUDE_TARFS
   /*
   ** Initialize the file system
   */
   printf("Populating Root file system from TAR file.\n");
   status = Untar_FromMemory(
                (unsigned char *)(&TARFILE_START),
		(unsigned long)&TARFILE_SIZE);
#endif

#ifdef RKI_INCLUDE_NETWORK
   /*
   ** Initialize the network subsystem and daemons
   */
   #ifdef RKI_AUTO_INIT_NETWORK
      /*
      **  Automatically init the network.
      **  Otherwise it is assumed that it's driven from a script
      */
      status = rtems_auto_init_network();
   #endif
#endif

#ifdef RKI_INCLUDE_NETWORK_SERVICES
      /*
      ** Start network services
      */
      rtems_start_network_services();
#endif

   /*
   ** Setup the filesystems and commands
   */
   printf("Setting up filesystems.\n");
   status = setup_filesystems();

#ifdef RKI_INCLUDE_SHELL
   /*
   ** Setup the RTEMS shell and add local commands
   */
   printf("Initializing Local Commands.\n");
   status = rtems_add_local_cmds();
#endif

   /*
   ** Delete the init task
   */
   printf("Ending the RTEMS Init task.\n");
   status = rtems_task_delete( RTEMS_SELF );    /* should not return */
   printf( "rtems_task_delete returned with status of %d.\n", status );
   exit( 1 );
}
