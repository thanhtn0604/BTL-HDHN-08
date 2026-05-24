#ifndef BMP280_H
#define BMP280_H

int bmp_open(void);
int bmp_init(int fd);
float bmp_read_temp(int fd);

#endif
