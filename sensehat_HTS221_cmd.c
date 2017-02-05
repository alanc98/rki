#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <bsp.h> /* for device driver prototypes */
#include <bsp/i2c.h>
#include <drivers/HTS221.h>

static int first_run = 0;

/*
** HTS221 test console command
*/
int sensehat_HTS221_test_command( int argc, char *argv[])
{
    int rv = 0;
    int fd = 0;
    uint16_t relative_humidity = 0;

    if (first_run == 0) /* only perform this registration once */
    {
        first_run = 1;

        /* probably need to move this to a common place */
        rv = rpi_setup_i2c_bus();
        if (rv < 0)
        {
            printf("Error, rpi_setup_i2c_bus failed\n");
            return(0);
        }

        rv = i2c_dev_register_sensehat_HTS221("/dev/i2c", "/dev/i2c.HTS221", SENSEHAT_HTS221_I2C_ADDRESS);
        if (rv < 0)
        {
            printf("Error, i2c_dev_register_sensehat_HTS221 failed\n");
            return(rv);
        }

        printf("Registered HTS221 device.\n");
    }

    /* 
    ** Open the HTS221 device file 
    */
    fd = open("/dev/i2c.HTS221", O_RDWR);
    if ( rv < 0 )
    {
        printf("Error opening HTS221 device\n");
        return(rv);
    }

    printf("Opened HTS221 device! Reading relative humidity...\n");  
 
    /* Read the relative humidity */
    rv = ioctl(fd, SENSEHAT_HTS221_READ_HUMIDITY,   (void *)&relative_humidity);
    if (rv < 0)
    {
        printf("Sense HAT HTS221 init fail (disconnected?)\n");
        return(rv);
    }
    else
    {
        printf("Relative Humidity = %d\n", relative_humidity);
    }
    
    /* close the device */
    close(fd);
    return(rv);
}
  
