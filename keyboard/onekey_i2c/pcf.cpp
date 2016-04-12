#include <inttypes.h>
#include <i2cmaster.h>

#include "pcf.h"

#define PCF8575_I2C_ADDRESS 		0x40

inline uint16_t pcf_read(void)
{
	uint16_t tmp;	
	i2c_start_wait(PCF8575_I2C_ADDRESS | I2C_READ);
	tmp = i2c_readAck();
	tmp |= (uint16_t)i2c_readNak() << 8;
	i2c_stop();	
	return tmp;
}

inline void pcf_write(uint16_t val)
{
	i2c_start_wait(PCF8575_I2C_ADDRESS | I2C_WRITE);
	i2c_write((uint8_t)(val & 0xFF));
	i2c_write((uint8_t)(val >> 8));
	i2c_stop();
}

inline void pcf_init(void)
{
  i2c_init();
  pcf_write(0);
}
