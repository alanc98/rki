#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <bsp.h> /* for device driver prototypes */
#include <bsp/i2c.h>
#include <drivers/sensehatdisplay.h>

void MapToHat(uint8_t *output, int outputOffset, uint8_t *input, int inputOffset);
/*
** Global data 
*/
static int    shd_once = 0;
uint8_t       videoBuffer[193];

uint32_t      inputBuffer[64] =
{ 
   0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000, /* Red */
   0x0000FF00, 0x0000FF00, 0x0000FF00, 0x0000FF00, 0x0000FF00, 0x0000FF00, 0x0000FF00, 0x0000FF00, /* green */
   0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF, /* blue */
   0x00FFFF00, 0x00FFFF00, 0x00FFFF00, 0x00FFFF00, 0x00FFFF00, 0x00FFFF00, 0x00FFFF00, 0x00FFFF00, /* Yellow */
   0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, /* I think white */ 
   0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, /* Light blue? */
   0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF, /* Purple ? */
   0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000  /* Black or off */

};

uint32_t      inputBufferFrame1[64] =
{ 
   SHD_RED,  SHD_RED,  SHD_RED,   SHD_RED,    SHD_RED,    SHD_RED,   SHD_RED,  SHD_RED, 
   
   SHD_RED,  SHD_BLUE, SHD_BLUE,  SHD_BLUE,   SHD_BLUE,   SHD_BLUE,  SHD_BLUE, SHD_RED, 
   
   SHD_RED,  SHD_BLUE, SHD_GREEN, SHD_GREEN,  SHD_GREEN,  SHD_GREEN, SHD_BLUE, SHD_RED,
   
   SHD_RED,  SHD_BLUE, SHD_GREEN, SHD_YELLOW, SHD_YELLOW, SHD_GREEN, SHD_BLUE, SHD_RED, 
   
   SHD_RED,  SHD_BLUE, SHD_GREEN, SHD_YELLOW, SHD_YELLOW, SHD_GREEN, SHD_BLUE, SHD_RED,  
   
   SHD_RED,  SHD_BLUE, SHD_GREEN, SHD_GREEN,  SHD_GREEN,  SHD_GREEN, SHD_BLUE, SHD_RED, 
   
   SHD_RED,  SHD_BLUE, SHD_BLUE,  SHD_BLUE,   SHD_BLUE,   SHD_BLUE,  SHD_BLUE, SHD_RED, 
   
   SHD_RED,  SHD_RED,  SHD_RED,   SHD_RED,    SHD_RED,    SHD_RED,   SHD_RED,  SHD_RED  
};

uint32_t      inputBufferFrame2[64] =
{ 
   SHD_BLUE,  SHD_BLUE,  SHD_BLUE,   SHD_BLUE,    SHD_BLUE,    SHD_BLUE,   SHD_BLUE,  SHD_BLUE, 
   
   SHD_BLUE,  SHD_GREEN, SHD_GREEN,  SHD_GREEN,   SHD_GREEN,   SHD_GREEN,  SHD_GREEN, SHD_BLUE, 
   
   SHD_BLUE,  SHD_GREEN, SHD_YELLOW, SHD_YELLOW,  SHD_YELLOW,  SHD_YELLOW, SHD_GREEN, SHD_BLUE,
   
   SHD_BLUE,  SHD_GREEN, SHD_YELLOW, SHD_RED,     SHD_RED,     SHD_YELLOW, SHD_GREEN, SHD_BLUE, 
   
   SHD_BLUE,  SHD_GREEN, SHD_YELLOW, SHD_RED,     SHD_RED,     SHD_YELLOW, SHD_GREEN, SHD_BLUE,  
   
   SHD_BLUE,  SHD_GREEN, SHD_YELLOW, SHD_YELLOW,  SHD_YELLOW,  SHD_YELLOW, SHD_GREEN, SHD_BLUE, 
   
   SHD_BLUE,  SHD_GREEN, SHD_GREEN,  SHD_GREEN,   SHD_GREEN,   SHD_GREEN,  SHD_GREEN, SHD_BLUE, 
   
   SHD_BLUE,  SHD_BLUE,  SHD_BLUE,   SHD_BLUE,    SHD_BLUE,    SHD_BLUE,   SHD_BLUE,  SHD_BLUE  
};

uint32_t      inputBufferFrame3[64] =
{ 
   SHD_GREEN,  SHD_GREEN,  SHD_GREEN,   SHD_GREEN,    SHD_GREEN,    SHD_GREEN,   SHD_GREEN,  SHD_GREEN, 
   
   SHD_GREEN,  SHD_YELLOW, SHD_YELLOW,  SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,  SHD_YELLOW, SHD_GREEN, 
   
   SHD_GREEN,  SHD_YELLOW, SHD_RED,     SHD_RED,      SHD_RED,      SHD_RED,     SHD_YELLOW, SHD_GREEN,
   
   SHD_GREEN,  SHD_YELLOW, SHD_RED,     SHD_BLUE,     SHD_BLUE,     SHD_RED,     SHD_YELLOW, SHD_GREEN, 
   
   SHD_GREEN,  SHD_YELLOW, SHD_RED,     SHD_BLUE,     SHD_BLUE,     SHD_RED,     SHD_YELLOW, SHD_GREEN,  
   
   SHD_GREEN,  SHD_YELLOW, SHD_RED,     SHD_RED,      SHD_RED,      SHD_RED,     SHD_YELLOW, SHD_GREEN, 
   
   SHD_GREEN,  SHD_YELLOW, SHD_YELLOW,  SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,  SHD_YELLOW, SHD_GREEN, 
   
   SHD_GREEN,  SHD_GREEN,  SHD_GREEN,   SHD_GREEN,    SHD_GREEN,    SHD_GREEN,   SHD_GREEN,  SHD_GREEN  
};

