/*
** rki_config.h
**
**  Author:  Alan Cudmore
**
**  This contains the configuration settings for an RTEMS Application
**
*/

/*
** Basic configuration settings for the RTEMS Kernel Image ( RKI )
*/

/*
** Include the RTEMS shell and startup script?
*/
#define RKI_INCLUDE_SHELL

/*
** Define the shell init script to run
*/
#define RKI_SHELL_INIT "/shell-init"

/*
** Include the TAR file system to initialze IMFS
** This is used to setup the base directories and startup scripts
*/
#define RKI_INCLUDE_TARFS


/*
** Include the network stack/driver
*/
#undef RKI_INCLUDE_NETWORK

/*
** Auto-config the network based on the values in rtems_net_config.h
** Otherwise use the netsetup shell command to init the network
*/
#undef RKI_AUTO_INIT_NETWORK

/*
** start network services. Enable/disable individual services below
*/
#undef RKI_INCLUDE_NETWORK_SERVICES

/*
** Include and start the telnetd server
*/
#undef RKI_INCLUDE_TELNETD_SERVER

/*
** Include and start the RTEMS ftp server
*/
#undef RKI_INCLUDE_FTPSERVER

/*
** Include the NFS client
*/
#undef RKI_INCLUDE_NFSCLIENT

/*
** Include support for the NVRAM disk
*/
#undef RKI_INCLUDE_NVRAMDISK

/*
** Define NVRAM Disk base address and size
*/
#define RKI_NVRAMDISK_BASE_ADDR  0x0
#define RKI_NVRAMDISK_SIZE       128 * 1024

/*
** Include support for the "regular" RAM disk
*/
#define RKI_INCLUDE_RAMDISK

/*
** Define RAM Disk 0 size
*/
#define RAM_DISK_0_BLOCKS    128 * 1024

/*
** Include the TFTP file system
*/
#undef RKI_INCLUDE_TFTPFS

