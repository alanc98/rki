/*  
** rtems_net_config.h
**
**  Author:  Alan Cudmore
**
**  This contains the network settings for an RTEMS Application
**
*/

/*
** Define the network driver name and Attach routine
** These may be already defined in bsp.h
*/
#ifndef RTEMS_BSP_NETWORK_DRIVER_NAME
   #define RTEMS_BSP_NETWORK_DRIVER_NAME    "fe0"
#endif

#ifndef RTEMS_BSP_NETWORK_DRIVER_ATTACH
   #define RTEMS_BSP_NETWORK_DRIVER_ATTACH   rtems_fec_driver_attach
#endif

#define RTEMS_NET_DOMAINNAME    "local"

/*
** Define the hostname, IP address, etc for the Main ethernet interface
**   These parameters can be set through the "netsetup" shell command
**   if desired. These are the defaults.
*/
#define RTEMS_NET_HOSTNAME      "rtems_target"
#define RTEMS_NET_IP_ADDRESS    "192.168.0.8"
#define RTEMS_NET_IP_NETMASK    "255.255.255.0"
#define RTEMS_NET_GATEWAY       "192.168.0.1"
#define RTEMS_NET_NAME_SERVER1  "192.168.0.1"

/*
 * Define RTEMS_SET_ETHERNET_ADDRESS if you want to specify the
 * Ethernet address here.  If RTEMS_SET_ETHERNET_ADDRESS is not
 * defined the driver will choose an address.
 */
#define RTEMS_SET_ETHERNET_ADDRESS
#define RTEMS_ETHERNET_ADDRESS  "00:04:9F:00:5B:21"