uint32_t      inputBufferFrame4[64] =
{ 
   SHD_YELLOW,  SHD_YELLOW,  SHD_YELLOW,   SHD_YELLOW,    SHD_YELLOW,    SHD_YELLOW,   SHD_YELLOW,  SHD_YELLOW, 
   
   SHD_YELLOW,  SHD_RED, SHD_RED,  SHD_RED,   SHD_RED,   SHD_RED,  SHD_RED, SHD_YELLOW, 
   
   SHD_YELLOW,  SHD_RED, SHD_BLUE,     SHD_BLUE,      SHD_BLUE,      SHD_BLUE,     SHD_RED, SHD_YELLOW,
   
   SHD_YELLOW,  SHD_RED, SHD_BLUE,     SHD_GREEN,     SHD_GREEN,     SHD_BLUE,     SHD_RED, SHD_YELLOW, 
   
   SHD_YELLOW,  SHD_RED, SHD_BLUE,     SHD_GREEN,     SHD_GREEN,     SHD_BLUE,     SHD_RED, SHD_YELLOW,  
   
   SHD_YELLOW,  SHD_RED, SHD_BLUE,     SHD_BLUE,      SHD_BLUE,      SHD_BLUE,     SHD_RED, SHD_YELLOW, 
   
   SHD_YELLOW,  SHD_RED, SHD_RED,  SHD_RED,   SHD_RED,   SHD_RED,  SHD_RED, SHD_YELLOW, 
   
   SHD_YELLOW,  SHD_YELLOW,  SHD_YELLOW,   SHD_YELLOW,    SHD_YELLOW,    SHD_YELLOW,   SHD_YELLOW,  SHD_YELLOW  
};

uint32_t      PacManFrames[4][64] =
{
  { 
   SHD_BLUE,      SHD_BLUE,   SHD_BLUE,   SHD_YELLOW, SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW, 
   
   SHD_BLUE,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW, 
   
   SHD_YELLOW,    SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_RED,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE,
   
   SHD_YELLOW,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE,   SHD_BLUE, 
   
   SHD_YELLOW,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE,   SHD_BLUE,   SHD_BLUE,  
   
   SHD_YELLOW,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE,   SHD_BLUE, 
   
   SHD_BLUE,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE, 
   
   SHD_BLUE,      SHD_BLUE,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW  
  },
  { 
   SHD_BLUE,      SHD_BLUE,   SHD_BLUE,   SHD_YELLOW, SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE,   SHD_BLUE, 
   
   SHD_BLUE,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE, 
   
   SHD_YELLOW,    SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_RED,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,
   
   SHD_YELLOW,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE,   SHD_BLUE,   SHD_BLUE, 
   
   SHD_YELLOW,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE,   SHD_BLUE,   SHD_BLUE,   SHD_BLUE,  
   
   SHD_YELLOW,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE,   SHD_BLUE,   SHD_BLUE, 
   
   SHD_BLUE,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW, 
   
   SHD_BLUE,      SHD_BLUE,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE,   SHD_BLUE  
  },
  { 
   SHD_BLUE,      SHD_BLUE,   SHD_BLUE,   SHD_YELLOW, SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE,   SHD_BLUE, 
   
   SHD_BLUE,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE, 
   
   SHD_YELLOW,    SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_RED,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,
   
   SHD_YELLOW,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW, 
   
   SHD_YELLOW,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE,   SHD_BLUE,   SHD_BLUE,   SHD_BLUE,  
   
   SHD_YELLOW,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW, 
   
   SHD_BLUE,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE, 
   
   SHD_BLUE,      SHD_BLUE,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE,   SHD_BLUE  
  },
  { 
   SHD_BLUE,      SHD_BLUE,   SHD_BLUE,   SHD_YELLOW, SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE,   SHD_BLUE, 
   
   SHD_BLUE,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE, 
   
   SHD_YELLOW,    SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_RED,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,
   
   SHD_YELLOW,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW, 
   
   SHD_YELLOW,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,  
   
   SHD_YELLOW,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE, 
   
   SHD_BLUE,      SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE, 
   
   SHD_BLUE,      SHD_BLUE,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_YELLOW,   SHD_BLUE,   SHD_BLUE  
  },
};

