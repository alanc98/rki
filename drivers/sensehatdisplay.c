/* 
 * RTEMS i2c driver for the Raspberry Pi Sense HAT 8x8 LED matrix display 
 *
 *  Should probably implement the read and write calls for reading and writing
 *   the frame buffer and joystick state
 *   
 *  The eeprom i2c device might serve as a good template for this.
 *  its kind of wierd with the device struct that is a superstruct of the i2c_dev type.
 *   probably dont need the extra stuff that eeprom uses.
 *
 * 
*/

/* 
 * (c) Alan Cudmore , Alan.Cudmore@gmail.com, 2015
 * 
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <rtems.h>
#include <dev/i2c/i2c.h>
#include <drivers/sensehatdisplay.h>

/*
** Global data
*/
uint8_t SHD_gamma[] = 
{
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
   0x02, 0x02, 0x03, 0x03, 0x04, 0x05, 0x06, 0x07,
   0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0E, 0x0F, 0x11,
   0x12, 0x14, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F
};

uint8_t SHD_inverse_gamma[] = 
{
   0x00, 0x06, 0x08, 0x0A, 0x0C, 0x0D, 0x0E, 0x0F,
   0x10, 0x11, 0x12, 0x13, 0x14, 0x14, 0x15, 0x16,
   0x16, 0x17, 0x18, 0x18, 0x19, 0x1A, 0x1A, 0x1B,
   0x1B, 0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E, 0x1F
};

/* 
** The layout of the LED buffer:
** Row 1: R R R R R R R R G G G G G G G G B B B B B B B B
** Row 2: R R R R R R R R G G G G G G G G B B B B B B B B
** ...
** Row 8: R R R R R R R R G G G G G G G G B B B B B B B B
** 
** Note: Each color component is only 5 bits long, i.e. 0=darkest and 31=brightest.
*/

/*
** This function arranges the array into the format that the
** LED array needs.
** I need to understand how this works
**  i.e. does it do:
**     RGBA --> R             G             B    ?
** 
** Note: I think this just does one row!
**       
**   Why wouldnt it apply gamma at this stage??
** 
** http://www.rapidtables.com/web/color/RGB_Color.htm
*/
void MapToHat(uint8_t *output, int outputOffset, uint8_t *input, int inputOffset)
{
   int i;
   for ( i=0; i < 8; i++ )
   {
      int     co    = inputOffset + i * 4;
      uint8_t blue  = input[co];
      uint8_t green = input[co + 1];
      uint8_t red   = input[co + 2];
      uint8_t alpha = input[co + 3]; /* Seems to be ignored */

      output[outputOffset + i]      = SHD_gamma[red >> 3];
      output[outputOffset + i + 8]  = SHD_gamma[green >> 3];
      output[outputOffset + i + 16] = SHD_gamma[blue >> 3];    

   }
}

/*
** Read an array of bytes from device
** values: external array to hold data. Put starting register in values[0].
** length: number of bytes to read
** 
*/
int senshatdisplay_readVideoMemory(i2c_dev *dev, uint8_t *videoMem)
{
   int     rv;
   i2c_msg msg[2];
   uint8_t reg[1];
   
   /* 
   ** Write the register address to read back 
   */
   reg[0] = 0; 
   msg[0].addr = dev->address;
   msg[0].flags = 0;
   msg[0].len = (uint16_t) 1; 
   msg[0].buf = &reg[0];
   
   /* 
   ** And read back the data 
   */
   msg[1].addr = dev->address;
   msg[1].flags = I2C_M_RD;
   msg[1].len = 192;
   msg[1].buf = videoMem;

   printf("Address of videoMem = 0x%08X\n", (unsigned int)videoMem);

   rv = i2c_bus_transfer(dev->bus, msg, 2);
   printf("sensehatdisplay: i2c_bus_transfer rv = %08X\n",rv);
   return(rv);
}

/*
** Write an array of bytes to Video Memory  
*/
int senshatdisplay_writeVideoMem(i2c_dev *dev, uint8_t *videoMem)
{
   int     rv;
   i2c_msg msg[2];
   uint8_t reg[1];
   
   /* 
   ** Write the register address to read back 
   */
   reg[0] = 0; 
   msg[0].addr = dev->address;
   msg[0].flags = 0;
   msg[0].len = (uint16_t) 1; 
   msg[0].buf = &reg[0];
   
   /*
   ** Write buffer to LED matrix
   */
   msg[1].addr = dev->address;
   msg[1].flags = 0;
   msg[1].len = 193;
   msg[1].buf = videoMem;

   rv = i2c_bus_transfer(dev->bus, msg, 2);
   return(rv);
}

/********************** Driver API ******************************************/

static int i2c_sensehatdisplay_linux_ioctl(
  i2c_dev *dev,
  ioctl_command_t command,
  void *arg
) 
{
  int      rv = 0;
  uint8_t *videoBuffer;
  int      i;

  videoBuffer = (uint8_t *)arg; 
  
  switch ( command ) 
  {

    case SENSEHATDISPLAY_WRITE_FB_MEM:
          rv = senshatdisplay_writeVideoMem(dev, videoBuffer);
          break;

    case SENSEHATDISPLAY_READ_FB_MEM:
          rv = senshatdisplay_readVideoMemory(dev, videoBuffer); 
          break;

    default:
          rv = -1;
  }
  return rv;
}

/*
** Register the shd device 
*/
int i2c_dev_register_sensehatdisplay(
  const char *bus_path,
  const char *dev_path,
  uint16_t address
) {
  i2c_dev *dev;

  dev = i2c_dev_alloc_and_init(sizeof(*dev), bus_path, address);
  if (dev == NULL) 
  {
    printf("SENSE HAT DISPLAY: Error calling i2c_dev_alloc_and_init\n");
    return -1;
  }
  dev->ioctl = i2c_sensehatdisplay_linux_ioctl;

  return i2c_dev_register(dev, dev_path);
}

