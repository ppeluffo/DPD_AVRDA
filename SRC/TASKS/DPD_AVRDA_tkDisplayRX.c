
#include "DPD_AVRDA.h"

//------------------------------------------------------------------------------
void tkLcdRX(void * pvParameters)
{

	/*
     * Esta tarea se encarga de recibir datos del display y
     * guardarlos en un buffer lineal.
     * Si se llena, BORRA EL BUFFER Y SE PIERDE TODO
     * No debería pasar porque antes lo debería haber procesado y flusheado
     * pero por las dudas. 
     */

( void ) pvParameters;
uint8_t c = 0;

    while ( ! starting_flag )
        vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );

    SYSTEM_ENTER_CRITICAL();
    task_running |= LRX_WDG_gc;
    SYSTEM_EXIT_CRITICAL();
        
    lBchar_CreateStatic ( &lcd_rx_lbuffer, (char *)&lcd_rx_buffer[0], LCD_RX_BUFFER_SIZE );
     
    xprintf_P(PSTR("Starting tkLcdRX..\r\n" ));
  
	// loop
	for( ;; )
	{
        u_kick_wdt(LRX_WDG_gc);
       
        while(true) {
            c = '\0';	// Lo borro para que luego del un CR no resetee siempre el timer.
            // el read se bloquea 50ms. lo que genera la espera.
            while ( xfgetc( fdLCD, (char *)&c ) == 1 ) {
                // Si hay datos recibidos, los encolo
                FSM_processLCDrx(c);
            }
        }
        vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
	}    
}
//------------------------------------------------------------------------------

