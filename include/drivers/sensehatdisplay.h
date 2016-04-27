#ifndef SENSEHATDISPLAY_H
#define SENSEHATDISPLAY_H

/* 
 * RTEMS i2c driver for the Raspberry Pi Sense HAT 8x8 LED display 
*/

/* 
**  Copyright (c) 2015 Alan Cudmore
*/

#include <rtems.h>
#include <dev/i2c/i2c.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SENSEHATDISPLAY_I2C_ADDRESS  0x46  


/*
** Colors for the Sense Hat Display
*/
#define SHD_BLACK      0x00000000
#define SHD_RED        0x00FF0000
#define SHD_GREEN      0x0000FF00
#define SHD_BLUE       0x000000FF
#define SHD_YELLOW     0x00FFFF00
#define SHD_WHITE      0x00FFFFFF
#define SHD_LT_BLUE    0x0000FFFF
#define SHD_PURPLE     0x00FF00FF


/*
** Sense HAT Display IOCTL commands
*/
typedef enum
{
  SENSEHATDISPLAY_WRITE_FB_MEM,
  SENSEHATDISPLAY_READ_FB_MEM
} sensehatdisplay_cmd;

int i2c_dev_register_sensehatdisplay(
  const char *bus_path,
  const char *dev_path,
  uint16_t address
);
   
#ifdef __cplusplus
}
#endif

#endif
