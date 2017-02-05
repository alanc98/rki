#ifndef SENSEHATLPS25H_H
#define SENSEHATLPS25H_H

/* 
** RTEMS i2c driver for the Raspberry Pi Sense HAT HTS221 Humidity Sensor
*/

/* 
**  Copyright (c) 2017 Scott Zemerick
*/

#include <rtems.h>
#include <dev/i2c/i2c.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
** HTS221 Register Addresses
*/
#define SENSEHAT_HTS221_I2C_ADDRESS             0x5f    /* i2c address on the SenseHat */
#define SENSEHAT_HTS221_WHOAMI_ADDRESS          0x0f    /* who ami i register */
#define SENSEHAT_HTS221_IAMID                   0xbc    /* whoami identifier */
#define SENSEHAT_HTS221_CTRL_REG1_ADDR          0x20
#define SENSEHAT_HTS221_CTRL_REG2_ADDR          0x21
#define SENSEHAT_HTS221_HUMIDITY_OUT_LSB_ADDR   0x28  
#define SENSEHAT_HTS221_HUMIDITY_OUT_MSB_ADDR   0x29
#define SENSEHAT_HTS221_TEMP_OUT_ADDR           0x2a

#define H0_T0_OUT_LSB               0x36
#define H0_T0_OUT_MSB               0x37
#define H1_T0_OUT_LSB               0x3a
#define H1_T0_OUT_MSB               0x3b

#define SENSEHAT_HTS221_CONT_1HZ    0x81

/*
** Calibration Registers 
*/
#define SENSEHAT_HTS221_H0_RH_X2    0x30
#define SENSEHAT_HTS221_H1_RH_X2    0x31

/*
** Sense HAT HTS221 IOCTL commands
*/
typedef enum
{
  SENSEHAT_HTS221_READ_WHOAMI,
  SENSEHAT_HTS221_READ_HUMIDITY,
  SENSEHAT_HTS221_READ_TEMP
} sensehat_HTS221_cmd;

/* 
** Prototypes
*/
int i2c_dev_register_sensehat_HTS221(const char *bus_path, 
                                     const char *dev_path, 
                                     uint16_t address);
   
#ifdef __cplusplus
}
#endif

#endif
