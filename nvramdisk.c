/*
 *  nvramdisk.c
 *
 * RTEMS Project (http://www.rtems.org/)
 *
 * Copyright 2007 Chris Johns (chrisj@rtems.org)
 */

/*
** File-system block driver test program.
*/

#include "rki_config.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <rtems.h>
#include <rtems/bdbuf.h>
#include <rtems/blkdev.h>
#include <rtems/diskdevs.h>
#include <rtems/dosfs.h>
#include <rtems/error.h>
#include <rtems/flashdisk.h>
#include <rtems/fsmount.h>
#include <rtems/shell.h>
#include <rtems/nvdisk-sram.h>

/*
** Let the IO system allocation the next available major number.
*/
#define RTEMS_DRIVER_AUTO_MAJOR (0)

/*
** Variable to hold the addresses of the memory pools for
** the NVRAM disk. The memory for the disk is allocated in the early
** int from the heap so it will be available as one chunk.
*/
uint32_t rki_nvdisk_base;

/*
** The NV Device descriptor. For this test it is just DRAM.
*/
rtems_nvdisk_device_desc rtems_nv_heap_device_descriptor[] =
{
  {
    flags:  0,
    base:   0, /* The memory address is assigned at runtime */
    size:   RKI_NVRAMDISK_SIZE,
    nv_ops: &rtems_nvdisk_sram_handlers
  },
};

/**
 * The NV Disk configuration.
 */
const rtems_nvdisk_config rtems_nvdisk_configuration[] =
{
  {
    block_size:         512,
    device_count:       1,
    devices:            &rtems_nv_heap_device_descriptor[0],
    flags:              0,
    info_level:         0
  },
};

/*
** The number of NV Disk configurations.
*/
uint32_t rtems_nvdisk_configuration_size = 1;

/*
 * Create the NV Disk Driver entry.
*/
rtems_driver_address_table rtems_nvdisk_io_ops = {
  initialization_entry: rtems_nvdisk_initialize,
  open_entry:           rtems_blkdev_generic_open,
  close_entry:          rtems_blkdev_generic_close,
  read_entry:           rtems_blkdev_generic_read,
  write_entry:          rtems_blkdev_generic_write,
  control_entry:        rtems_blkdev_generic_ioctl
};

#ifdef RKI_INCLUDE_NVRAMDISK

/*
** Allocate the disk RAM
*/
int allocate_disk_ram(void)
{
  rki_nvdisk_base = (uint32_t) malloc (RKI_NVRAMDISK_SIZE);
  if (!rki_nvdisk_base)
  {
    printf ("error: no memory for NV disk\n");
    return 1;
  }
  else
  {
    printf("Allocated memory for NVRAM Disk\n");
  }

  return(0);
}

int shell_nvram_erase (int argc, char* argv[])
{
  const char* driver = NULL;
  int         arg;
  int         fd;

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      printf ("error: invalid option: %s\n", argv[arg]);
      return 1;
    }
    else
    {
      if (!driver)
        driver = argv[arg];
      else
      {
        printf ("error: only one driver name allowed: %s\n", argv[arg]);
        return 1;
      }
    }
  }

  printf ("erase nvram disk: %s\n", driver);

  fd = open (driver, O_WRONLY, 0);
  if (fd < 0)
  {
    printf ("error: nvram driver open failed: %s\n", strerror (errno));
    return 1;
  }

  if (ioctl (fd, RTEMS_NVDISK_IOCTL_ERASE_DISK) < 0)
  {
    printf ("error: nvram driver erase failed: %s\n", strerror (errno));
    return 1;
  }

  close (fd);

  printf ("flash nvram erased successful\n");

  return 0;
}

int setup_nvdisk (void)
{
  rtems_device_major_number major;
  rtems_status_code         sc;

  /*
  ** check to see if the RAM is allocated before initializing the driver
  */
  if ( rki_nvdisk_base == 0 )
  {
     printf("Cannot setup NVRAM disk, memory was not allocated\n");
     return(1);
  }
  /*
  ** Assign the RAM allocated
  */
  rtems_nv_heap_device_descriptor[0].base = rki_nvdisk_base;

  /*
  ** Register the NVRAM Disk driver.
  */
  printf ("Register NV Disk Driver: ");
  sc = rtems_io_register_driver (RTEMS_DRIVER_AUTO_MAJOR,
                                 &rtems_nvdisk_io_ops,
                                 &major);
  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: nvdisk driver not initialised: %s\n",
            rtems_status_text (sc));
    return 1;
  }

  rtems_shell_add_cmd ("nverase", "misc", "nverase driver", shell_nvram_erase);

  printf ("successful\n");

  return 0;
}

#endif
