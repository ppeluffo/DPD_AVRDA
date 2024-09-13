/*
 * File:   tkSystem.c
 * Author: pablo
 *
 * Espera timerPoll y luego muestra los valores de las entradas analogicas.
 * 
 */


#include "DPD_AVRDA.h"

void test0(void);
void test1(void);
void test2(void);

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

	while (!starting_flag )
		vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );
    
    vTaskDelay( ( TickType_t)( 250 / portTICK_PERIOD_MS ) );
    
    SYSTEM_ENTER_CRITICAL();
    task_running |= SYS_WDG_gc;
    SYSTEM_EXIT_CRITICAL();
    
    xprintf_P(PSTR("Starting tkSys..\r\n"));
        
    // Para evitar la primera conversion erronea ....
    ADC_read_multiple(64, false);
    
    testCB.standby = true;
    
	for( ;; )
	{         
        u_kick_wdt(SYS_WDG_gc);
        
        if ( testCB.standby ) {
            // No hago nada. Espero
            vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
            
        } else {
            
            // Ejecuto la funcion y reseteo el puntero
            testCB.fn( testCB.param1, testCB.param2, testCB.param3);
            testCB.fn = NULL;
            testCB.standby = true;
        }
    }
}
//------------------------------------------------------------------------------
void tkSys__(void * pvParameters)
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

	while (!starting_flag )
		vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );
    
    vTaskDelay( ( TickType_t)( 250 / portTICK_PERIOD_MS ) );
    
    SYSTEM_ENTER_CRITICAL();
    task_running |= SYS_WDG_gc;
    SYSTEM_EXIT_CRITICAL();
    
    xprintf_P(PSTR("Starting tkSys..\r\n"));
        
    // Para evitar la primera conversion erronea ....
    ADC_read_multiple(64, false);
    
    testCB.standby = true;
    
	for( ;; )
	{         
        u_kick_wdt(SYS_WDG_gc);
        
        if ( testCB.standby ) {
            // No hago nada. Espero
            vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
            
        } else {
            
            // Ejecuto el test
            switch(testCB.test_id) {
                case 0:
                    test0();
                    break;
                case 1:
                    test1();
                    break;
                case 2:
                    test2();
                    break;
                case 3:
                    test3();
                    break;
                default:
                    xprintf_P(PSTR("ERROR: test no implementado!!\r\n"));
                    testCB.standby = true;
                    break;
            }
          
            testCB.counts++;
            if ( testCB.counts > testCB.max_counts ) {
                // Llegue al maximo: detengo el test
                testCB.standby = true;
                xprintf_P(PSTR("Test End.\r\n"));
            } else {
                // Espero para el siguiente
                vTaskDelay( ( TickType_t)( (testCB.delay_secs * 1000) / portTICK_PERIOD_MS ) );
            }
        }
    }

}
//------------------------------------------------------------------------------
void test0(void)
{
    /*
     * TEST 0: Solo toma medidas. Samplea 64 veces por medida.
     * No importa si el led esta prendido o apagado.
     * 
     */
   
uint16_t adc_acc;

    //xprintf_P(PSTR("Test0: %d\r\n"), testCB.counts);
    adc_acc = ADC_read_multiple(64, false);
    xprintf_P(PSTR("%d,%d\r\n"), testCB.counts, adc_acc);
    
}
//------------------------------------------------------------------------------
void test1(void)
{
   /*
     * TEST 1: Solo toma medidas. Samplea 64 veces por medida.
     * No importa si el led esta prendido o apagado.
     * Genera una linea con c/u de las 64 medidas de c/muestra
     * a efectos de hacer un debug.  
     * 
     */
   
uint16_t adc_acc;

    //xprintf_P(PSTR("Test0: %d\r\n"), testCB.counts);
    xprintf_P(PSTR("%d,"), testCB.counts);
    adc_acc = ADC_read_multiple(64, true);
    xprintf_P(PSTR("%d\r\n"), adc_acc);
}
//-----------------------------------------------------------------------------
void test2(void)
{
   /*
     * TEST 2: Solo toma medidas. Samplea 64 veces por medida.
     * No importa si el led esta prendido o apagado.
     * Genera una linea con c/u de las 64 medidas de c/muestra
     * a efectos de hacer un debug.  
     * 
     */
   
uint16_t adc_acc;

    //xprintf_P(PSTR("Test0: %d\r\n"), testCB.counts);
    opto_test("ON");
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    adc_acc = ADC_read_multiple(64, false);
    xprintf_P(PSTR("%d,%d\r\n"), testCB.counts, adc_acc);
    opto_test("OFF");
}
//-----------------------------------------------------------------------------
void test3(void)
{
    /*
     * Se prende el led.
     * Se esperan 5s y se miden 64 samples
     * Se apaga el led.
     
     */
    //xprintf_P(PSTR("Test2: %d\r\n"), testCB.counts);
   
uint16_t adc_acc;

    opto_test("ON");
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    xprintf_P(PSTR("%d,"), testCB.counts);
    adc_acc = ADC_read_multiple(64, true);
    xprintf_P(PSTR("%d\r\n"), testCB.counts, adc_acc);
    opto_test("OFF");
}
//-----------------------------------------------------------------------------
