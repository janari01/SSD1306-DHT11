#ifndef COM_H
#define COM_H

#include <stdint.h>  

int i2c_init();
int i2c_write(uint8_t slave_addr, uint8_t reg, uint8_t data);
int i2c_read(uint8_t slave_addr, uint8_t reg, uint8_t *result);
void i2c_stop();

#endif