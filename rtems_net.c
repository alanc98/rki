/****************************************************************
**  rtems_net.c
**
**  Author:  Alan Cudmore
**
**  This module is responsible for RTEMS specific network initialization.
**
**
*/
#include "rki_config.h"

/*
** If the Network subsystem is not selected, just ifdef the whole thing out
*/
#ifdef RKI_INCLUDE_NETWORK

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

/*
** include network configuration
*/
#include "rtems_net_config.h"

/*
** Declare the network configuration structure
*/
struct rtems_bsdnet_config	rtems_bsdnet_config;

/*
** Loopback interface config
*/
struct rtems_bsdnet_ifconfig	loopback_config __attribute__((aligned(16)));
extern int rtems_bsdnet_loopattach(void * dummy);

/*
** ethernet interface config
*/
struct rtems_bsdnet_ifconfig	if1_config     __attribute__((aligned(16)));
extern int RTEMS_BSP_NETWORK_DRIVER_ATTACH (struct rtems_bsdnet_ifconfig *, int);

/*
** Ethernet address
*/
static   char ethernet_address[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


/*
** Convert an ethernet address string
** from a form of "00:00:00:00:00:00" to a binary byte array.
*/
int rtems_convert_ethernet_addr( char ethernet_address[], char *AddressString )
{
   int  result;
   int  i;

   unsigned int temp_eth_address[6];

   printf("Convert ethernet address\n");

   /*
   ** Do some basic format checking
   */
   if ( strlen(AddressString) != 17 )
   {
      printf("Error: Invalid MAC Address: %s\n",AddressString);
      return(-1);
   }
   if ( AddressString[2] != ':' || AddressString[5] != ':' ||
        AddressString[8] != ':' || AddressString[11] != ':' ||
        AddressString[14] != ':' )
   {
      printf("Error: Invalid MAC Address: %s\n",AddressString);
      return(-1);
   }

   printf("Converting: %s\n",AddressString);

   result = sscanf(AddressString, "%2x:%2x:%2x:%2x:%2x:%2x",
         &temp_eth_address[0],
         &temp_eth_address[1],
         &temp_eth_address[2],
         &temp_eth_address[3],
         &temp_eth_address[4],
         &temp_eth_address[5]);
   if ( result < 6 )
   {
      printf("Error: Invalid MAC Address: %s\n",AddressString);
      return(-1);
   }

   /*
   ** fill byte array
   */
   for ( i = 0; i < 6; i++ )
   {
      ethernet_address[i] = temp_eth_address[i] & 0xFF;
   }

   return 0;

}

/*
** rtems_init_network
** Initialize the network subsystem and devices for RTEMS
**
*/
int rtems_init_network(char *EthernetAddress,
                       char *IPAddress,
                       char *HostName,
                       char *NetMask,
                       char *GateWay,
                       char *NameServer )
{

   int                           status;
   struct rtems_bsdnet_ifconfig	*if1;

   printf("Starting RTEMS network configuration\n");

   /*
   ** Clear out the 3 config structures
   */
   memset(&rtems_bsdnet_config,0,sizeof(struct rtems_bsdnet_config));
   memset(&loopback_config,  0, sizeof(struct rtems_bsdnet_ifconfig));
   memset(&if1_config, 0, sizeof(struct rtems_bsdnet_ifconfig));

   /*
   ** Fill out the network config structure
   */
   rtems_bsdnet_config.network_task_priority    = 1;
   rtems_bsdnet_config.mbuf_bytecount           = 64*1024; /* Was 128K */
   rtems_bsdnet_config.mbuf_cluster_bytecount   = 256*1024;

   rtems_bsdnet_config.hostname =                 HostName;
   rtems_bsdnet_config.domainname =               RTEMS_NET_DOMAINNAME;
   rtems_bsdnet_config.gateway =                  GateWay;
   rtems_bsdnet_config.name_server[0]=            NameServer;

   rtems_bsdnet_config.ifconfig = &loopback_config;

   /*
   ** Configure the loopback device structure
   */
   loopback_config.name =        "lo0";
   loopback_config.attach =      (int (*)(struct rtems_bsdnet_ifconfig *, int))rtems_bsdnet_loopattach;
   loopback_config.ip_address=   "127.0.0.1";
   loopback_config.ip_netmask=   "255.0.0.0";
   loopback_config.next =        &if1_config;

   /*
   ** Configure the ethernet device structure
   */
   if1_config.name =       RTEMS_BSP_NETWORK_DRIVER_NAME;
   if1_config.attach =     RTEMS_BSP_NETWORK_DRIVER_ATTACH;
   if1_config.ip_address=  IPAddress;
   if1_config.ip_netmask=  NetMask;
   if1_config.rbuf_count = 32;
   if1_config.xbuf_count = 32;
   if1_config.next = NULL;

   printf("RTEMS network config: All structures filled out, now calling init functions\n");


   #if (defined (RTEMS_SET_ETHERNET_ADDRESS))

      /*
      ** Convert the ethernet address string to the byte array
      */
      status = rtems_convert_ethernet_addr(ethernet_address, EthernetAddress );

      if ( status == 0 )
      {
         if1_config.hardware_address = ethernet_address;
      }
   #endif

   /*
   ** Initialize the network
   */
   printf("Calling rtems_bsdnet_initialize_network\n");

   if( rtems_bsdnet_initialize_network() == -1 )
   {
      printf("RTEMS Network Initialization failed\n");
      return(1);
   }
   else
   {
      printf("RTEMS bsdnet_initialize_network returned OK\n");
   }

   printf("Hostname is %s.%s\n", rtems_bsdnet_config.hostname, rtems_bsdnet_config.domainname );

   /*
   ** Loop through the available devices and print the stats
   */
   for( if1 = &loopback_config; if1; if1 = if1->next )
   {
      printf("   Device %s, netmask %s, address %s\n", if1->name, if1->ip_netmask, if1->ip_address );
   }

   /*
   ** Printout the gateway and DNS server(s)
   */
   printf("   gateway %s, dns1 %s, dns2 %s\n\n\n", rtems_bsdnet_config.gateway,
                                                   rtems_bsdnet_config.name_server[0],
                                                   rtems_bsdnet_config.name_server[1] );
   return(0);
}

/*
** rtems_auto_init_network
** This function is called from the RTEMS Init function if
** the user wishes to setup a static network address without
** using the shell configuration.
*/
int rtems_auto_init_network(void)
{
   int status;

   status = rtems_init_network(RTEMS_ETHERNET_ADDRESS,
                               RTEMS_NET_IP_ADDRESS,
                               RTEMS_NET_HOSTNAME,
                               RTEMS_NET_IP_NETMASK,
                               RTEMS_NET_GATEWAY,
                               RTEMS_NET_NAME_SERVER1);
   return(status);
}
#else

/*
** no network config!
*/
int rtems_init_network( void )
{
   return(0);
}

#endif



