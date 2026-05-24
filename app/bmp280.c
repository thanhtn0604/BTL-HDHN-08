#include "bmp280.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>

#define I2C_BUS             "/dev/i2c-2"
#define BMP280_I2C_ADDR     0x77  

#define BMP280_REG_DIG_T1    0x88 
#define BMP280_REG_CHIPID    0xD0
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_CONFIG    0xF5
#define BMP280_REG_TEMP_MSB  0xFA

static unsigned short dig_T1;
static short dig_T2, dig_T3;

int bmp_open(void) {
    int fd = open(I2C_BUS, O_RDWR);
    if (fd < 0) return -1;
    if (ioctl(fd, I2C_SLAVE, BMP280_I2C_ADDR) < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

int bmp_init(int fd) {
    uint8_t reg, buf[6];
    
    reg = BMP280_REG_CHIPID;
    write(fd, &reg, 1);
    read(fd, buf, 1);
    if (buf[0] != 0x58) return -1;

    reg = BMP280_REG_DIG_T1;
    write(fd, &reg, 1);
    read(fd, buf, 6);
    dig_T1 = (buf[1] << 8) | buf[0];
    dig_T2 = (buf[3] << 8) | buf[2];
    dig_T3 = (buf[5] << 8) | buf[4];

    uint8_t config[2] = {BMP280_REG_CTRL_MEAS, 0x27};
    write(fd, config, 2);
    config[0] = BMP280_REG_CONFIG; config[1] = 0x00;
    write(fd, config, 2);

    return 0;
}

float bmp_read_temp(int fd) {
    uint8_t reg = BMP280_REG_TEMP_MSB;
    uint8_t data[3];
    
    write(fd, &reg, 1);
    read(fd, data, 3);

    int32_t adc_T = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);

    double var1, var2, T;
    var1 = (((double)adc_T) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
    var2 = ((((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0) *
            (((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0)) * ((double)dig_T3);
    T = (var1 + var2) / 5120.0;
    
    return (float)T; 
}
