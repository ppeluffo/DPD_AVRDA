
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
                 ADS1115_PGA_4_096 |
                 ADS1115_MUX_AIN0_AIN1 |
                 ADS1115_OS_OFF;
        
    //config_register = 0xC383;
    
    //xprintf_P(PSTR("DEBUG: ADC_init Config register = 0x%04x\r\n"), config_register);
    
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
    
    //xprintf_P(PSTR("DEBUG: ADC_start_conversion Config register = 0x%02x\r\n"), config_register);
    // Indico la direccion del registro a configurar
    ADC1115_write ( CONFIG_REG_ADDR, (char *)&config_register, 2, false);
}
//------------------------------------------------------------------------------
float ADC1115_multiple_read(uint8_t times)
{
    /*
     * Leo times veces y promedio
     */
    
uint16_t config_register = 0x00;
uint8_t data[2];
uint16_t val;
float total = 0;
uint8_t i;

  
    for (i=0; i<times; i++) {
        // Inicializo para leer 1 vez
        ADC1115_read ( CONFIG_REG_ADDR, (char *)&config_register, 2, false);
        config_register |= OS_START_CONVERTION_gc;
        ADC1115_write ( CONFIG_REG_ADDR, (char *)&config_register, 2, false);
        ADC1115_start_conversion();
        while( !ADC1115_is_conversion_done() )
            vTaskDelay(50);
        // Leo
        ADC1115_read ( CONVERSION_REG_ADDR, (char *)&data, 2, false);
        val = ( data[0] << 8 | data[1] ); 
        total += val;
        //xprintf_P(PSTR("mr%02d: data=%u, msb=0x%02x, lsb=0x%02x, total=%lu\r\n"), i, val, data[1], data[0], total);
    }

    total = total / times;
    return(total);
       
}
//------------------------------------------------------------------------------
uint32_t ADC1115_multiple_read_mV(uint8_t times)
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
        ADC1115_start_conversion();
        while( !ADC1115_is_conversion_done() );
        //vTaskDelay(50);
        // Leo
        ADC1115_read ( CONVERSION_REG_ADDR, (char *)&data, 2, false);
        val = ( data[1] << 8 | data[0] ); 
        total += val;
        xprintf_P(PSTR("mr%02d: data=%u, msb=0x%02x, lsb=0x%02x, total=%lu\r\n"), i, val, data[1], data[0], total);
    }

    total = (total / times)*0.125; //multiplico por el paso en la escala correspondiente (+-4.096V)
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
    
    //xprintf_P(PSTR("DEBUG: ADC_is_conversion_done Config register = 0x%02x\r\n"), config_register);
    
    return( config_register & CONVERTION_DONE_bm);
}
//------------------------------------------------------------------------------
uint16_t ADC1115_get_conversion_result(void)
{

uint16_t convertion_results = 0x00;
uint8_t data[2];

    //xprintf_P(PSTR("DEBUG: ADC_get_conversion_result\r\n"));
    
    // Indico la direccion del registro a configurar
    // Escribo el registro
    ADC1115_read ( CONVERSION_REG_ADDR, (char *)&data, 2, false);
    convertion_results = ( data[1] << 8 | data[0] ); 
    //xprintf_P(PSTR("DEBUG: AADC1115_get_conversion_result: result = [0x%02x][0x%02x]\r\n"), data[1], data[0]);
    
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
                 ADS1115_PGA_4_096 |
                 ADS1115_MUX_AIN0_AIN1 |
                 ADS1115_OS_SINGLE;
	
    // Escribo el registro
    //xprintf_P(PSTR("DEBUG: ADC1115_read_single_ended: config = 0x%02x\r\n"), adc_config);
    ADC1115_write ( CONFIG_REG_ADDR, (char *)&adc_config, 2, false);
    //ADC1115_get_config(false);
    ADC1115_start_conversion();
    _delay_us(25);
    while( !ADC1115_is_conversion_done() );
    //vTaskDelay(50);
    
    // Espero la conversion y leo
    ADC1115_read ( CONVERSION_REG_ADDR, (char *)&adc_data, 2, false);
    //xprintf_P(PSTR("DEBUG: ADC1115_read_single_ended: result = [0x%02x][0x%02x]\r\n"), adc_data[1], adc_data[0]);
    resVal = adc_data[0] << 8 | adc_data[1]; 
    
    return(resVal);
}
//------------------------------------------------------------------------------
uint16_t ADC1115_read_single_ended_mV(void)
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
                 ADS1115_PGA_4_096 |
                 ADS1115_MUX_AIN0_AIN1 |
                 ADS1115_OS_SINGLE;
	
    ADC1115_write ( CONFIG_REG_ADDR, (char *)&adc_config, 2, false);
    ADC1115_start_conversion();
    _delay_us(25);
    while( !ADC1115_is_conversion_done() );
    
    // Espero la conversion y leo
    ADC1115_read ( CONVERSION_REG_ADDR, (char *)&adc_data, 2, false);
    //xprintf_P(PSTR("DEBUG: ADC1115_read_single_ended: result = [0x%02x][0x%02x]\r\n"), adc_data[1], adc_data[0]);
    resVal = adc_data[0] << 8 | adc_data[1]; 
    resVal = resVal*0.125;//multiplico por el paso en la escala correspondiente (+-4.096V)
    return(resVal);
}
//------------------------------------------------------------------------------
void ADC1115_get_config(bool debug)
{    
uint8_t adc_config[2];

    ADC1115_read(CONFIG_REG_ADDR, (char *) &adc_config, 2, debug);
    xprintf_P(PSTR("ADC1115_CONFIG_REG: HB=0x%02x, LB=0x%02x  \r\n"), adc_config[1], adc_config[0]);
}
//------------------------------------------------------------------------------
int16_t ADC1115_read( uint16_t rdAddress, char *data, uint8_t length, bool debug )
{
int16_t rcode = 0;

    if (debug){
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

    if (debug){
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

