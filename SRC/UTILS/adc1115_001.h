/* 
 * File:   adc1115.h
 * Author: pablo
 *
 * Created on 10 de abril de 2024, 11:03 AM
 */

#ifndef ADC1115_H
#define	ADC1115_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "frtos-io.h"
#include "stdint.h"
#include "i2c.h"
#include "xprintf.h"

#define ADS1115_OS 			15
#define ADS1115_IMUX2 		14
#define ADS1115_IMUX1 		13
#define ADS1115_IMUX0 		12
#define ADS1115_PGA2 		11
#define ADS1115_PGA1 		10
#define ADS1115_PGA0 		9
#define ADS1115_MODE 		8
#define ADS1115_DR2 		7
#define ADS1115_DR1 		6
#define ADS1115_DR0 		5
#define ADS1115_COMP_MODE 	4
#define ADS1115_COMP_POL 	3
#define ADS1115_COMP_LAT 	2
#define ADS1115_COMP_QUE1 	1
#define ADS1115_COMP_QUE0	0
    
// Bit 0,1
#define ADS1115_COMP_QUE_CON1			(0x0000)
#define ADS1115_COMP_QUE_CON2			(0x0001)
#define ADS1115_COMP_QUE_CON4			(0x0002)
#define ADS1115_COMP_QUE_DIS			(0x0003)

// Bit 2
#define ADS1115_COMP_LAT_NonLatching	(0x0000)
#define ADS1115_COMP_LAT_Latching		(0x0004)

// Bit 3
#define ADS1115_COMP_POL_3_ACTIVELOW	(0x0000)
#define ADS1115_COMP_POL_3_ACTIVEHIGH	(0x0008)

// Bit 4
#define ADS1115_COMP_MODE_TRADITIONAL	(0x0000)
#define ADS1115_COMP_MODE_WINDOWCOMP	(0x0010)

// Bit 5,6,7
#define ADS1115_DR_8SPS					(0x0000)
#define ADS1115_DR_16SPS				(0x0020)
#define ADS1115_DR_32SPS				(0x0040)
#define ADS1115_DR_64SPS				(0x0060)
#define ADS1115_DR_128SPS				(0x0080)
#define ADS1115_DR_250SPS				(0x00A0)
#define ADS1115_DR_475SPS				(0x00C0)
#define ADS1115_DR_860SPS				(0x00E0)

// Bit 8
#define ADS1115_MODE_CONTINUOUS			(0x0000)
#define ADS1115_MODE_SINGLE				(0x0100) 

// Bit 9,10,11
#define ADS1115_PGA_6_144				(0x0000)
#define ADS1115_PGA_4_096				(0x0200)
#define ADS1115_PGA_2_048				(0x0400)
#define ADS1115_PGA_1_024				(0x0600)
#define ADS1115_PGA_0_512				(0x0800)
#define ADS1115_PGA_0_256				(0x0A00)
#define ADS1115_PGA_0_256_2				(0x0C00)
#define ADS1115_PGA_0_256_3				(0x0E00)

// Bit 12,13,14
#define ADS1115_MUX_AIN0_AIN1			(0x0000)
#define ADS1115_MUX_AIN0_AIN3			(0x1000)
#define ADS1115_MUX_AIN1_AIN3			(0x2000)
#define ADS1115_MUX_AIN2_AIN3			(0x3000)
#define ADS1115_MUX_AIN0_GND			(0x4000)
#define ADS1115_MUX_AIN1_GND			(0x5000)
#define ADS1115_MUX_AIN2_GND			(0x6000)
#define ADS1115_MUX_AIN3_GND			(0x7000)

// Bit 15
#define ADS1115_OS_OFF					(0x0000)
#define ADS1115_OS_SINGLE				(0x8000)
    
 
#define CONVERSION_REG_ADDR 0x00
#define CONFIG_REG_ADDR     0x01
    
#define OS_START_CONVERTION_gc  (0x01<<15)
#define CONVERTION_DONE_bm 0x8000
    
#define MUX_SINGLE_END_AIN0_gc  (0x100 << 12)
#define MUX_SINGLE_END_AIN1_gc  (0x101 << 12)
#define MUX_SINGLE_END_AIN2_gc  (0x110 << 12)
#define MUX_SINGLE_END_AIN3_gc  (0x111 << 12)    

#define PGA_6144_gc (0x000 << 9)
#define PGA_4096_gc (0x001 << 9)
#define PGA_2048_gc (0x010 << 9)
#define PGA_1024_gc (0x110 << 9)
#define PGA_0512_gc (0x100 << 9)
#define PGA_0256_gc (0x101 << 9)
    
#define MODE_CONTINUO_gc    (0x00 << 8)
#define MODE_SINGLESHOT_gc  (0x01 << 8)
    
