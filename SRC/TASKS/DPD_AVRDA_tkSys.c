/*
 * File:   tkSystem.c
 * Author: pablo
 *
 * Espera timerPoll y luego muestra los valores de las entradas analogicas.
 * 
 */


#include "DPD_AVRDA.h"

//------------------------------------------------------------------------------
void tkSys(void * pvParameters)
{
    /*
     * Espero timerpoll y poleo todos los canales, dejando los datos en 
     * un dataRcd.
     * Le paso el dataRcd a la tarea de WAN para que lo envie o lo almacene
     * Finalmente muestro los datos en la terminal
     * 
     * TickType_t puede ser uint16 o uint32. Si usamos configUSE_16_BIT_TICKS 0
     * entonces es de 32 bits.
     * 
     */
TickType_t xLastWakeTime;

	while (!starting_flag )
		vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );
    
    SYSTEM_ENTER_CRITICAL();
    task_running |= SYS_WDG_gc;
    SYSTEM_EXIT_CRITICAL();
    
    xprintf_P(PSTR("Starting tkSys..\r\n"));
        
    // Espero solo 10s para el primer poleo ( no lo almaceno !!)
    vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );
    
	for( ;; )
	{
       
        xLastWakeTime = xTaskGetTickCount();
               
        u_kick_wdt(SYS_WDG_gc);
        vTaskDelayUntil( &xLastWakeTime, ( 10000 / portTICK_PERIOD_MS ) );
    }

}
//------------------------------------------------------------------------------
