
#include "contadores.h"
#include <avr/interrupt.h>

//------------------------------------------------------------------------------
void counter_init_outofrtos( void )
{
    CNT0_CONFIG();
          
    // Configuro los pines para interrumpir
    // Habilito a interrumpir, pullup, flanco de bajada.
    cli();
    PORTC.PIN2CTRL |= PORT_PULLUPEN_bm | PORT_ISC_FALLING_gc;
    sei();
    
    contador = 0;
}
// ----------------------------------------------------------------------------- 
uint8_t counter_read_pin(void)
{
    return ( ( CNT0_PORT.IN & CNT0_PIN_bm ) >> CNT0_PIN) ;
}
// -----------------------------------------------------------------------------
uint32_t counter_read(void)
{
    return(contador);
}
// -----------------------------------------------------------------------------
void counter_clear(void)
{
    contador = 0;
}
//------------------------------------------------------------------------------
ISR(PORTC_PORT_vect)
{

    // Borro las flags.
    if (PC2_INTERRUPT ) {
        
        contador++;            
        // La interrupcion la vuelve a habilitar el timer.
        PC2_CLEAR_INTERRUPT_FLAG;
    }

}
//-----------------------------------------------------------------------------  
bool counter_test( char *action)
{
            
	// cnt {read|clear|pin}
    if (!strcmp_P( strupr(action), PSTR("READ")) ) {
        xprintf_P(PSTR("cnt=%lu\r\n"), counter_read());
        return(true);
    }
    
    if (!strcmp_P( strupr(action), PSTR("CLEAR")) ) {
        counter_clear();
        return(true);
    }
    
    if (!strcmp_P( strupr(action), PSTR("PIN")) ) {
        xprintf_P(PSTR("cnt_pin=%d\r\n"), counter_read_pin());
        return(true);
    }

    return(false);
}
//------------------------------------------------------------------------------