#define DATA_RATE_8SPS_gc    (0x000 << 5)
#define DATA_RATE_16SPS_gc   (0x001 << 5)
#define DATA_RATE_32SPS_gc   (0x100 << 5)
#define DATA_RATE_64SPS_gc   (0x110 << 5)
#define DATA_RATE_128SPS_gc  (0x100 << 5)
#define DATA_RATE_250SPS_gc  (0x101 << 5)
    
#define COMP_TRADICIONAL_gc (0x00 << 4)
#define COMP_WINDOWING_gc   (0x01 << 4)
    
    
int8_t ADC1115_init(void);
void ADC1115_start_conversion(void);
bool ADC1115_is_conversion_done(void);
uint16_t ADC1115_get_conversion_result(void);

int16_t ADC1115_read( uint16_t rdAddress, char *data, uint8_t length, bool debug );
int16_t ADC1115_write( uint16_t wrAddress, char *data, uint8_t length, bool debug );
int16_t ADC1115_test_write ( char *rconf_val_str );
int16_t ADC1115_test_read ( char *regs );
uint16_t ADC1115_read_single_ended(void);
uint32_t ADC1115_multiple_read(uint8_t times);

typedef enum {
    ADS1115_COMP_0_1   = 0x0000,
    ADS1115_COMP_0_3   = 0x1000,
    ADS1115_COMP_1_3   = 0x2000,
    ADS1115_COMP_2_3   = 0x3000,
    ADS1115_COMP_0_GND = 0x4000,
    ADS1115_COMP_1_GND = 0x5000,
    ADS1115_COMP_2_GND = 0x6000,
    ADS1115_COMP_3_GND = 0x7000, 
} ADS1115_MUX;

#define ADS1115_CONV_REG       0x00 // Conversion Register
#define ADS1115_CONFIG_REG     0x01 // Configuration Register
#define ADS1115_LO_THRESH_REG  0x02 // Low Threshold Register
#define ADS1115_HI_THRESH_REG  0x03 // High Threshold Register

#define ADS1115_REG_FACTOR    32768
#define ADS1115_REG_RESET_VAL 0x8583

typedef enum ADS1115_CONV_RATE{
    ADS1115_8_SPS    = 0x0000,
    ADS1115_16_SPS   = 0x0020,
    ADS1115_32_SPS   = 0x0040,
    ADS1115_64_SPS   = 0x0060,
    ADS1115_128_SPS  = 0x0080,
    ADS1115_250_SPS  = 0x00A0,
    ADS1115_475_SPS  = 0x00C0,
    ADS1115_860_SPS  = 0x00E0,
    ADS1015_128_SPS  = ADS1115_8_SPS,
    ADS1015_250_SPS  = ADS1115_16_SPS,
    ADS1015_490_SPS  = ADS1115_32_SPS,
    ADS1015_920_SPS  = ADS1115_64_SPS,
    ADS1015_1600_SPS = ADS1115_128_SPS,
    ADS1015_2400_SPS = ADS1115_250_SPS,
    ADS1015_3300_SPS = ADS1115_475_SPS,
    ADS1015_3300_SPS_2 = ADS1115_860_SPS
} convRate;

typedef enum {
    ADS1115_CONTINUOUS = 0x0000, 
    ADS1115_SINGLE     = 0x0100,
    ADS1015_CONTINUOUS = ADS1115_CONTINUOUS,
    ADS1015_SINGLE     = ADS1115_SINGLE 
} ADS1115_MEASURE_MODE;

typedef enum {
    ADS1115_RANGE_6144  = 0x0000,
    ADS1115_RANGE_4096  = 0x0200,
    ADS1115_RANGE_2048  = 0x0400,
    ADS1115_RANGE_1024  = 0x0600,
    ADS1115_RANGE_0512  = 0x0800,
    ADS1115_RANGE_0256  = 0x0A00,
    ADS1015_RANGE_6144  = ADS1115_RANGE_6144,
    ADS1015_RANGE_4096  = ADS1115_RANGE_4096,
    ADS1015_RANGE_2048  = ADS1115_RANGE_2048,
    ADS1015_RANGE_1024  = ADS1115_RANGE_1024,
    ADS1015_RANGE_0512  = ADS1115_RANGE_0512,
    ADS1015_RANGE_0256  = ADS1115_RANGE_0256
} ADS1115_RANGE;

convRate ADC_getConvRate(void);
void ADC_setCompareChannels(ADS1115_MUX mux);
void ADC_startSingleMeasurement(void);
bool ADC_isBusy(void);
float ADC_getResult_V(void);
float ADC_getResult_mV(void);
int16_t ADC_getRawResult(void);
float ADC_readChannel(ADS1115_MUX channel);
bool ADC_init(void);
void ADC_setup(void);
void ADC_setVoltageRange_mV(ADS1115_RANGE range);
void ADC_setMeasureMode(ADS1115_MEASURE_MODE mode);

uint16_t ADC_readRegister(uint8_t rdAddress);
void ADC_writeRegister( uint8_t wrAddress, uint16_t val);

#ifdef	__cplusplus
}
#endif

#endif	/* ADC1115_H */

