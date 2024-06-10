/* 
 * File:   contadores.h
 * Author: pablo
 *
 * Created on July 19, 2023, 5:04 PM
 */

#ifndef CONTADORES_H
#define	CONTADORES_H

#ifdef	__cplusplus
extern "C" {
#endif


#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
    
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "math.h"
#include <avr/pgmspace.h>
    
#include "xprintf.h"
 
#include "pines.h"

#define CNT0_PORT	PORTC
#define CNT0_PIN    2   
#define CNT0_PIN_bm	PIN2_bm
#define CNT0_PIN_bp	PIN2_bp

// Los CNTx son inputs
#define CNT0_CONFIG()               ( CNT0_PORT.DIR &= ~CNT0_PIN_bm )

#define PC2_INTERRUPT               ( PORTC.INTFLAGS & PIN2_bm )
#define PC2_CLEAR_INTERRUPT_FLAG    ( PORTC.INTFLAGS &= PIN2_bm )
    
uint32_t contador;

void counter_init_outofrtos(void);
uint8_t counter_read_pin(void);
uint32_t counter_read();
void counter_clear(void);
bool counter_test( char *action);

#ifdef	__cplusplus
}
#endif

#endif	/* CONTADORES_H */

