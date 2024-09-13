
#include "adc.h"
#include "DPD_AVRDA.h"

#define ADC_SAMPLES 32

uint16_t adc_buffer[32];

bool f_adc_debug;

//------------------------------------------------------------------------------
void ADC_set_debug(void) {
    f_adc_debug = true;
}
//------------------------------------------------------------------------------
void ADC_clear_debug(void) {
    f_adc_debug = false;
}
//------------------------------------------------------------------------------
/**
 * \brief Initialize ADC interface
 * If module is configured to disabled state, the clock to the ADC is disabled
 * if this is supported by the device's clock system.
 *
 * \return Initialization status.
 * \retval 0 the ADC init was successful
 * \retval 1 the ADC init was not successful
 */
int8_t ADC_init()
{

	//ADC0.CTRLB = ADC_SAMPNUM_NONE_gc; /* No accumulation */
    //ADC0.CTRLB = ADC_SAMPNUM_ACC8_gc; // 8 cuentas
    ADC0.CTRLB = ADC_SAMPNUM_ACC16_gc; // 16 cuentas
    
	ADC0.CTRLC = ADC_PRESC_DIV32_gc; /* CLK_PER divided by 2 */

	//ADC0.CTRLD = 0x0 << ADC_SAMPDLY_gp /* Sampling Delay Selection: 0x0 */
	//		 | ADC_INITDLY_DLY0_gc; /* Delay 0 CLK_ADC cycles */

    ADC0.CTRLD = ADC_INITDLY_DLY64_gc | ADC_SAMPDLY_DLY10_gc;
    
	ADC0.CTRLE = ADC_WINCM_NONE_gc; /* No Window Comparison */

	ADC0.DBGCTRL = 0 << ADC_DBGRUN_bp; /* Debug run: disabled */

	ADC0.EVCTRL = 0 << ADC_STARTEI_bp; /* Start Event Input Enable: disabled */

	ADC0.INTCTRL = 0 << ADC_RESRDY_bp /* Result Ready Interrupt Enable: disabled */
			 | 0 << ADC_WCMP_bp; /* Window Comparator Interrupt Enable: disabled */

	// ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc; /* ADC input pin 0 */
        
	ADC0.SAMPCTRL = 0x0 << ADC_SAMPLEN_gp; /* Sample length: 0x0 */

	ADC0.WINHT = 0x0; /* Window Comparator High Threshold: 0x0 */

	ADC0.WINLT = 0x0; /* Window Comparator Low Threshold: 0x0 */

	ADC0.CTRLA = 1 << ADC_ENABLE_bp      /* ADC Enable: enabled */
	             | 0 << ADC_FREERUN_bp   /* ADC Freerun mode: disabled */
	             | ADC_RESSEL_12BIT_gc   /* 12-bit mode */
	             | 0 << ADC_RUNSTBY_bp   /* Run standby mode: disabled */
	             | 0 << ADC_LEFTADJ_bp   /* Left Adjust Result: disabled */
	             | 0 << ADC_CONVMODE_bp; /* Differential Mode Conversion: disabled */

    /*
     Para usar el ADC debemos configurar su voltaje de referencia.
     */
    VREF.ADC0REF = VREF_REFSEL_VDD_gc | VREF_ALWAYSON_bm;
    //VREF.ADC0REF = VREF_REFSEL_2V500_gc | VREF_ALWAYSON_bm;
    //VREF.ADC0REF = VREF_REFSEL_VREFA_gc | VREF_ALWAYSON_bm;
    
	return 0;
}
//------------------------------------------------------------------------------
/**
 * \brief Enable ADC_0
 * 1. If supported by the clock system, enables the clock to the ADC
 * 2. Enables the ADC module by setting the enable-bit in the ADC control register
 *
 * \return Nothing
 */
void ADC_enable()
{
    if (f_adc_debug) {
        xprintf_P(PSTR("ADC_enable:\r\n"));
    }
    
	ADC0.CTRLA |= ADC_ENABLE_bm;
    vTaskDelay( ( TickType_t)( 100 ) );
}
//------------------------------------------------------------------------------
/**
 * \brief Disable ADC_0
 * 1. Disables the ADC module by clearing the enable-bit in the ADC control register
 * 2. If supported by the clock system, disables the clock to the ADC
 *
 * \return Nothing
 */
