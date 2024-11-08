/*
 * drv_uart_spx.c
 *
 *  Created on: 11 jul. 2018
 *      Author: pablo
 * 
 * PLACA BASE: sensor_cloro.
 * 
 * El driver de las uart permite crear las uarts y 2 estructuras tipo 
 * ringbuffer (chars) para c/u.
 * Estos son las interfaces a la capa de FRTOS-IO.
 * Para transmitir se escribe en el ringBuffer de TX y para leer lo recibido
 * se lee del ringBuffer de RX.
 * La transmision / recepcion se hace por interrupcion. Estas ISR son 
 * provistas por el driver
 * Cada placa tiene diferente asignacion de puertos por lo tanto hay
 * que modificar el driver a c/placa.
 * 
 * 
 */

#include "drv_uart_avrDX.h"

//------------------------------------------------------------------------------
// USART3: TERM
//------------------------------------------------------------------------------
void drv_uart3_init(uint32_t baudrate )
{
    
    PORTB.DIR &= ~PIN1_bm;
    PORTB.DIR |= PIN0_bm;
    USART3.BAUD = (uint16_t)USART_SET_BAUD_RATE(baudrate);     
    USART3.CTRLC = USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc;
    
    // Habilito el TX y el RX
    USART3.CTRLB |= USART_TXEN_bm;
    USART3.CTRLB |= USART_RXEN_bm;
    
    // Habilito las interrupciones por RX
    USART3.CTRLA |= USART_RXCIE_bm;
    
    // Las transmisiones son por poleo no INT.
    
    // RingBuffers
    rBchar_CreateStatic ( &TXRB_uart3, &uart3_txBuffer[0], UART3_TXSIZE  );
    rBchar_CreateStatic ( &RXRB_uart3, &uart3_rxBuffer[0], UART3_RXSIZE  );
}
//------------------------------------------------------------------------------
ISR(USART3_RXC_vect)
{
    // Driver ISR: Cuando se genera la interrupcion por RXIE, lee el dato
    // y lo pone en la cola (ringBuffer.)
char cChar = ' ';

	cChar = USART3.RXDATAL;
 	rBchar_PokeFromISR( &RXRB_uart3, cChar );
}
//------------------------------------------------------------------------------

