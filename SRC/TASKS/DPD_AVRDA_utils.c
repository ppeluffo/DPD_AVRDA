/* 
 * File:   frtos20_utils.c
 * Author: pablo
 *
 * Created on 22 de diciembre de 2021, 07:34 AM
 */

#include "DPD_AVRDA.h"
#include "pines.h"

//------------------------------------------------------------------------------
int8_t WDT_init(void);
int8_t CLKCTRL_init(void);
uint8_t checksum( uint8_t *s, uint16_t size );

struct {
    
    systemConf_t systemConf;
    pumps_conf_t pumps_conf; 
    
    // El checksum SIEMPRE debe ser el ultimo byte !!!!!
    uint8_t checksum;
    
} memConfBuffer;

//-----------------------------------------------------------------------------
void system_init()
{

    // Init OUT OF RTOS !!!
    
	CLKCTRL_init();
    WDT_init();
    LED_init();
    XPRINTF_init();
    TOYI_VALVES_init();
    CNT0_CONFIG();
    
    pump0_init();
    pump1_init();
    pump2_init();
    
    CONFIG_OPTO();
    I2C_init();
        
    ADC_init();
    
    MODEM_init();
}
//-----------------------------------------------------------------------------
int8_t WDT_init(void)
{
	/* 8K cycles (8.2s) */
	/* Off */
	ccp_write_io((void *)&(WDT.CTRLA), WDT_PERIOD_8KCLK_gc | WDT_WINDOW_OFF_gc );  
	return 0;
}
//-----------------------------------------------------------------------------
int8_t CLKCTRL_init(void)
{
	// Configuro el clock para 24Mhz
	
	ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA), CLKCTRL_FRQSEL_24M_gc         /* 24 */
                                                
	| 0 << CLKCTRL_AUTOTUNE_bp /* Auto-Tune enable: disabled */
	| 0 << CLKCTRL_RUNSTDBY_bp /* Run standby: disabled */);

	// ccp_write_io((void*)&(CLKCTRL.MCLKCTRLA),CLKCTRL_CLKSEL_OSCHF_gc /* Internal high-frequency oscillator */
	//		 | 0 << CLKCTRL_CLKOUT_bp /* System clock out: disabled */);

	// ccp_write_io((void*)&(CLKCTRL.MCLKLOCK),0 << CLKCTRL_LOCKEN_bp /* lock enable: disabled */);

	return 0;
}
//-----------------------------------------------------------------------------
void reset(void)
{
    xprintf_P(PSTR("ALERT !!!. Going to reset...\r\n"));
    vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );
	/* Issue a Software Reset to initilize the CPU */
	ccp_write_io( (void *)&(RSTCTRL.SWRR), RSTCTRL_SWRST_bm ); 
                                           
}
//------------------------------------------------------------------------------
void u_kick_wdt( uint8_t wdg_gc)
{
    // Pone el bit correspondiente en 0.
    sys_watchdog &= ~wdg_gc;
    
}
//------------------------------------------------------------------------------
void u_config_default(void)
{

    memcpy(systemConf.dlgid, "DEFAULT", sizeof(DLGID_LENGTH));
    systemConf.timerpoll = 60;
    systemConf.timermedida = 10800;
    systemConf.adc_cal_factor = 3400;
    systemConf.adc_cal_volts = 3.0;
    
    // Configuro a default todas las configuraciones locales
    pump_config_default();
    
}
//------------------------------------------------------------------------------
bool u_save_config_in_NVM(void)
{
int8_t retVal;
uint8_t cks;

    memset( &memConfBuffer, '\0', sizeof(memConfBuffer));

    // Cargamos el buffer con las configuraciones
    memcpy( &memConfBuffer.systemConf, &systemConf, sizeof(systemConf_t));
    memcpy( &memConfBuffer.pumps_conf, &pumps_conf, sizeof(pumps_conf_t));

    cks = checksum ( (uint8_t *)&memConfBuffer, ( sizeof(memConfBuffer) - 1));
    memConfBuffer.checksum = cks;

    retVal = NVMEE_write( 0x00, (char *)&memConfBuffer, sizeof(memConfBuffer) );

    xprintf_P(PSTR("SAVE NVM: memblock size = %d\r\n"), sizeof(memConfBuffer));    
    
    if (retVal == -1 )
        return(false);
    
    return(true);

   
}
//------------------------------------------------------------------------------
bool u_load_config_from_NVM(void)
{
    
uint8_t rd_cks, calc_cks;

    xprintf_P(PSTR("NVM: memblock size=%d\r\n"), sizeof(memConfBuffer));

    memset( &memConfBuffer, '\0', sizeof(memConfBuffer));
    
    NVMEE_read( 0x00, (char *)&memConfBuffer, sizeof(memConfBuffer) );
    rd_cks = memConfBuffer.checksum;
        
    calc_cks = checksum ( (uint8_t *)&memConfBuffer, ( sizeof(memConfBuffer) - 1));
    
    if ( calc_cks != rd_cks ) {
		xprintf_P( PSTR("ERROR: Checksum systemConf failed: calc[0x%0x], read[0x%0x]\r\n"), calc_cks, rd_cks );
        return(false);
	} 
    
    // Desarmo el buffer de memoria
    memcpy( &systemConf, &memConfBuffer.systemConf, sizeof(systemConf_t));       
    memcpy( &pumps_conf, &memConfBuffer.pumps_conf, sizeof(pumps_conf_t));
    
    return(true);
}
//------------------------------------------------------------------------------
uint8_t checksum( uint8_t *s, uint16_t size )
{
	/*
	 * Recibe un puntero a una estructura y un tamaño.
	 * Recorre la estructura en forma lineal y calcula el checksum
	 */

uint8_t *p = NULL;
uint8_t cks = 0;
uint16_t i = 0;

	cks = 0;
	p = s;
	for ( i = 0; i < size ; i++) {
		 cks = (cks + (int)(p[i])) % 256;
	}

	return(cks);
}
//------------------------------------------------------------------------------
void SYSTEM_ENTER_CRITICAL(void)
{
    while ( xSemaphoreTake( sem_SYSVars, ( TickType_t ) 5 ) != pdTRUE )
  		vTaskDelay( ( TickType_t)( 10 ) );   
}
//------------------------------------------------------------------------------
void SYSTEM_EXIT_CRITICAL(void)
{
    xSemaphoreGive( sem_SYSVars );
}
//------------------------------------------------------------------------------
float cloro_from_absorbancia(float abs, bool debug)
{
    /*
     * Busca en la curva de calibracion el tramo que se ajusta a la absorbancia
     * dada y calcula el cloro equivalente
     */
    
uint8_t i;
uint8_t P0 = -1;
uint8_t P1 = -1;
float x0,x1,y0,y1;
float cloro;

    xprintf_P(PSTR("AbS2CL: %0.4f\r\n"), abs);

    if (abs < 0 ) {
        cloro = -1;
        goto quit;
    }
    // Busco el mejor tramo
    for (i=1; i<CAL_MAX_POINTS; i++) {
        
        if ( systemConf.xCal[i] == - 1) {
            // Se termino la curva y no hay tramos utiles
            xprintf_P(PSTR("ERROR en curva de calibracion. No hay tramos para %0.3f\r\n"), abs);
            return (-1.0);
        }
        
        if ( systemConf.xCal[i] > abs ) {
            // Encontre el tramo adecuado
            P0 = (i-1);
            P1 = i;
            break;
        }
    }
    
    if (P0 == -1 ) {
        // Termine de recorrer la curva y no encontre un tramos util
        xprintf_P(PSTR("ERROR en curva de calibracion. No hay tramos para %0.3f\r\n"), abs);
        return (-1.0);        
    }

    // El mejor tramo esta entre P0 y P1
    x0 = systemConf.xCal[P0];
    x1 = systemConf.xCal[P1];
    y0 = systemConf.yCal[P0];
    y1 = systemConf.yCal[P1];
    if (debug) {
        xprintf_P(PSTR("(X0=%0.3f,Y0=%0.3f}\r\n"), x0,y0);
        xprintf_P(PSTR("(X1=%0.3f,Y1=%0.3f}\r\n"), x1,y1);
    }
    
    cloro = ( (y1-y0)/(x1-x0))*(abs - x0) + y0;
    
quit:
    
    xprintf_P(PSTR("Absorbancia = %0.3f\r\n"), abs);
    xprintf_P(PSTR("Cloro(ppm) = %0.3f\r\n"), cloro);
    
    return(cloro);
    
}
//------------------------------------------------------------------------------
void u_data_resync_clock( char *str_time, bool force_adjust)
{
	/*
	 * Ajusta el clock interno de acuerdo al valor de rtc_s
     * Si force_adjust es TRUE siempre lo ajusta.
     * Si es FALSE, solo lo ajusta si la diferencia con la hora actual son mas
     * de 90 segundos
     * 
	 * Bug 01: 2021-12-14:
	 * El ajuste no considera los segundos entonces si el timerpoll es c/15s, cada 15s
	 * se reajusta y cambia la hora del datalogger.
	 * Modifico para que el reajuste se haga si hay una diferencia de mas de 90s entre
	 * el reloj local y el del server
	 */


float diff_seconds;
RtcTimeType_t rtc_l, rtc_wan;
int8_t xBytes = 0;
   
    // Convierto el string YYMMDDHHMM a RTC.
    //xprintf_P(PSTR("DATA: DEBUG CLOCK2\r\n") );
    memset( &rtc_wan, '\0', sizeof(rtc_wan) );        
    RTC_str2rtc( str_time, &rtc_wan);
    //xprintf_P(PSTR("DATA: DEBUG CLOCK3\r\n") );
            
            
	if ( force_adjust ) {
		// Fuerzo el ajuste.( al comienzo )
		xBytes = RTC_write_dtime(&rtc_wan);		// Grabo el RTC
		if ( xBytes == -1 ) {
			xprintf_P(PSTR("ERROR: CLOCK: I2C:RTC:pv_process_server_clock\r\n"));
		} else {
			xprintf_P( PSTR("CLOCK: Update rtc.\r\n") );
		}
		return;
	}

	// Solo ajusto si la diferencia es mayor de 90s
	// Veo la diferencia de segundos entre ambos.
	// Asumo yy,mm,dd iguales
	// Leo la hora actual del datalogger
	RTC_read_dtime( &rtc_l);
	diff_seconds = abs( rtc_l.hour * 3600 + rtc_l.min * 60 + rtc_l.sec - ( rtc_wan.hour * 3600 + rtc_wan.min * 60 + rtc_wan.sec));
	//xprintf_P( PSTR("COMMS: rtc diff=%.01f\r\n"), diff_seconds );

	if ( diff_seconds > 90 ) {
		// Ajusto
		xBytes = RTC_write_dtime(&rtc_wan);		// Grabo el RTC
		if ( xBytes == -1 ) {
			xprintf_P(PSTR("ERROR: CLOCK: I2C:RTC:pv_process_server_clock\r\n"));
		} else {
			xprintf_P( PSTR("CLOCK: Update rtc\r\n") );
		}
		return;
	}
}
//------------------------------------------------------------------------------