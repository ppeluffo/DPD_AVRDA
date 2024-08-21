/* 
 * File:   ads1115.h
 * Author: pablo
 *
 * Created on September 29, 2023, 1:51 PM
 */

#ifndef ADS1115_H
#define	ADS1115_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "frtos-io.h"
#include "stdint.h"
#include "i2c.h"
#include "xprintf.h"

    
// Direccion de los registros del ADC
#define CONVERSION_REG  0x00
#define CONFIG_REG      0x01
#define L_THRESH        0x02
#define H_THRESH        0x03

// Posición y largo(cantidad de bits en el CONFIG_REG) de bits de configuración del ADC
#define ADS1115_CFG_OS_BIT          15
#define ADS1115_CFG_MUX_BIT         14
#define ADS1115_CFG_MUX_LENGTH      3
#define ADS1115_CFG_PGA_BIT         11
#define ADS1115_CFG_PGA_LENGTH      3
#define ADS1115_CFG_MODE_BIT        8
#define ADS1115_CFG_DR_BIT          7
#define ADS1115_CFG_DR_LENGTH       3
#define ADS1115_CFG_COMP_MODE_BIT   4
#define ADS1115_CFG_COMP_POL_BIT    3
#define ADS1115_CFG_COMP_LAT_BIT    2
#define ADS1115_CFG_COMP_QUE_BIT    1
#define ADS1115_CFG_COMP_QUE_LENGTH 2

// Configuración de puerto de entrada de datos -AINP (analog input positive)- , -AINN (analog input negative)-
#define ADS1115_MUX_P0_N1           0x00  // 000 : AINP = AIN0 and AINN = AIN1 (por defecto) 
#define ADS1115_MUX_P0_N3           0x01  // 001 : AINP = AIN0 and AINN = AIN3
#define ADS1115_MUX_P1_N3           0x02  // 010 : AINP = AIN1 and AINN = AIN3
#define ADS1115_MUX_P2_N3           0x03  // 011 : AINP = AIN2 and AINN = AIN3
#define ADS1115_MUX_P0_NG           0x04  // 100 : AINP = AIN0 and AINN = GND
#define ADS1115_MUX_P1_NG           0x05  // 101 : AINP = AIN1 and AINN = GND
#define ADS1115_MUX_P2_NG           0x06  // 110 : AINP = AIN2 and AINN = GND
#define ADS1115_MUX_P3_NG           0x07  // 111 : AINP = AIN3 and AINN = GND

// Configuracion de amplificador de ganancia programable - rango de voltajes admisibles en el ADC -
#define ADS1115_PGA_6P144           0x00
#define ADS1115_PGA_4P096           0x01
#define ADS1115_PGA_2P048           0x02 //por defecto
#define ADS1115_PGA_1P024           0x03
#define ADS1115_PGA_0P512           0x04
#define ADS1115_PGA_0P256           0x05
#define ADS1115_PGA_0P256B          0x06
#define ADS1115_PGA_0P256C          0x07
    
// Modo de operación
#define ADS1115_MODE_CONTINUOUS     0x00
#define ADS1115_MODE_SINGLESHOT     0x01 //por defecto

// El tiempo de conversión es igual a 1/DR(data-rate))
//            SPS                              conversion time
#define ADS1115_DATA_RATE_8              0x00  // 125.00 ms
#define ADS1115_DATA_RATE_16             0x01  // 62.500 ms
#define ADS1115_DATA_RATE_32             0x02  // 31.250 ms
#define ADS1115_DATA_RATE_64             0x03  // 15.625 ms
#define ADS1115_DATA_RATE_128            0x04  //  7.813 ms, por defecto
#define ADS1115_DATA_RATE_250            0x05  //  4.000 ms
#define ADS1115_DATA_RATE_475            0x06  //  2.105 ms
#define ADS1115_DATA_RATE_860            0x07  //  1.163 ms

// Deshabilito/habilito funciones del pin ALERT/RDY (activo por nivel bajo por defecto))
#define ADS1115_COMP_QUE_ASSERT1    0x00  // aviso luego de una conversión
#define ADS1115_COMP_QUE_ASSERT2    0x01  // aviso luego de dos conversiones
#define ADS1115_COMP_QUE_ASSERT4    0x02  // aviso luego de cuatro conversiones
#define ADS1115_COMP_QUE_DISABLE    0x03  // por defecto, desabilito el pin ALERT/RDY     
    
bool ads1115_debug_flag;    

int16_t ADS1115_config_inicial(bool debug);
/***devuelve true si el ADC está ocupado o false si el ADC está disponible***/
bool ADS1115_is_busy(bool debug);
/***dispara una nueva conversión***/
void ADS1115_start(bool debug);
/***devuelve el valor leído en mVolts***/
float ADS1115_readValue(uint8_t n, float k_z); //n: cantidad de medidas a promediar, k_z: constante de calibración (voltaje de offset)
/***devuelve el voltaje de offset (voltaje con entrada igual a 0 Volts)***/ 
float ADS1115_calibrar_zero(uint8_t m); //m: cantidad de medidas a promediar

void ADS1115_config_debug_flag(bool flag);

/***lee el contenido del registro de datos (registro de conversión)***/
uint16_t ADS1115_readConvReg(bool debug);
/***lanza una conversión y devuelve el dato crudo (valor entero entre 0-32768) convertido***/ 
uint16_t ADS1115_readRawValue(bool debug);
uint16_t ads1115_readReg(uint8_t RegAddr, bool debug);
uint16_t ads1115_readBitInReg(uint8_t RegAddr, uint8_t BitPos, bool debug);
uint16_t ads1115_readBitsInReg(uint8_t RegAddr, uint8_t BitPosStart, uint8_t Length, bool debug);
int16_t ads1115_writeReg(uint8_t RegAddr, uint16_t RegWord, bool debug);
int16_t ads1115_writeBitInReg(uint8_t RegAddr, uint8_t BitPos, bool BitValue, bool debug); //BitValue se debe ingresar en formato decimal
int16_t ads1115_writeBitsInReg(uint8_t RegAddr, uint8_t BitPosStart, uint8_t Length, uint16_t Wvalue, bool debug); //Wvalue se debe ingresar en formato decimal

#ifdef	__cplusplus
}
#endif

#endif	/* ADS1115_H */

