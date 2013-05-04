/****************************************************************
**  rtems_net_svc.c
**
**  Author:  Alan Cudmore
**
**  This module is responsible for RTEMS network services initialization.
**
**
*/
#include "rki_config.h"

/*
** If the Network services are not wanted, just ifdef the whole thing out
*/
#ifdef RKI_INCLUDE_NETWORK_SERVICES

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#include <rtems/rtems_bsdnet.h>
#include <sys/domain.h>
#include <sys/mbuf.h>
#include <sys/socketvar.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/callout.h>
#include <sys/proc.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <vm/vm.h>
#include <arpa/inet.h>

#include <net/netisr.h>
#include <net/route.h>

#include <rtems/libio.h>


#ifdef RKI_INCLUDE_FTPSERVER
   #include <rtems/ftpd.h>
   struct rtems_ftpd_configuration rtems_ftpd_configuration = {
      10,                     /* FTPD task priority            */
      1024,                   /* Maximum buffersize for hooks  */
      21,                     /* Well-known port     */
      NULL                    /* List of hooks       */
   };
#endif

#ifdef RKI_INCLUDE_NFSCLIENT
   #include <librtemsNfs.h>
   #include <rpc/rpc.h>
#endif

#ifdef RKI_INCLUDE_TELNETD_SERVER
  void rtemsShell(
    char *pty_name,
    void *cmd_arg
  )
  {
    rtems_shell_main_loop( NULL );
  }
  #define SHELL_ENTRY rtemsShell
#endif

void rtems_start_network_services(void)
{
   /*
   ** Start the network daemons
   */

   /*
   ** Starting the FTP server
   */
   #ifdef RKI_INCLUDE_FTPSERVER
     printf("Starting the FTP Server.\n");
     rtems_initialize_ftpd();
   #endif

   /*
   ** Starting the telnetd server
   */
   #ifdef RKI_INCLUDE_TELNETD_SERVER
     printf("Starting the telnetd Server.\n");
     rtems_telnetd_initialize(
       SHELL_ENTRY,               /* "shell" function */
       NULL,                      /* no context necessary for echoShell */
       FALSE,                     /* spawn a new thread */
       RTEMS_MINIMUM_STACK_SIZE * 20,  /* shell needs a large stack */
       1,                         /* priority .. we feel important today */
       0                          /* do not ask for password */
      );
   #endif

   /*
   ** Setup the NFS Client
   */
   #ifdef RKI_INCLUDE_NFSCLIENT
       printf("============== Initializing RPC ==============\n");
       status = rpcUdpInit();
       if ( status )
          printf( "RPC UDP Initialization failed\n" );

       printf("============== Initializing NFS Subsystem ==============\n");
       nfsInit( 0, 0 );
   #endif

   /*
   ** Init the TFTP file system
   */
   #ifdef RKI_INCLUDE_TFTPFS
      printf("The TFTP file system can now be mounted like a regular file system\n");
      printf("Separate init is no longer necessary\n");
   #endif
}


#else

   /*
   ** no network services!
   */
   void rtems_start_network_services( void )
   {
      return;
   }

#endif



