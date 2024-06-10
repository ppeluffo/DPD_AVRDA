/*
 * l_i2c.h
 *
 *  Created on: 26 de mar. de 2018
 *      Author: pablo
 */

#ifndef SPX_LIBS_L_I2C_H_
#define SPX_LIBS_L_I2C_H_

#include "stdint.h"
#include "frtos-io.h"
#include "xprintf.h"

#define DEVADDRESS_EEPROM_M2402	0xA0
#define DEVADDRESS_RTC_M79410	0xDE
#define DEVADDRESS_INA         	0x82
#define DEVADDRESS_ADS1115      0x90
#define DEVADDRESS_ADC1115      0x90

void I2C_init(void);
int16_t I2C_read  ( int fdTWI, uint8_t devAddress, uint16_t dataAddress, uint8_t dataAddress_length, char *data, uint8_t data_length, bool i2c_debug );
int16_t I2C_write ( int fdTWI, uint8_t devAddress, uint16_t dataAddress, uint8_t dataAddress_length, char *data, uint8_t data_length, bool i2c_debug );


#endif /* SPX_LIBS_L_I2C_H_ */
