
#include "adc1115.h"


//------------------------------------------------------------------------------
int8_t ADC1115_init(void)
{
    
uint16_t config_register = 0x00;
    
    config_register = ADS1115_COMP_QUE_DIS	|
				 ADS1115_COMP_LAT_NonLatching |
				 ADS1115_COMP_POL_3_ACTIVELOW |	
				 ADS1115_COMP_MODE_TRADITIONAL | 
				 ADS1115_DR_128SPS |
				 ADS1115_MODE_SINGLE | 
                 ADS1115_PGA_2_048 |
                 ADS1115_MUX_AIN0_GND |
                 ADS1115_OS_OFF;
        
    //config_register = 0xC383;
    
    xprintf_P(PSTR("DEBUG: ADC_init Config register = 0x%02x\r\n"), config_register);
    
    // Escribo el registro
    ADC1115_write ( CONFIG_REG_ADDR, (char *)&config_register, 2, false);
    
    return(0);
    
}
//------------------------------------------------------------------------------
void ADC1115_start_conversion(void)
{
    
uint16_t config_register = 0x00;

    ADC1115_read ( CONFIG_REG_ADDR, (char *)&config_register, 2, false);
    
    config_register |= OS_START_CONVERTION_gc;
    
    xprintf_P(PSTR("DEBUG: ADC_start_conversion Config register = 0x%02x\r\n"), config_register);
    // Indico la direccion del registro a configurar
    ADC1115_write ( CONFIG_REG_ADDR, (char *)&config_register, 2, false);
}
//------------------------------------------------------------------------------
uint32_t ADC1115_multiple_read(uint8_t times)
{
    /*
     * Leo times veces y promedio
     */
    
uint16_t config_register = 0x00;
uint8_t data[2];
uint16_t val;
uint32_t total = 0;
uint8_t i;

  
    for (i=0; i<times; i++) {
        // Inicializo para leer 1 vez
        ADC1115_read ( CONFIG_REG_ADDR, (char *)&config_register, 2, false);
        config_register |= OS_START_CONVERTION_gc;
        ADC1115_write ( CONFIG_REG_ADDR, (char *)&config_register, 2, false);
        vTaskDelay(50);
        // Leo
        ADC1115_read ( CONVERSION_REG_ADDR, (char *)&data, 2, false);
        val = ( data[1] << 8 | data[0] );
        total += val;
        xprintf_P(PSTR("mr%02d: data=%u, msb=0x%02x, lsb=0x%02x, total=%lu\r\n"), i, val, data[1], data[0], total);
    }

    total = total / times;
    return(total);
       
}
//------------------------------------------------------------------------------
bool ADC1115_is_conversion_done(void)
{
    /*
     * Leemos el CONFIG REGISTER y el bit 15 nos dice como esta
     */

uint16_t config_register = 0x00;
   
    // Escribo el registro
    ADC1115_read ( CONFIG_REG_ADDR, (char *)&config_register, 2, false);
    
    xprintf_P(PSTR("DEBUG: ADC_is_conversion_done Config register = 0x%02x\r\n"), config_register);
    
    return( config_register & CONVERTION_DONE_bm);
}
//------------------------------------------------------------------------------
uint16_t ADC1115_get_conversion_result(void)
{

uint16_t convertion_results = 0x00;
uint8_t data[2];

    xprintf_P(PSTR("DEBUG: ADC_get_conversion_result\r\n"));
    
    // Indico la direccion del registro a configurar
    // Escribo el registro
    ADC1115_read ( CONVERSION_REG_ADDR, (char *)&data, 2, false);
    convertion_results = ( data[1] << 8 | data[0] );
    xprintf_P(PSTR("DEBUG: AADC1115_get_conversion_result: result = [0x%02x][0x%02x]\r\n"), data[1], data[0]);
    
    return(convertion_results);   
}
//------------------------------------------------------------------------------
uint16_t ADC1115_read_single_ended(void)
{
    // Lee un canal
   
uint16_t adc_config;
uint8_t adc_data[2];
uint16_t resVal;

    adc_config = ADS1115_COMP_QUE_DIS	|
				 ADS1115_COMP_LAT_NonLatching |
				 ADS1115_COMP_POL_3_ACTIVELOW |	
				 ADS1115_COMP_MODE_TRADITIONAL | 
				 ADS1115_DR_128SPS |
				 ADS1115_MODE_SINGLE | 
                 ADS1115_PGA_2_048 |
                 ADS1115_MUX_AIN0_GND |
                 ADS1115_OS_SINGLE;
	
    // Escribo el registro
    xprintf_P(PSTR("DEBUG: ADC1115_read_single_ended: config = 0x%02x\r\n"), adc_config);
    ADC1115_write ( CONFIG_REG_ADDR, (char *)&adc_config, 2, false);
    
    vTaskDelay(50);
    
    // Espero la conversion y leo
    ADC1115_read ( CONVERSION_REG_ADDR, (char *)&adc_data, 2, false);
    xprintf_P(PSTR("DEBUG: ADC1115_read_single_ended: result = [0x%02x][0x%02x]\r\n"), adc_data[1], adc_data[0]);
    resVal = adc_data[1] << 8 | adc_data[0];
    
    return(resVal);
}
//------------------------------------------------------------------------------
int16_t ADC1115_read( uint16_t rdAddress, char *data, uint8_t length, bool debug )
{
int16_t rcode = 0;

    if (debug) {
        xprintf_P(PSTR("ADC1115_read.\r\n"));
    }

	rcode =  I2C_read( fdI2C0, DEVADDRESS_ADC1115, rdAddress, 0x01, data, length, debug );
	if ( rcode == -1 ) {
		// Hubo error: trato de reparar el bus y reintentar la operacion
		// Espero 1s que se termine la fuente de ruido.
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
		// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
		xprintf("ERROR: ADC1115_read recovering i2c bus\r\n" );
		goto quit;
	}

quit:

	return( rcode );
}
//------------------------------------------------------------------------------
int16_t ADC1115_write( uint16_t wrAddress, char *data, uint8_t length, bool debug )
{
int16_t rcode = 0;

    if (debug) {
        xprintf_P(PSTR("ADC1115_write.\r\n"));
    }

 	rcode =  I2C_write ( fdI2C0, DEVADDRESS_ADC1115, wrAddress, 0x01, data, length, debug );
	if ( rcode == -1 ) {
		// Hubo error: trato de reparar el bus y reintentar la operacion
		// Espero 1s que se termine la fuente de ruido.
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
		// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
		xprintf("ERROR: ADC1115_write recovering i2c bus\r\n" );
	}

	return( rcode );
}
//------------------------------------------------------------------------------
int16_t ADC1115_test_write ( char *rconf_val_str )
{
    return(0);
}
//------------------------------------------------------------------------------
int16_t ADC1115_test_read ( char *regs )
{
    return(0);
}
//------------------------------------------------------------------------------
uint16_t ADC_voltageRange;
bool ADC_autoRangeMode;
ADS1115_MEASURE_MODE ADC_deviceMeasureMode;

