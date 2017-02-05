/* 
 * RTEMS i2c driver for the Raspberry Pi Sense HAT
 * HTS221 Humidity / Temperature Sensor
 * 
*/

/* 
 ** (c) Scott Zemerick, scott.zemerick@embeddedhorizon.com, 2017
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <rtems.h>
#include <dev/i2c/i2c.h>
#include <drivers/HTS221.h>

/* 
** set this definition to 1 for debug printf statements 
*/
#define DEBUG 0

/*
** Global data
*/

int HTS221_read_temp(i2c_dev *dev)
{
    printf("reading HTS221 temperature...\n");

}

int HTS221_read_humidity(i2c_dev *dev, uint16_t *relative_humidity)
{
    /* 
    ** This code is derived from the application note: 
    ** "TN218 - Interpreting humidity and temperature readings in the HTS221 digital humidity sensor"
    */

    int rv = 0;
    i2c_msg msg[2];
    uint8_t reg[2];
    uint8_t h0 = 0;
    uint8_t h1 = 0;
    int16_t H0_rh = 0;
    int16_t H1_rh = 0;
    uint8_t h0_t0_lsb = 0;
    uint8_t h0_t0_msb = 0;
    int16_t h0_t0 = 0;
    uint8_t h1_t0_lsb = 0;
    uint8_t h1_t0_msb = 0;
    int16_t h1_t0 = 0;
    uint8_t hout_raw_lsb = 0;
    uint8_t hout_raw_msb = 0;
    int16_t hout_raw = 0;
    uint32_t temp = 0;
    uint8_t ctrl_reg_1_val = 0xff;

    /* 
    ** Read the Control Register 1 for sanity check of active mode
    ** and continuous update at 1Hz. If not, then set this mode. By default
    ** the chip comes up in one-shot mode
    */
    reg[0] = SENSEHAT_HTS221_CTRL_REG1_ADDR;
    msg[0].addr = dev->address;
    msg[0].flags = 0;
    msg[0].len = (uint16_t)1;
    msg[0].buf = &reg[0];

    msg[1].addr = dev->address;
    msg[1].flags = I2C_M_RD;
    msg[1].len = 1;
    msg[1].buf = &ctrl_reg_1_val;
    rv = i2c_bus_transfer(dev->bus, msg, 2);
    if (ctrl_reg_1_val != SENSEHAT_HTS221_CONT_1HZ)
    {
        /* Write the Control Register 1 for one-hertz update, active mode */
        reg[0] = SENSEHAT_HTS221_CTRL_REG1_ADDR;
        reg[1] = SENSEHAT_HTS221_CONT_1HZ;
        msg[0].addr = dev->address;
        msg[0].flags = 0;
        msg[0].len = (uint16_t)2;
        msg[0].buf = &reg[0];
        rv = i2c_bus_transfer(dev->bus, msg, 1);
    }

    /* 
    ** 1. Read the value of coefficients H0_rH_x2 and H1_rH_x2 from registers 0x30 & 0x31.
    **    Note - these values do not change and only need read one time
    */
    reg[0] = SENSEHAT_HTS221_H0_RH_X2;
    msg[0].addr = dev->address;
    msg[0].flags = 0;
    msg[0].len = (uint16_t)1;
    msg[0].buf = &reg[0];

    msg[1].addr = dev->address;
    msg[1].flags = I2C_M_RD;
    msg[1].len = 1;
    msg[1].buf = &h0;
    rv = i2c_bus_transfer(dev->bus, msg, 2);

    /* Read the H1 Calibration Register */
    reg[0] = SENSEHAT_HTS221_H1_RH_X2;
    msg[0].addr = dev->address;
    msg[0].flags = 0;
    msg[0].len = (uint16_t)1;
    msg[0].buf = &reg[0];

    msg[1].addr = dev->address;
    msg[1].flags = I2C_M_RD;
    msg[1].len = 1;
    msg[1].buf = &h1;
    rv = i2c_bus_transfer(dev->bus, msg, 2);

    /* 
    ** 2. Divide by two the content of registers 0x30 (H0_rH_x2)and 0x31 (H1_rH_x2)in order
    ** to obtain the value of coefficients H0_rH and H1_rH.
    */
    H0_rh = h0 / 2;
    H1_rh = h1 / 2;
    
    /* 
     ** 3. Read the value of H0_T0_OUT from registers 0x36 & 0x37. 
     ** Read H0_T0_OUT LSB and MSB and combine 
     */
    reg[0] = H0_T0_OUT_LSB;
    msg[0].addr = dev->address;
    msg[0].flags = 0;
    msg[0].len = (uint16_t)1;
    msg[0].buf = &reg[0];

    msg[1].addr = dev->address;
    msg[1].flags = I2C_M_RD;
    msg[1].len = 1;
    msg[1].buf = &h0_t0_lsb;
    rv = i2c_bus_transfer(dev->bus, msg, 2);

    reg[0] = H0_T0_OUT_MSB;
    msg[0].addr = dev->address;
    msg[0].flags = 0;
    msg[0].len = (uint16_t)1;
    msg[0].buf = &reg[0];

    msg[1].addr = dev->address;
    msg[1].flags = I2C_M_RD;
    msg[1].len = 1;
    msg[1].buf = &h0_t0_msb;
    rv = i2c_bus_transfer(dev->bus, msg, 2);
    h0_t0 = (uint16_t)((h0_t0_msb << 8) | h0_t0_lsb);
    
    /* 
     ** 4. Read the value of H1_T0_OUT from registers 0x3A & 0x3B.
     ** Read H1_T0_OUT LSB and MSB and combine 
     */
    reg[0] = H1_T0_OUT_LSB;
    msg[0].addr = dev->address;
    msg[0].flags = 0;
    msg[0].len = (uint16_t)1;
    msg[0].buf = &reg[0];

    msg[1].addr = dev->address;
    msg[1].flags = I2C_M_RD;
    msg[1].len = 1;
    msg[1].buf = &h1_t0_lsb;
    rv = i2c_bus_transfer(dev->bus, msg, 2);

    reg[0] = H1_T0_OUT_MSB;
    msg[0].addr = dev->address;
    msg[0].flags = 0;
    msg[0].len = (uint16_t)1;
    msg[0].buf = &reg[0];

    msg[1].addr = dev->address;
    msg[1].flags = I2C_M_RD;
    msg[1].len = 1;
    msg[1].buf = &h1_t0_msb;
    rv = i2c_bus_transfer(dev->bus, msg, 2);
    h1_t0 = (uint16_t)((h1_t0_msb << 8) | h1_t0_lsb);

    /* 
     ** 5. Read the humidity value in raw counts H_T_OUT from registers 0x28 & 0x29
     */
    reg[0] = SENSEHAT_HTS221_HUMIDITY_OUT_LSB_ADDR;
    msg[0].addr = dev->address;
    msg[0].flags = 0;
    msg[0].len = (uint16_t)1;
    msg[0].buf = &reg[0];

    msg[1].addr = dev->address;
    msg[1].flags = I2C_M_RD;
    msg[1].len = 1;
    msg[1].buf = &hout_raw_lsb;
    rv = i2c_bus_transfer(dev->bus, msg, 2);

    reg[0] = SENSEHAT_HTS221_HUMIDITY_OUT_MSB_ADDR;
    msg[0].addr = dev->address;
    msg[0].flags = 0;
    msg[0].len = (uint16_t)1;
    msg[0].buf = &reg[0];

    msg[1].addr = dev->address;
    msg[1].flags = I2C_M_RD;
    msg[1].len = 1;
    msg[1].buf = &hout_raw_msb;
    rv = i2c_bus_transfer(dev->bus, msg, 2);

    hout_raw = (uint16_t)((hout_raw_msb << 8) | hout_raw_lsb);
#if DEBUG == 1
    printf("raw humidity counts = %d\n", (uint16_t)hout_raw);
#endif

    /* 
    ** 6. Compute the RH[%] value, by linear interpolation, applying the formula from data sheet                
    */

    *relative_humidity = (((H1_rh - H0_rh) * (hout_raw - h0_t0)) / (h1_t0 - h0_t0)) + H0_rh;
#if DEBUG == 1
    printf("HTS221: humidity=%d\n", relative_humidity);
#endif
    return(rv);
}