void ADC_disable()
{
    if (f_adc_debug) {
        xprintf_P(PSTR("ADC_disable:\r\n"));
    }
	ADC0.CTRLA &= ~ADC_ENABLE_bm;
    vTaskDelay( ( TickType_t)( 100 ) );
}
//------------------------------------------------------------------------------
/**
 * \brief Start a conversion on ADC_0
 *
 * \param[in] channel The ADC channel to start conversion on
 *
 * \return Nothing
 */
void ADC_start_conversion(adc_0_channel_t channel)
{
    if (f_adc_debug) {
        xprintf_P(PSTR("ADC_start_conversion: channel=%d\r\n"), channel);
    }
	ADC0.CTRLA &= ~ADC_CONVMODE_bm;
	ADC0.MUXPOS  = channel;
	ADC0.COMMAND = ADC_STCONV_bm;
}
//------------------------------------------------------------------------------
/**
 * \brief Start a differential conversion on ADC_0
 *
 * \param[in] channel,channel1 The ADC channels to start conversion on
 *
 * \return Nothing
 */
void ADC_start_diff_conversion(adc_0_channel_t channel, adc_0_muxneg_channel_t channel1)
{
	ADC0.CTRLA |= ADC_CONVMODE_bm;
	ADC0.MUXPOS  = channel;
	ADC0.MUXNEG  = channel1;
	ADC0.COMMAND = ADC_STCONV_bm;
}
//------------------------------------------------------------------------------
/**
 * \brief Stop a conversion on ADC_0
 *
 * \return Nothing
 */
void ADC_stop_conversion()
{
    if (f_adc_debug) {
        xprintf_P(PSTR("ADC_stop_conversion:\r\n"));
    }
	ADC0.COMMAND = ADC_SPCONV_bm;
} 
//------------------------------------------------------------------------------
/**
   * \brief Check if the ADC conversion is done
   *
   * \return The status of ADC converison done check
   * \retval true The ADC conversion is done
   * \retval false The ADC converison is not done
   */
bool ADC_is_conversion_done()
{
	return (ADC0.INTFLAGS & ADC_RESRDY_bm);
}
//------------------------------------------------------------------------------
/**
 * \brief Read a conversion result from ADC_0
 *
 * \return Conversion result read from the ADC_0 ADC module
 */
adc_result_t ADC_get_conversion_result(void)
{
    
 uint16_t adcVal;
 
    adcVal = ADC0.RES;
    adcVal = adcVal >> ADC_SHIFT_DIV16;
 
    if (f_adc_debug) {
        xprintf_P(PSTR("ADC_get_conversion_result: acc=0x%02x(%u), val=%d\r\n"), ADC0.RES, ADC0.RES, adcVal);
    }
     
	return (adcVal);
}
//------------------------------------------------------------------------------
/**
 * \brief Start a conversion, wait until ready, and return the conversion result
 *
 * \return Conversion result read from the ADC_0 ADC module
 */
adc_result_t ADC_get_conversion(adc_0_channel_t channel)
{
	adc_result_t res;

    if (f_adc_debug) {
        xprintf_P(PSTR("ADC_get_conversion: channel=%d\r\n"), channel );
    }
	ADC_start_conversion(channel);
	while (!ADC_is_conversion_done()) {
        vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
    }
	res = ADC_get_conversion_result();
	ADC0.INTFLAGS |= ADC_RESRDY_bm;
	return res;
}
//------------------------------------------------------------------------------
/**
 * \brief Start a differential conversion, wait until ready, and return the conversion result
 *
 * \return Conversion result read from the ADC_0 ADC module
 */
diff_adc_result_t ADC_get_diff_conversion(adc_0_channel_t channel, adc_0_muxneg_channel_t channel1)
{
	diff_adc_result_t res;

	ADC_start_diff_conversion(channel, channel1);
	while (!ADC_is_conversion_done())
		;
	res = ADC_get_conversion_result();
	ADC0.INTFLAGS |= ADC_RESRDY_bm;
	return res;
} 
//------------------------------------------------------------------------------
/**
   * \brief Return the number of bits in the ADC conversion result
   *
   * \return The number of bits in the ADC conversion result
   */