//------------------------------------------------------------------------------
uint16_t ADC_readRegister(uint8_t rdAddress)
{
uint16_t data;

    I2C_read( fdI2C0, DEVADDRESS_ADC1115, rdAddress, 0x01, (char*)&data, 0x02, false );
    return (data);
}
//------------------------------------------------------------------------------
void ADC_writeRegister( uint8_t wrAddress, uint16_t val)
{
uint16_t data = val;
    
    I2C_write ( fdI2C0, DEVADDRESS_ADC1115, wrAddress, 0x01, (char *)&data, 0x02, false );
}
//------------------------------------------------------------------------------
bool ADC_init(void)
{

    ADC_writeRegister(ADS1115_CONFIG_REG, ADS1115_REG_RESET_VAL);
    ADC_setVoltageRange_mV(ADS1115_RANGE_2048);
    ADC_writeRegister(ADS1115_LO_THRESH_REG, 0x8000);
    ADC_writeRegister(ADS1115_HI_THRESH_REG, 0x7FFF);
    ADC_deviceMeasureMode = ADS1115_SINGLE;
    ADC_autoRangeMode = false;
    return (true);
}
//------------------------------------------------------------------------------
void ADC_setup(void)
{
    if(!ADC_init()){
        xprintf_P(PSTR("ADS1115 not connected!\r\n"));
    }

  /* Set the voltage range of the ADC to adjust the gain
   * Please note that you must not apply more than VDD + 0.3V to the input pins!
   * 
   * ADS1115_RANGE_6144  ->  +/- 6144 mV
   * ADS1115_RANGE_4096  ->  +/- 4096 mV
   * ADS1115_RANGE_2048  ->  +/- 2048 mV (default)
   * ADS1115_RANGE_1024  ->  +/- 1024 mV
   * ADS1115_RANGE_0512  ->  +/- 512 mV
   * ADS1115_RANGE_0256  ->  +/- 256 mV
   */
   //ADC_setVoltageRange_mV(ADS1115_RANGE_6144); //comment line/change parameter to change range
}
//------------------------------------------------------------------------------  
void ADC_setMeasureMode(ADS1115_MEASURE_MODE mode)
{
    
uint16_t currentConfReg;

    currentConfReg = ADC_readRegister(ADS1115_CONFIG_REG);
    ADC_deviceMeasureMode = mode;
    currentConfReg &= ~(0x8100);    
    currentConfReg |= mode;
    ADC_writeRegister(ADS1115_CONFIG_REG, currentConfReg);
}
//------------------------------------------------------------------------------      
void ADC_setVoltageRange_mV(ADS1115_RANGE range)
{
    
//uint16_t currentVoltageRange;
uint16_t currentConfReg;
//uint16_t currentRange;
//uint16_t currentAlertPinMode;
//int16_t alertLimit;

    //currentVoltageRange = ADC_voltageRange;
    currentConfReg = ADC_readRegister(ADS1115_CONFIG_REG);
    //currentRange = (currentConfReg >> 9) & 7;
    //currentAlertPinMode = currentConfReg & 3;
    
    ADC_setMeasureMode(ADS1115_SINGLE);
    
    switch(range){
        case ADS1115_RANGE_6144:
            ADC_voltageRange = 6144;
            break;
        case ADS1115_RANGE_4096:
            ADC_voltageRange = 4096;
            break;
        case ADS1115_RANGE_2048:
            ADC_voltageRange = 2048;
            break;
        case ADS1115_RANGE_1024:
            ADC_voltageRange = 1024;
            break;
        case ADS1115_RANGE_0512:
            ADC_voltageRange = 512;
            break;
        case ADS1115_RANGE_0256:
            ADC_voltageRange = 256;
            break;
    }
    
    /*
    if ((currentRange != range) && (currentAlertPinMode != ADS1115_DISABLE_ALERT)){
        alertLimit = ADC_readRegister(ADS1115_HI_THRESH_REG);
        alertLimit = alertLimit * (currentVoltageRange * 1.0 / ADC_voltageRange);
        ADC_writeRegister(ADS1115_HI_THRESH_REG, alertLimit);
        
        alertLimit = ADC_readRegister(ADS1115_LO_THRESH_REG);
        alertLimit = alertLimit * (currentVoltageRange * 1.0 / ADC_voltageRange);
        ADC_writeRegister(ADS1115_LO_THRESH_REG, alertLimit);
    }
    */
    
    currentConfReg &= ~(0x8E00);    
    currentConfReg |= range;
    ADC_writeRegister(ADS1115_CONFIG_REG, currentConfReg);
    
    /*
    convRate rate = getConvRate();
    delayAccToRate(rate);
     */
    vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
}
//------------------------------------------------------------------------------     
convRate ADC_getConvRate(void)
{    
uint16_t currentConfReg;

    currentConfReg = ADC_readRegister(ADS1115_CONFIG_REG);
    return (convRate)(currentConfReg & 0xE0);
}
//------------------------------------------------------------------------------
void ADC_setCompareChannels(ADS1115_MUX mux)
{
    
uint16_t currentConfReg = ADC_readRegister(ADS1115_CONFIG_REG);
//convRate rate;
        
    currentConfReg &= ~(0xF000);    
    currentConfReg |= (mux);
    ADC_writeRegister(ADS1115_CONFIG_REG, currentConfReg);
    
    //
    /*
    if(!(currentConfReg & 0x0100)){  // => if not single shot mode
        rate = ADC_getConvRate();      
        for(int i=0; i<2; i++){ // waiting time for two measurements
            //delayAccToRate(rate);
            vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
        }                 
    }
     */
    //
}
//------------------------------------------------------------------------------
void ADC_startSingleMeasurement(void)
{
    
uint16_t currentConfReg;

    currentConfReg = ADC_readRegister(ADS1115_CONFIG_REG);
    currentConfReg |= (1 << 15);
    ADC_writeRegister(ADS1115_CONFIG_REG, currentConfReg);
}
//------------------------------------------------------------------------------
bool ADC_isBusy(void)
{
    
uint16_t currentConfReg;

    currentConfReg = ADC_readRegister(ADS1115_CONFIG_REG);
    return (!(currentConfReg>>15) & 1);
}
//------------------------------------------------------------------------------
float ADC_getResult_V(void)
{
float result;

    result = ADC_getResult_mV();
    result /= 1000;
    return (result);  
}
//------------------------------------------------------------------------------
float ADC_getResult_mV(void)
{
int16_t rawResult;
int16_t rawC2;

float result = 0.0;

    rawResult = ADC_getRawResult();
    rawC2 = twosCompToDec(rawResult);
    
    xprintf_P(PSTR("RAW=0x%02x, C2=0x%02x (%d)\r\n"), rawResult, rawC2);
    // Complemento a 2
    
    result = (rawResult * 1.0 / ADS1115_REG_FACTOR) * ADC_voltageRange;
    xprintf_P(PSTR("MV=%d\r\n"), result);
    return (result);
}
//------------------------------------------------------------------------------
int16_t ADC_getRawResult(void)
{
int16_t rawResult;
//int16_t rawResultCopy;

    rawResult = ADC_readRegister(ADS1115_CONV_REG);
    
    /* bool autoRangeMode;
     * Autorange es FALSE
    
    if(autoRangeMode){
        rawResultCopy = rawResult;
        if(rawResultCopy == -32768){
            rawResultCopy++; 
        }
        rawResultCopy = abs(rawResultCopy);
        if((rawResultCopy > 26214) && (voltageRange != 6144)){ // 80%
            setAutoRange();
            rawResult = readRegister(ADS1115_CONV_REG);
        }
        else if((rawResultCopy < 9800) && (voltageRange != 256)){ //30%
            setAutoRange();
            rawResult = readRegister(ADS1115_CONV_REG);
        }
    }
    */
    return rawResult;
}
//------------------------------------------------------------------------------
float ADC_readChannel(ADS1115_MUX channel) 
{
  
float voltage = 0.0;
  
    ADC_setCompareChannels(channel);
    ADC_startSingleMeasurement();
    while(ADC_isBusy()) {
        ;
    }
    voltage = ADC_getResult_V(); // alternative: getResult_mV for Millivolt
    
    return voltage;
}
//------------------------------------------------------------------------------
int16_t twosCompToDec(uint16_t two_compliment_val)
{
    // [0x0000; 0x7FFF] corresponds to [0; 32,767]
    // [0x8000; 0xFFFF] corresponds to [-32,768; -1]
    // int16_t has the range [-32,768; 32,767]

    uint16_t sign_mask = 0x8000;

    // if positive
    if ( (two_compliment_val & sign_mask) == 0 ) {
        return two_compliment_val;
    //  if negative
    } else {
        // invert all bits, add one, and make negative
        return -(~two_compliment_val + 1);
    }
}