int HTS221_read_whoami(i2c_dev *dev, uint8_t *whoami)
{
    int rv = 0;
    i2c_msg msg[2];
    uint8_t reg[1];

    /* 
    ** write the register address to read back 
    */
    reg[0] = SENSEHAT_HTS221_WHOAMI_ADDRESS;

    /* 
    ** setup the i2c message - write the address
    */
    msg[0].addr = dev->address;
    msg[0].flags = 0;
    msg[0].len = (uint16_t)1;
    msg[0].buf = &reg[0];

    /*
    ** And read back the data
    */
    msg[1].addr = dev->address;
    msg[1].flags = I2C_M_RD;
    msg[1].len = 1;
    msg[1].buf = whoami;

    rv = i2c_bus_transfer(dev->bus, msg, 2);

#if DEBUG == 1
    printf("HTS221(): i2c_bus_transfer rv = 0x%08X, whoami=0x%x\n", rv, *whoami);
#endif

    return(rv);
}


/********************** Driver API ******************************************/

static int i2c_sensehat_HTS221_linux_ioctl(i2c_dev *dev,
                                           ioctl_command_t command,
                                           void *arg) 
{
    int rv = 0;
    uint8_t *whoami = NULL;
    uint16_t *relative_humidity = NULL;

    switch ( command ) 
    {
        case SENSEHAT_HTS221_READ_WHOAMI:
            whoami = (uint8_t *)arg;
            rv = HTS221_read_whoami(dev, whoami);
            break;
        case SENSEHAT_HTS221_READ_HUMIDITY:
            relative_humidity = (uint16_t *)arg;
            rv = HTS221_read_humidity(dev, relative_humidity);
            break;
        case SENSEHAT_HTS221_READ_TEMP:
            rv = HTS221_read_temp(dev);
            break;
    }

    return rv;
}


/*
** Register the device 
*/
int i2c_dev_register_sensehat_HTS221(const char *bus_path,
                                     const char *dev_path,
                                     uint16_t address) 
{
    i2c_dev *dev;
    dev = i2c_dev_alloc_and_init(sizeof(*dev), bus_path, address);
    if (dev == NULL) 
    {
        printf("SENSE HAT HTS221: Error calling i2c_dev_alloc_and_init\n");
        return -1;
    }

    dev->ioctl = i2c_sensehat_HTS221_linux_ioctl;
    return i2c_dev_register(dev, dev_path);
}

