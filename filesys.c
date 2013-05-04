/*
 *  filesys.c 
 *
 * RTEMS Project (http://www.rtems.org/)
 *
 * Copyright 2007 Chris Johns (chrisj@rtems.org)
 */

/*
 * File-system block driver test program.
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
#include <rtems/ramdisk.h>
#include <rtems/shell.h>
#include <rtems/nvdisk-sram.h>
#include <librtemsNfs.h>

/*
** Defines
*/


/*
** For the file write test command
*/
#define FILENAME_BASE "/test_file"

/*
** External functions
*/
int setup_ramdisk (void);

/*
** Internal functions
*/
int ReadTestCmd( int argc, char *argv[]);
int WriteTestCmd( int argc, char *argv[]);


/*
** Global Data
*/


/*
** A file read/write buffer for the test commands
*/
char file_buffer[8192];

#ifdef RKI_INCLUDE_NFSCLIENT
int shell_fs_nfsmount (int argc, char* argv[])
{
  int status;

  if ( argc < 4 )
  {
     printf("nfsmount error: required parameter missing.\n");
     printf("     nfsmount <server> <server_path> <client_mount_point>\n");
     return(-1);
  }

  /* This code uses the NFS mount wrapper function */
  status = nfsMount( argv[1], argv[2], argv[3] );

  return status;
}
#endif

int setup_filesystems (void)
{
  int ret;

#ifdef RKI_INCLUDE_RAMDISK
  ret = setup_ramdisk ();
  if (ret)
    return (ret);
#endif

#ifdef RKI_INCLUDE_NVRAMDISK
  ret = setup_nvdisk ();
  if (ret)
    exit (ret);
#endif

#ifdef RKI_INCLUDE_NFSCLIENT
  rtems_shell_add_cmd ("nfsmount", "misc","nfsmount", shell_fs_nfsmount);
#endif

  if ( rtems_shell_add_cmd("write_test","misc","Test a File System by writing files",WriteTestCmd) == NULL)
   {
      printf("Error: Could not add write_test command\n");
   }

   if ( rtems_shell_add_cmd("read_test","misc","Test a File System by reading files",ReadTestCmd) == NULL)
   {
      printf("Error: Could not add read_test command\n");
   }

  return 0;
}

/*
** File Write Test command
*/
int WriteTestCmd( int argc, char *argv[])
{
   char   volume_path[32];
   char   file_name[32];
   char   file_num_string[5];
   int    file_size;
   int    num_files;
   int    write_size;
   int    priority;
   size_t size_written;
   int    done = 0;
   int    fd;
   int    i;
   int    j;

   if (argc != 6 )
   {
       printf("Command Usage: write_test volume_path file_size num_files write_size priority\n");
       printf("               volume_path = the volume to write the files to. Ex: /O1/S1\n");
       printf("               file_size = the size in bytes to make each test file\n");
       printf("               num_files = the number of test files to create\n");
       printf("               write_size = the number of bytes to try to write at once\n");
       printf("               priority = the priority of the task spawned to write the files\n");
       return(-1);
   }
   else
   {
       printf("Disk Write test\n");
       strncpy(volume_path, argv[1], 32 );
       file_size = atoi(argv[2]);
       num_files = atoi(argv[3]);
       write_size = atoi(argv[4]);
       priority = atoi(argv[5]);

       printf("Chosen parameters: volume_path = %s, file_size = %d, num_files = %d, write_size = %d, priority = %d\n",
       volume_path, (int)file_size, (int)num_files, (int)write_size, (int)priority);

       for ( i = 0; i < num_files; i++ )
       {
          if ( done == 1 )
          {
             break;
          }

          strncpy(file_name, volume_path, 32);
          strncat(file_name, FILENAME_BASE, 32);
          sprintf(file_num_string, "%d",i);
          strncat(file_name, file_num_string, 5 );
          printf("File: %s\n",file_name);

          fd = open(file_name, O_CREAT | O_WRONLY, S_IRWXU );

          if ( fd < 0 )
          {
             printf("Error: Cannot Open %s for writing\n",file_name);
             return(-1);
          }

          for ( j = 0; j < file_size; j+= write_size )
          {
             size_written = write(fd, file_buffer, write_size);
             if ( size_written != write_size )
             {
                printf("Error: write failed. Aborting file creation\n");
                done = 1;
                break;
             }
          }
          close(fd);
       }
   }

   return(0);
}

int ReadTestCmd( int argc, char *argv[])
{
   char   file_name[32];
   int    read_size;
   int    priority;
   size_t size_read;
   int    done = 0;
   int    fd;

   if (argc != 4 )
   {
       printf("Command Usage: read_test read_file read_size priority\n");
       printf("               read_file = The path and file to read. Ex: /O1/S1/test_file01\n");
       printf("               read_size = the size in bytes for each read.\n");
       printf("               priority = the priority of the task spawned to write the files\n");
       return(-1);
   }
   else
   {
       printf("Disk Read test\n");
       strncpy(file_name, argv[1], 32 );
       read_size = atoi(argv[2]);
       priority = atoi(argv[3]);

       printf("Chosen parameters: read_file = %s, read_size = %d, priority = %d\n",
                                  file_name, (int)read_size, (int)priority);

       fd = open(file_name, O_RDONLY, S_IRWXU );

       if ( fd < 0 )
       {
          printf("Error: Cannot Open %s for writing\n",file_name);
          return(-1);
       }

       while ( done == 0 )
       {
          size_read = read(fd, file_buffer, read_size);
          if ( size_read != read_size )
          {
             done = 1;
          }
       }
       close(fd);
   }
   return(0);
}

