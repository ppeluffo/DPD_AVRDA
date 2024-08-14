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
#include "xprintf.h"
#include <avr/pgmspace.h>
    
    
typedef enum { VALVE_OPEN=0, VALVE_CLOSE } t_valve_status;
    
typedef struct {
    t_valve_status vs0;
    t_valve_status vs1;
    t_valve_status vs2;
} vstatus_t;

vstatus_t vstatus;

#define VALVE0_CTRL_PORT       PORTD
#define VALVE0_CTRL_PIN_bm     PIN2_bm
#define VALVE0_CTRL_PIN_bp     PIN2_bp
   
#define CONFIG_VALVE0_CTRL()  (VALVE0_CTRL_PORT.DIR |= VALVE0_CTRL_PIN_bm )
#define OPEN_VALVE0()  ( VALVE0_CTRL_PORT.OUT |= VALVE0_CTRL_PIN_bm )
#define CLOSE_VALVE0() ( VALVE0_CTRL_PORT.OUT &= ~VALVE0_CTRL_PIN_bm )

#define VALVE1_CTRL_PORT       PORTD
#define VALVE1_CTRL_PIN_bm     PIN3_bm
#define VALVE1_CTRL_PIN_bp     PIN3_bp

#define CONFIG_VALVE1_CTRL()  (VALVE1_CTRL_PORT.DIR |= VALVE1_CTRL_PIN_bm )
#define OPEN_VALVE1()  ( VALVE1_CTRL_PORT.OUT |= VALVE1_CTRL_PIN_bm )
#define CLOSE_VALVE1() ( VALVE1_CTRL_PORT.OUT &= ~VALVE1_CTRL_PIN_bm )

#define VALVE2_CTRL_PORT       PORTD
#define VALVE2_CTRL_PIN_bm     PIN4_bm
#define VALVE2_CTRL_PIN_bp     PIN4_bp
    
#define CONFIG_VALVE2_CTRL()  (VALVE2_CTRL_PORT.DIR |= VALVE2_CTRL_PIN_bm )
#define OPEN_VALVE2()  ( VALVE2_CTRL_PORT.OUT |= VALVE2_CTRL_PIN_bm )
#define CLOSE_VALVE2() ( VALVE2_CTRL_PORT.OUT &= ~VALVE2_CTRL_PIN_bm )

void TOYI_VALVES_init(void);
void valve_get_status(vstatus_t *lvstatus);
bool valve_tests( char *valve_id, char *action);
bool valve_open(uint8_t valve_id);
bool valve_close(uint8_t valve_id);
void valve_print_status(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TOYI_VALVES_H */

