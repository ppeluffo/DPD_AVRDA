/* 
 * File:   toyi_valves.h
 * Author: pablo
 *
 * Created on 19 de enero de 2024, 08:51 AM
 */

#ifndef TOYI_VALVES_H
#define	TOYI_VALVES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include <avr/pgmspace.h>
    
    
typedef enum { VALVE_OPEN=0, VALVE_CLOSE } t_valve_status;
    
typedef struct {
    t_valve_status vs1;
    t_valve_status vs2;
    t_valve_status vs3;
} vstatus_t;

vstatus_t vstatus;

#define VALVE1_CTRL_PORT       PORTD
#define VALVE1_CTRL_PIN_bm     PIN2_bm
#define VALVE1_CTRL_PIN_bp     PIN2_bp
   
#define CONFIG_VALVE1_CTRL()  (VALVE1_CTRL_PORT.DIR |= VALVE1_CTRL_PIN_bm );
#define OPEN_VALVE1()  ( VALVE1_CTRL_PORT.OUT |= VALVE1_CTRL_PIN_bm ); vstatus.vs1 = VALVE_OPEN;
#define CLOSE_VALVE1() ( VALVE1_CTRL_PORT.OUT &= ~VALVE1_CTRL_PIN_bm ); vstatus.vs1 = VALVE_CLOSE;

#define VALVE2_CTRL_PORT       PORTD
#define VALVE2_CTRL_PIN_bm     PIN3_bm
#define VALVE2_CTRL_PIN_bp     PIN3_bp

#define CONFIG_VALVE2_CTRL()  (VALVE2_CTRL_PORT.DIR |= VALVE2_CTRL_PIN_bm );
#define OPEN_VALVE2()  ( VALVE2_CTRL_PORT.OUT |= VALVE2_CTRL_PIN_bm ); vstatus.vs2 = VALVE_OPEN;
#define CLOSE_VALVE2() ( VALVE2_CTRL_PORT.OUT &= ~VALVE2_CTRL_PIN_bm ); vstatus.vs2 = VALVE_CLOSE;

#define VALVE3_CTRL_PORT       PORTD
#define VALVE3_CTRL_PIN_bm     PIN4_bm
#define VALVE3_CTRL_PIN_bp     PIN4_bp
    
#define CONFIG_VALVE3_CTRL()  (VALVE3_CTRL_PORT.DIR |= VALVE3_CTRL_PIN_bm );
#define OPEN_VALVE3()  ( VALVE3_CTRL_PORT.OUT |= VALVE3_CTRL_PIN_bm ); vstatus.vs3 = VALVE_OPEN;
#define CLOSE_VALVE3() ( VALVE3_CTRL_PORT.OUT &= ~VALVE3_CTRL_PIN_bm ); vstatus.vs3 = VALVE_CLOSE;

void TOYI_VALVES_init(void);
void get_valve_status(vstatus_t *lvstatus);
bool valve_tests( char *valve_id, char *action);

#ifdef	__cplusplus
}
#endif

#endif	/* TOYI_VALVES_H */

