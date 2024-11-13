/*
 * File:   tkCtl.c
 * Author: pablo
 *
 * Created on 25 de octubre de 2021, 12:50 PM
 */


#include "DPD_AVRDA.h"

#define TKCTL_DELAY_S   1

void sys_watchdog_check(void);
void sys_daily_reset(void);
void sys_send_data(void);
void sys_medir(void);

dataRcd_s dr;

//------------------------------------------------------------------------------
void tkCtl(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;
//uint16_t a;

	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );
    xprintf_P(PSTR("Starting tkCtl..\r\n"));
      
    WDG_INIT(); // Pone todos los bits habilitados en 1
     
	// Arranco el RTC. Si hay un problema lo inicializo.
    RTC_init();

    vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );

    // Leo la configuracion de EE en systemConf
    xprintf_P(PSTR("Loading config...\r\n"));
    if ( ! u_load_config_from_NVM())  {
       xprintf_P(PSTR("Loading config default..\r\n"));
       u_config_default();
    }
    
    // Por ultimo habilito a todas las otras tareas a arrancar
    starting_flag = true;
    
    // Inicializo para tomar la primer medida en 5 minutos
    systemVars.time2medida = 120;
    systemVars.midiendo = false;
    memcpy(&systemVars.timestamp[0], "000000-000000", sizeof(systemVars.timestamp));
    
 	for( ;; )
	{
        // Duerme 5 secs y corre.
		vTaskDelay( ( TickType_t)( 1000 * TKCTL_DELAY_S / portTICK_PERIOD_MS ) );
        led_flash();
        sys_watchdog_check();
        sys_daily_reset();
        
        // Una vez por minuto mando datos al servidor
        sys_send_data();
        
        // Cada timerpoll disparo una medida
        sys_medir();   
	}
}
//------------------------------------------------------------------------------
void sys_watchdog_check(void)
{
    // El watchdog se inicializa en 2F.
    // Cada tarea debe poner su bit en 0. Si alguna no puede, se resetea
    // Esta funcion se corre cada 5s (TKCTL_DELAY_S)
    
static uint16_t wdg_count = 0;
uint8_t i;

    //xprintf_P(PSTR("wdg reset\r\n"));
    wdt_reset();
    return;
 
    // EL wdg lo leo cada 30 secs ( 5 x 6 )
    if ( wdg_count++ <  (30 / TKCTL_DELAY_S ) ) {
        return;
    }
   
    //xprintf_P(PSTR("DEBUG: wdg check\r\n"));
    wdg_count = 0;
    
    // Analizo los watchdows individuales
    //xprintf_P(PSTR("tkCtl: check wdg [0x%02X]\r\n"), sys_watchdog );
    for (i = 0; i < 8; i++) {
        // Si la tarea esta corriendo...
        if ( (task_running & ( 1<<i)) == 1) {
            // Si el wdg esta en 1 es que no pudo borrarlo !!!
            if ( (sys_watchdog & ( 1<<i)) == 1 ) {
                xprintf_P(PSTR("ALARM !!!. TKCTL: RESET BY WDG %d [0x%02X]\r\n"), i, sys_watchdog );
                vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
                reset();
            }
        }
    }

    WDG_INIT();

}
//------------------------------------------------------------------------------
void sys_daily_reset(void)
{
	// Todos los dias debo resetearme para restaturar automaticamente posibles
	// problemas.
	// Se invoca 1 vez por minuto ( 60s ).

static uint32_t ticks_to_reset = 86400 / TKCTL_DELAY_S ; // ticks en 1 dia.

	//xprintf_P( PSTR("DEBUG dailyReset..\r\n\0"));
	while ( --ticks_to_reset > 0 ) {
		return;
	}

	xprintf_P( PSTR("Daily Reset !!\r\n\0") );
    vTaskDelay( ( TickType_t)( 2000 / portTICK_PERIOD_MS ) );
    reset();
    
}
//------------------------------------------------------------------------------
void sys_send_data(void)
{
    
static int16_t timer = 30;
bool f_status;
    /*
     * tkCtl invoca esta función cada TKCTL_DELAY_S segundos
     */
    
    timer -= TKCTL_DELAY_S;

    if ( timer <= 0 ) {
        
        //xprintf_P(PSTR("Send data\r\n"));
        
        timer = systemConf.timerpoll;
        
        // Armo el frame:
        dr.absorbancia = systemVars.absorbancia;
        dr.cloro_ppm = systemVars.absorbancia;
        dr.S0 = systemVars.S0;
        dr.S100 = systemVars.S100;
        memcpy(&dr.timestamp[0], &systemVars.timestamp[0], sizeof(systemVars.timestamp));
        
         // Agrego el timestamp.
        f_status = RTC_read_dtime( &dr.rtc);
        if ( ! f_status ) {
            xprintf_P(PSTR("sys_send_data: ERROR I2C RTC:data_read_inputs\r\n"));
        }
        
        WAN_process_data_rcd(&dr);
    }
}
//------------------------------------------------------------------------------
void sys_medir(void)
{
    systemVars.time2medida -= TKCTL_DELAY_S;
    
    if (systemVars.time2medida <= 10 ) {
        // Tiempo de medir cloro
        xprintf_P(PSTR("MEDIR CLORO x TIMER\r\n"));
        systemVars.time2medida = systemConf.timermedida;
        // Pongo la orden de medir
        action_medida_completa(true);
    }
}
//------------------------------------------------------------------------------
