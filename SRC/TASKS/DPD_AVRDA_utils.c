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

    // Configuro a default todas las configuraciones locales
    pump_config_default();
    //
    systemVars.adc_cal_factor = 3400;
    systemVars.adc_cal_volts = 3.0;
    
}
//------------------------------------------------------------------------------
bool u_save_config_in_NVM(void)
{
   /*
    * Leo los valores de las freq.de las pumps y actualizao el systemVars
    * para luego guardrlo en la NVM
    
    */
    
int8_t retVal;
uint8_t cks;

    systemVars.pump_freq[0] = pump0.freq;
    systemVars.pump_freq[1] = pump1.freq;
    systemVars.pump_freq[2] = pump2.freq;

    cks = checksum ( (uint8_t *)&systemVars, ( sizeof(systemVars) - 1));
    systemVars.checksum = cks;

    retVal = NVMEE_write( 0x00, (char *)&systemVars, sizeof(systemVars) );
    xprintf_P(PSTR("SAVE NVM: memblock size = %d\r\n"), sizeof(systemVars));    
    //xprintf_P(PSTR("DEBUG: Save in NVM OK\r\n"));
    
    if (retVal == -1 )
        return(false);
    
    return(true);

   
}
//------------------------------------------------------------------------------
bool u_load_config_from_NVM(void)
{
    
uint8_t rd_cks, calc_cks;

    xprintf_P(PSTR("NVM: memblock size=%d\r\n"), sizeof(systemVars));

    memset( &systemVars, '\0', sizeof(systemVars));
    
    NVMEE_read( 0x00, (char *)&systemVars, sizeof(systemVars) );
    rd_cks = systemVars.checksum;
        
    calc_cks = checksum ( (uint8_t *)&systemVars, ( sizeof(systemVars) - 1));
    
    if ( calc_cks != rd_cks ) {
		xprintf_P( PSTR("ERROR: Checksum systemConf failed: calc[0x%0x], read[0x%0x]\r\n"), calc_cks, rd_cks );
		return(false);
	}

    pump_update_config();
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
void f_init_system(bool f_debug, uint16_t dummyarg0, uint16_t dummyarg1)
{
    if (f_debug)
        xprintf_P(PSTR("Init System\r\n"));
    
    f_opto_off(f_debug, 0, 0);
    f_valve_0_close(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    f_valve_1_close(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    f_valve_2_close(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
}
//------------------------------------------------------------------------------
void f_lavar_reservorio_muestra(bool f_debug, uint16_t dummyarg0, uint16_t dummyarg1)
{
    if (f_debug)
        xprintf_P(PSTR("Lavar reservorio muestra\r\n"));
        
    // Abrir válvula V0 para llenar reservorio.
    f_valve_0_open(f_debug, 0, 0);
    
    // Permitir llenado del reservorio. Es necesario llenarlo por lo menos 150 mL.
    vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );
    
    // Cerrar válvula V0.
    f_valve_0_close(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
    // Abrir válvula V1 para purgar reservorio.
    f_valve_1_open(f_debug, 0, 0);
    
    // Permitir vaciado del reservorio.
    vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );
    
    // Cerrar válvula V1.
    f_valve_1_close(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
}
//------------------------------------------------------------------------------
void f_llenar_reservorio_muestra(bool f_debug, uint16_t dummyarg0, uint16_t dummyarg1)
{
     if (f_debug)
        xprintf_P(PSTR("Llenar reservorio muestra\r\n"));
     
    // Abrir válvula V0 para llenar reservorio.
    f_valve_0_open(f_debug, 0, 0);
    
    // Permitir llenado del reservorio. Es necesario llenarlo por lo menos 150 mL.
    vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );
    
    // Cerrar válvula V0.
    f_valve_0_close(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
}
//------------------------------------------------------------------------------
void f_purga_canal_muestra (bool f_debug, uint16_t dummyarg0, uint16_t dummyarg1)
{
    /*
     *  Encender bomba M2 para purgar canal de muestra (la válvula V2 está abierta, por lo que se
     * descarta a través de ella).
     *  Apagar bomba M2 transcurridos Z segundos de encendida (a determinar).
     */
    if (f_debug)
        xprintf_P(PSTR("Purga canal muestra\r\n"));
    
    f_pump_2_run(f_debug, 15, 0);
    vTaskDelay( ( TickType_t)( 15000 / portTICK_PERIOD_MS ) );
}
//------------------------------------------------------------------------------
void f_lavado_celda(bool f_debug, uint16_t dummyarg0, uint16_t dummyarg1)
{
    
    if (f_debug)
        xprintf_P(PSTR("Lavado celda\r\n"));
    
    // Cerrar válvula V2.
    f_valve_2_close(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
    // Encender bomba M2 para llenar la celda de reacción con muestra.
    // Apagar bomba M2 transcurridos Z? segundos de encendida (a determinar).
    f_pump_2_run(f_debug, 15, 0);
    vTaskDelay( ( TickType_t)( 15000 / portTICK_PERIOD_MS ) );
    
    // Abrir válvula V2 para vaciar el contenido de la celda de reacción.
    f_valve_2_open(f_debug, 0, 0);
    
    vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );
    
    // Cerrar válvula V2.
    f_valve_2_close(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
}
//------------------------------------------------------------------------------
void f_ajustes_fotometricos(bool f_debug, uint16_t dummyarg0, uint16_t dummyarg1)
{
    /*
     * Encender bomba M2 para llenar la celda de reacción con muestra.
     * Apagar bomba M2 transcurridos Z? segundos de encendida (a determinar).
     * Asegurarse de que estén apagados los drivers (este paso es fundamental porque los drivers
     * introducen ruido significativamente en las lecturas fotométricas).
     */
    
    if (f_debug)
        xprintf_P(PSTR("Ajustes fotométricos\r\n"));
    
    f_pump_2_run(f_debug, 15, 0);
    vTaskDelay( ( TickType_t)( 15000 / portTICK_PERIOD_MS ) );
    
    // Asegurarse que el LED esté apagado.
    f_opto_off(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 2000 / portTICK_PERIOD_MS ) );
    
    /*
     *  Registrar la señal fotométrica y almacenar el promedio de 100 lecturas, lo que corresponde al
     * ajuste del 0%T (S0).
     */
    f_adc_read( f_debug, 128, 0);
    
    // Encender LED.
    f_opto_on(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 2000 / portTICK_PERIOD_MS ) );
    
    // Registrar la señal fotométrica y almacenar el promedio de 100 lecturas, lo que corresponde al
    // 100%T (S100).
    f_adc_read(f_debug, 128, 0);
    
    // Abrir válvula V2 para vaciar el contenido de la celda de reacción.
    f_valve_2_open(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );
    
}
//------------------------------------------------------------------------------
void f_medicion(bool f_debug, uint16_t dummyarg0, uint16_t dummyarg1)
{
    
    if (f_debug)
        xprintf_P(PSTR("Medicion\r\n"));
    
    // Cerrar válvula V2.
    f_valve_2_close(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
    // Encender bomba M0 para dispensar 0.5 mL de reactivo DPD.
    // Apagar bomba M0 transcurridos 10 segundos de encendida.
    f_pump_0_run(f_debug, 10, 0);
    vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );
    
    // Encender bomba M1 para dispensar 0.5 mL de buffer.
    // Apagar bomba M1 transcurridos 11.5 segundos de encendida.
    f_pump_1_run(f_debug, 10, 0);
    vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );
    
    // Encender bomba M2 para dispensar 10 mL de muestra.
    // Apagar bomba M2 transcurridos Z? segundos de encendida (a determinar).
    // Asegurarse de que estén apagados los drivers.
    f_pump_2_run(f_debug, 10, 0);
    vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );
    
    
    /*
     *  Esperar 5 segundos para estabilización de la señal fotométrica.
     *  Registrar la señal fotométrica y almacenar el promedio de 100 lecturas (Lprom). Calcular y
     * almacenar la absorbancia Aprom, que se calcula como:
     */
    // Encender LED.
    f_opto_on(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 2000 / portTICK_PERIOD_MS ) );
    
    // Registrar la señal fotométrica y almacenar el promedio de 100 lecturas, lo que corresponde al
    // 100%T (S100).
    f_adc_read(f_debug, 128, 0);
    
    // Abrir válvula V2 para vaciar el contenido de la celda de reacción.
    f_valve_2_open(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
}
//------------------------------------------------------------------------------
void f_lavado_final(bool f_debug, uint16_t dummyarg0, uint16_t dummyarg1)
{
    
    if (f_debug)
        xprintf_P(PSTR("Lavado final\r\n"));
    
    // Cerrar válvula V2.
    f_valve_2_close(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
    // Encender bomba M2 para llenar la celda de reacción con muestra.
    // Apagar bomba M2 transcurridos Z? segundos de encendida (a determinar).
    f_pump_2_run(f_debug, 15, 0);
    vTaskDelay( ( TickType_t)( 15000 / portTICK_PERIOD_MS ) );
    
    // Abrir válvula V2 para vaciar el contenido de la celda de reacción.
    f_valve_2_open(f_debug, 0, 0);
    
    vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );
    
    // Cerrar válvula V2.
    f_valve_2_close(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );

    // Abrir válvula V1 para purgar reservorio.
    f_valve_1_open(f_debug, 0, 0);
    
    vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );

    // Cerrar válvula V1.
    f_valve_1_close(f_debug, 0, 0);
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
    // Encender bomba M2 para vaciar el canal de muestra.
    // Apagar la bomba M2 transcurridos Z segundos de encendida (a determinar).
    f_pump_2_run(f_debug, 15, 0);
    vTaskDelay( ( TickType_t)( 15000 / portTICK_PERIOD_MS ) );
}
//------------------------------------------------------------------------------
void f_test_medir1(bool f_debug, uint16_t samples, uint16_t delay_secs)
{
    /*
     * TEST 0: Solo toma medidas. Samplea 64 veces por medida.
     * No importa si el led esta prendido o apagado.
     * 
     */
   
uint16_t adc_acc;
uint16_t i;

    if (f_debug)
        xprintf_P(PSTR("TEST medir1 start.\r\n"));

    for (i=0; i<samples; i++) {
        adc_acc = ADC_read_multiple(64, false);
        xprintf_P(PSTR("%d,%d\r\n"), i, adc_acc);
        vTaskDelay( ( TickType_t)( (delay_secs * 1000) / portTICK_PERIOD_MS ) );
        // ABORT ??       
    }

    if (f_debug)
        xprintf_P(PSTR("TEST medir1 end.\r\n"));
    
}
//------------------------------------------------------------------------------
void f_test_medir2(bool f_debug, uint16_t samples, uint16_t delay_secs)
{
    /*
     * TEST 0: Solo toma medidas. Samplea 64 veces por medida.
     * No importa si el led esta prendido o apagado.
     * 
     */
   
uint16_t adc_acc;
uint16_t i;

    if (f_debug)
        xprintf_P(PSTR("TEST medir2 start.\r\n"));

    for (i=0; i<samples; i++) {
        f_opto_on(f_debug, 0, 0);
        vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
        adc_acc = ADC_read_multiple(64, false);
        f_opto_off(f_debug, 0, 0);
        
        xprintf_P(PSTR("%d,%d\r\n"), i, adc_acc);
        vTaskDelay( ( TickType_t)( (delay_secs * 1000) / portTICK_PERIOD_MS ) );
        // ABORT ??       
    } 

    if (f_debug)
        xprintf_P(PSTR("TEST medir2 end.\r\n"));
}
//------------------------------------------------------------------------------