uint8_t ADC_get_resolution()
{
	return (ADC0.CTRLA & ADC_RESSEL0_bm) ? 10 : 12;
}
//------------------------------------------------------------------------------
float ADC_convert_to_volts(uint16_t counts)
{
   float volts = 0.0;
   
   volts = systemVars.adc_cal_volts * counts / systemVars.adc_cal_factor;
   //xprintf_P(PSTR("ADC convert = %0.3f mV\r\n"), volts);
   
   return (volts);
   
}
//------------------------------------------------------------------------------
uint16_t ADC_read_single(void)
{
    
uint16_t adc_acc;

    if (f_adc_debug) {
        xprintf_P(PSTR("ADC_read_single:\r\n"));
    }

    ADC_enable();
    adc_acc = ADC_get_conversion(ADC_MUXPOS_AIN11_gc); /* ADC input pin 12 */
    ADC_disable();
    
    return( adc_acc);    
}
//------------------------------------------------------------------------------
uint16_t ADC_read_multiple(uint8_t samples, bool debug)
{
    
uint8_t i;
uint16_t adc; 
float adc_acc;

    if ( ( samples == 0) || (samples > 128)) {
        return(0);
    }

    // Muestreo

    ADC_enable();
    for (i=0; i<samples; i++) {
         adc = ADC_get_conversion(ADC_MUXPOS_AIN11_gc);
         adc_acc += (float)(adc);
         vTaskDelay( ( TickType_t)( 10 ) );
         if (debug) {
             //xprintf_P(PSTR("s%02d=%u, %0.3f\r\n"), i, adc, adc_acc);
             xprintf_P(PSTR("%u,"), adc);
         }    
    }
    if (debug) {
        xprintf_P(PSTR("\r\n"));
    }
    ADC_disable();
        
    adc_acc /= samples;
    //xprintf_P(PSTR("ADC_RES=%u\r\n"), (uint16_t)(adc_acc) );
    //xprintf_P(PSTR("%u\r\n"), (uint16_t)(adc_acc) );
    return( (uint16_t) adc_acc);
    
}
//------------------------------------------------------------------------------
void ADC_test_read_single(void)
{
uint16_t adc_acc;
float volts = 0.0;

    adc_acc = ADC_read_single();
    volts = ADC_convert_to_volts(adc_acc);
    //xprintf_P(PSTR("ADC single = %d\r\n"), adc_acc);
    //xprintf_P(PSTR("ADC single = %0.3f V\r\n"), volts);
    xprintf_P(PSTR("%d\r\n"), adc_acc);
}
//------------------------------------------------------------------------------
void ADC_test_read_multiple( uint8_t times)
{

uint16_t adc_acc;
float volts = 0.0;

    adc_acc = ADC_read_multiple(times, false);
    volts = ADC_convert_to_volts(adc_acc);
    xprintf_P(PSTR("%d\r\n"), adc_acc);
    //xprintf_P(PSTR("ADC muti = %d\r\n"), adc_acc);
    //xprintf_P(PSTR("ADC multi = %0.3f V\r\n"), volts);    
}
//------------------------------------------------------------------------------
uint16_t ADC_calibrar(void)
{

uint16_t adc_acc;
//float mV = 0.0;

    adc_acc = ADC_read_multiple(128, false);
    //mV = ( 2500.0 * adc_acc) / 4096.0;
    xprintf_P(PSTR("ADC factor calibracion = %d\r\n"), adc_acc);
    //xprintf_P(PSTR("ADC multi = %0.3f mV\r\n"), mV); 
    return (adc_acc);
}
//------------------------------------------------------------------------------
// ACCIONES BASICAS
//------------------------------------------------------------------------------
void f_adc_read(bool f_debug, uint16_t times, uint16_t dummyarg0)
{

uint16_t adc_acc;

    if (f_debug)
        xprintf_P(PSTR("ADC read %d times\r\n"), times);

    adc_acc = ADC_read_multiple(times, false);
    xprintf_P(PSTR("%d\r\n"), adc_acc);
    return;
}
//------------------------------------------------------------------------------
