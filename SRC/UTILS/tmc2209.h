/* 
 * File:   tmc2209.h
 * Author: pablo
 *
 * Created on 22 de enero de 2024, 12:10 PM
 */

#ifndef TMC2209_H
#define	TMC2209_H

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
    
typedef enum { DIR_FW=0, DIR_REV } t_tmc2209_dir;
typedef enum { RUNNING=0, STOPPED } t_tmc2209_status;

#define MAX_PUMPS 3

typedef struct {
    uint16_t freq;
    uint32_t ticks;
    bool enabled;
    bool running;
} pump_CB_t;

pump_CB_t pump0, pump1, pump2;

#define PUMP0_EN_PORT           PORTF
#define PUMP0_EN_PIN_bm         PIN4_bm
#define PUMP0_EN_PIN_bp         PIN4_bp

#define PUMP0_STEP_PORT         PORTF
#define PUMP0_STEP_PIN_bm       PIN3_bm
#define PUMP0_STEP_PIN_bp       PIN3_bp

#define PUMP0_DIR_PORT          PORTF
#define PUMP0_DIR_PIN_bm        PIN2_bm
#define PUMP0_DIR_PIN_bp        PIN2_bp

#define PUMP0_EN_CONFIG()       (PUMP0_EN_PORT.DIR |= PUMP0_EN_PIN_bm)
#define PUMP0_STEP_CONFIG()     (PUMP0_STEP_PORT.DIR |= PUMP0_STEP_PIN_bm)
#define PUMP0_DIR_CONFIG()      (PUMP0_DIR_PORT.DIR |= PUMP0_DIR_PIN_bm)

#define PUMP0_DISABLE()     ( PUMP0_EN_PORT.OUT |= PUMP0_EN_PIN_bm )
#define PUMP0_ENABLE()      ( PUMP0_EN_PORT.OUT &= ~PUMP0_EN_PIN_bm )
#define PUMP0_FORWARD()     ( PUMP0_DIR_PORT.OUT |= PUMP0_DIR_PIN_bm )
#define PUMP0_REVERSE()     ( PUMP0_DIR_PORT.OUT &= ~PUMP0_DIR_PIN_bm )
#define PUMP0_STEP_ON()     ( PUMP0_STEP_PORT.OUT |= PUMP0_STEP_PIN_bm )
#define PUMP0_STEP_OFF()    ( PUMP0_STEP_PORT.OUT &= ~PUMP0_STEP_PIN_bm )
#define PUMP0_STEP_TOGGLE() ( PUMP0_STEP_PORT.OUT ^= 1UL << PUMP0_STEP_PIN_bp);

#define PUMP1_EN_PORT           PORTF
#define PUMP1_EN_PIN_bm         PIN1_bm
#define PUMP1_EN_PIN_bp         PIN1_bp

#define PUMP1_STEP_PORT         PORTF
#define PUMP1_STEP_PIN_bm       PIN0_bm
#define PUMP1_STEP_PIN_bp       PIN0_bp

#define PUMP1_DIR_PORT          PORTE
#define PUMP1_DIR_PIN_bm        PIN7_bm
#define PUMP1_DIR_PIN_bp        PIN7_bp

#define PUMP1_EN_CONFIG()       (PUMP1_EN_PORT.DIR |= PUMP1_EN_PIN_bm);
#define PUMP1_STEP_CONFIG()     (PUMP1_STEP_PORT.DIR |= PUMP1_STEP_PIN_bm);
#define PUMP1_DIR_CONFIG()      (PUMP1_DIR_PORT.DIR |= PUMP1_DIR_PIN_bm);

#define PUMP1_DISABLE()     ( PUMP1_EN_PORT.OUT |= PUMP1_EN_PIN_bm )
#define PUMP1_ENABLE()      ( PUMP1_EN_PORT.OUT &= ~PUMP1_EN_PIN_bm )
#define PUMP1_FORWARD()     ( PUMP1_DIR_PORT.OUT |= PUMP1_DIR_PIN_bm )
#define PUMP1_REVERSE()     ( PUMP1_DIR_PORT.OUT &= ~PUMP1_DIR_PIN_bm )
#define PUMP1_STEP_ON()     ( PUMP1_STEP_PORT.OUT |= PUMP1_STEP_PIN_bm )
#define PUMP1_STEP_OFF()    ( PUMP1_STEP_PORT.OUT &= ~PUMP1_STEP_PIN_bm )
#define PUMP1_STEP_TOGGLE() ( PUMP1_STEP_PORT.OUT ^= 1UL << PUMP1_STEP_PIN_bp)

#define PUMP2_EN_PORT           PORTD
#define PUMP2_EN_PIN_bm         PIN7_bm
#define PUMP2_EN_PIN_bp         PIN7_bp

#define PUMP2_STEP_PORT         PORTD
#define PUMP2_STEP_PIN_bm       PIN6_bm
#define PUMP2_STEP_PIN_bp       PIN6_bp

#define PUMP2_DIR_PORT          PORTD
#define PUMP2_DIR_PIN_bm        PIN5_bm
#define PUMP2_DIR_PIN_bp        PIN5_bp

#define PUMP2_EN_CONFIG()       (PUMP2_EN_PORT.DIR |= PUMP2_EN_PIN_bm);
#define PUMP2_STEP_CONFIG()     (PUMP2_STEP_PORT.DIR |= PUMP2_STEP_PIN_bm);
#define PUMP2_DIR_CONFIG()      (PUMP2_DIR_PORT.DIR |= PUMP2_DIR_PIN_bm);

#define PUMP2_DISABLE()     ( PUMP2_EN_PORT.OUT |= PUMP2_EN_PIN_bm )
#define PUMP2_ENABLE()      ( PUMP2_EN_PORT.OUT &= ~PUMP2_EN_PIN_bm )
#define PUMP2_FORWARD()     ( PUMP2_DIR_PORT.OUT |= PUMP2_DIR_PIN_bm )
#define PUMP2_REVERSE()     ( PUMP2_DIR_PORT.OUT &= ~PUMP2_DIR_PIN_bm )
#define PUMP2_STEP_ON()     ( PUMP2_STEP_PORT.OUT |= PUMP2_STEP_PIN_bm )
#define PUMP2_STEP_OFF()    ( PUMP2_STEP_PORT.OUT &= ~PUMP2_STEP_PIN_bm )
#define PUMP2_STEP_TOGGLE() ( PUMP2_STEP_PORT.OUT ^= 1UL << PUMP2_STEP_PIN_bp)


void pump0_init(void);
void pump0_stop(void);
void pump0_run(uint16_t secs);

void pump1_init(void);
void pump1_stop(void);
void pump1_run(uint16_t secs);

void pump2_init(void);
void pump2_stop(void);
void pump2_run(uint16_t secs);

bool pump( char *s_id, char *s_cmd, char *s_param );
bool pump_tests( char *s_id, char *s_cmd, char *s_param );
void pump_print_status(void);
bool pump_config( char *s_id, char *s_freq );

void pump_config_default(void);
void pump_update_config(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TMC2209_H */

