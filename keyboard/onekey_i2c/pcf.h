
#ifndef PCF8575_H
#define PCF8575_H

#include <inttypes.h>

void pcf_init(void);
uint16_t pcf_read();
void pcf_write(uint16_t val);

#endif
