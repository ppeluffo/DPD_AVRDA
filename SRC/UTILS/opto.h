/* 
 * File:   opto.h
 * Author: pablo
 *
 * Created on 13 de marzo de 2024, 09:36 AM
 */

#ifndef OPTO_H
#define	OPTO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "stdint.h"
#include "stdbool.h"
#include <avr/pgmspace.h>
#include "stdlib.h"   
#include "string.h"
#include "xprintf.h"

bool opto_status;

#define OPTO_ENABLE_PORT       PORTG
#define OPTO_ENABLE_PIN_bm     PIN6_bm
#define OPTO_ENABLE_PIN_bp     PIN6_bp
   
#define CONFIG_OPTO()    (OPTO_ENABLE_PORT.DIR |= OPTO_ENABLE_PIN_bm )
#define PRENDER_OPTO()   (OPTO_ENABLE_PORT.OUT |= OPTO_ENABLE_PIN_bm )
#define APAGAR_OPTO()    (OPTO_ENABLE_PORT.OUT &= ~OPTO_ENABLE_PIN_bm )

typedef struct {
    bool debug;
} optoCB_t;

optoCB_t optoCB;

void opto_on( bool debug);
void opto_off( bool debug);
bool opto_get_status(void);
void cbk_opto_on(void);
void cbk_opto_off(void);





#ifdef	__cplusplus
}
#endif

#endif	/* OPTO_H */