/*
** Sense hat display test command  
*/
int shd_test_command( int argc, char *argv[])
{
   int                i;
   int                j;
   int                z;
   int                rv = 0;
   int                fd;

   if ( shd_once == 0 )
   {   
      /* probably need to move this to a common place */
      rv = rpi_setup_i2c_bus();
      if (rv < 0)
      {
         printf("Error, rpi_setup_i2c_bus failed\n");
         return(0);
      }
      rv = i2c_dev_register_sensehatdisplay(
            "/dev/i2c",
            "/dev/i2c.shd",
            SENSEHATDISPLAY_I2C_ADDRESS
          );

      if (rv < 0)
      {
         printf("Error, i2c_dev_register_sensehatdisplay failed\n");
         return(rv);
      }
      printf("Registered Sense HAT Display device.\n");

      shd_once = 1;
   }
   
   /* 
   ** Open the Sense HAT display device file 
   */
   fd = open("/dev/i2c.shd", O_RDWR);
   if ( rv < 0 )
   {
      printf("Error opening Sense HAT display device\n");
      return(rv);
   }
   printf("Opened Sense HAT Display device!\n");  
 
   printf("Address of videoBuffer = 0x%08X\n", (unsigned int)videoBuffer); 
    
   rv = ioctl(fd, SENSEHATDISPLAY_READ_FB_MEM, (void *)videoBuffer);
   if (rv >= 0)
   {
      printf("Sense HAT Buffer:\n");
      for ( i = 0; i < 8; i++ )
      {
          printf(" Row %d\n",i);
          printf(" 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
                   videoBuffer[i*8], videoBuffer[i*8+1], videoBuffer[i*8+2], videoBuffer[i*8+3],
                   videoBuffer[i*8+4], videoBuffer[i*8+5], videoBuffer[i*8+6], videoBuffer[i*8+7]);
          printf(" 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
                   videoBuffer[i*8+8], videoBuffer[i*8+9], videoBuffer[i*8+10], videoBuffer[i*8+11],
                   videoBuffer[i*8+12], videoBuffer[i*8+13], videoBuffer[i*8+14], videoBuffer[i*8+15]);
          printf(" 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
                   videoBuffer[i*8+16], videoBuffer[i*8+17], videoBuffer[i*8+18], videoBuffer[i*8+19],
                   videoBuffer[i*8+20], videoBuffer[i*8+21], videoBuffer[i*8+22], videoBuffer[i*8+23]);
      }


      rtems_task_wake_after(20);
      memset(videoBuffer, 0, 193);
      rv = ioctl(fd, SENSEHATDISPLAY_WRITE_FB_MEM, (void *)videoBuffer); 
      
      for ( i = 0; i < 8; i++ )
      {
            MapToHat(&videoBuffer[1], i * 8 * 3, (uint8_t *)inputBuffer, i * 8 * 4);
      }
      rv = ioctl(fd, SENSEHATDISPLAY_WRITE_FB_MEM, (void *)videoBuffer); 

  for ( j = 0; j < 30; j++)
  {
     for ( z = 0; z < 4; z++ )
     {
         for ( i = 0; i < 8; i++ )
         {
             MapToHat(&videoBuffer[1], i * 8 * 3, (uint8_t *)PacManFrames[z], i * 8 * 4);
         }
         rv = ioctl(fd, SENSEHATDISPLAY_WRITE_FB_MEM, (void *)videoBuffer); 
         rtems_task_wake_after(15);
     }
  }
  for ( j = 0; j < 10; j++)
  {      
      for ( i = 0; i < 8; i++ )
      {
            MapToHat(&videoBuffer[1], i * 8 * 3, (uint8_t *)inputBufferFrame1, i * 8 * 4);
      }
      rv = ioctl(fd, SENSEHATDISPLAY_WRITE_FB_MEM, (void *)videoBuffer); 
      rtems_task_wake_after(30);
      
      for ( i = 0; i < 8; i++ )
      {
            MapToHat(&videoBuffer[1], i * 8 * 3, (uint8_t *)inputBufferFrame2, i * 8 * 4);
      }
      rv = ioctl(fd, SENSEHATDISPLAY_WRITE_FB_MEM, (void *)videoBuffer); 
      rtems_task_wake_after(30);
      
      for ( i = 0; i < 8; i++ )
      {
            MapToHat(&videoBuffer[1], i * 8 * 3, (uint8_t *)inputBufferFrame3, i * 8 * 4);
      }
      rv = ioctl(fd, SENSEHATDISPLAY_WRITE_FB_MEM, (void *)videoBuffer); 
      rtems_task_wake_after(30);
      
      for ( i = 0; i < 8; i++ )
      {
            MapToHat(&videoBuffer[1], i * 8 * 3, (uint8_t *)inputBufferFrame4, i * 8 * 4);
      }
      rv = ioctl(fd, SENSEHATDISPLAY_WRITE_FB_MEM, (void *)videoBuffer); 
      rtems_task_wake_after(30);

   }

  }
  else
  {
     printf("Sense HAT Display init fail (disconnected?)\n");
     return(rv);
  }
  close(fd);
    
  return(0);
}
  
