/*
 *  ramdisk.c
 *
 * RTEMS Project (http://www.rtems.org/)
 *
 * Copyright 2007 Chris Johns (chrisj@rtems.org)
 */

#include "rki_config.h"

#include <rtems/ramdisk.h>
#include <rtems/blkdev.h>
#include <rtems/libio.h>

#ifdef RKI_INCLUDE_RAMDISK

#define  RAMDISK_BLOCK_SIZE  (512)
#define  RAMDISK_BLOCK_COUNT  RAM_DISK_0_BLOCKS 
#define  RAMDISK_PATH "/dev/ramdisk"

dev_t dev = 0;

int setup_ramdisk (void)
{
  int rc = 0;
  rc = rtems_disk_io_initialize ();
  if ( rc == RTEMS_SUCCESSFUL )
  {
     
     rc = ramdisk_register (RAMDISK_BLOCK_SIZE, RAMDISK_BLOCK_COUNT,
                         false, RAMDISK_PATH, &dev);
	 if ( rc == RTEMS_SUCCESSFUL )
     {  
        return(0);
     }
     else
     {
        return(rc);
     }		
  }
  else
  {
     return(rc); 
  }

}
#endif


