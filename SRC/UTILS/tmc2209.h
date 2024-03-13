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
    
typedef enum { DIR_FW=0, DIR_REV } t_tmc2209_dir;
typedef enum { RUNNING=0, STOPPED } t_tmc2209_status;

typedef struct {
    t_tmc2209_status status[3];
    uint32_t cnt[3];
} cnt_cb_t;

#define TMC2209_0_EN_PORT           PORTF
#define TMC2209_0_EN_PIN_bm         PIN4_bm
#define TMC2209_0_EN_PIN_bp         PIN4_bp

#define TMC2209_0_STEP_PORT         PORTF
#define TMC2209_0_STEP_PIN_bm       PIN3_bm
#define TMC2209_0_STEP_PIN_bp       PIN3_bp

#define TMC2209_0_DIR_PORT          PORTF
#define TMC2209_0_DIR_PIN_bm        PIN2_bm
#define TMC2209_0_DIR_PIN_bp        PIN2_bp

#define TMC2209_1_EN_PORT           PORTF
#define TMC2209_1_EN_PIN_bm         PIN1_bm
#define TMC2209_1_EN_PIN_bp         PIN1_bp

#define TMC2209_1_STEP_PORT         PORTF
#define TMC2209_1_STEP_PIN_bm       PIN0_bm
#define TMC2209_1_STEP_PIN_bp       PIN0_bp

#define TMC2209_1_DIR_PORT          PORTE
#define TMC2209_1_DIR_PIN_bm        PIN7_bm
#define TMC2209_1_DIR_PIN_bp        PIN7_bp

#define TMC2209_2_EN_PORT           PORTD
#define TMC2209_2_EN_PIN_bm         PIN7_bm
#define TMC2209_2_EN_PIN_bp         PIN7_bp

#define TMC2209_2_STEP_PORT         PORTD
#define TMC2209_2_STEP_PIN_bm       PIN6_bm
#define TMC2209_2_STEP_PIN_bp       PIN6_bp

#define TMC2209_2_DIR_PORT          PORTD
#define TMC2209_2_DIR_PIN_bm        PIN5_bm
#define TMC2209_2_DIR_PIN_bp        PIN5_bp

#define CONFIG_TMC2209_0_EN()       (TMC2209_0_EN_PORT.DIR |= TMC2209_0_EN_PIN_bm);
#define CONFIG_TMC2209_0_STEP()     (TMC2209_0_STEP_PORT.DIR |= TMC2209_0_STEP_PIN_bm);
#define CONFIG_TMC2209_0_DIR()      (TMC2209_0_DIR_PORT.DIR |= TMC2209_0_DIR_PIN_bm);

#define TMC2209_0_DISABLE()     ( TMC2209_0_EN_PORT.OUT |= TMC2209_0_EN_PIN_bm )
#define TMC2209_0_ENABLE()      ( TMC2209_0_EN_PORT.OUT &= ~TMC2209_0_EN_PIN_bm )
#define TMC2209_0_FORWARD()     ( TMC2209_0_DIR_PORT.OUT |= TMC2209_0_DIR_PIN_bm )
#define TMC2209_0_REVERSE()     ( TMC2209_0_DIR_PORT.OUT &= ~TMC2209_0_DIR_PIN_bm )
#define TMC2209_0_STEP_ON()     ( TMC2209_0_STEP_PORT.OUT |= TMC2209_0_STEP_PIN_bm )
#define TMC2209_0_STEP_OFF()    ( TMC2209_0_STEP_PORT.OUT &= ~TMC2209_0_STEP_PIN_bm )
#define TMC2209_0_STEP_TOGGLE() ( TMC2209_0_STEP_PORT.OUT ^= 1UL << TMC2209_0_STEP_PIN_bp);

#define CONFIG_TMC2209_1_EN()       (TMC2209_1_EN_PORT.DIR |= TMC2209_1_EN_PIN_bm);
#define CONFIG_TMC2209_1_STEP()     (TMC2209_1_STEP_PORT.DIR |= TMC2209_1_STEP_PIN_bm);
#define CONFIG_TMC2209_1_DIR()      (TMC2209_1_DIR_PORT.DIR |= TMC2209_1_DIR_PIN_bm);

#define TMC2209_1_DISABLE()     ( TMC2209_1_EN_PORT.OUT |= TMC2209_1_EN_PIN_bm )
#define TMC2209_1_ENABLE()      ( TMC2209_1_EN_PORT.OUT &= ~TMC2209_1_EN_PIN_bm )
#define TMC2209_1_FORWARD()     ( TMC2209_1_DIR_PORT.OUT |= TMC2209_1_DIR_PIN_bm )
#define TMC2209_1_REVERSE()     ( TMC2209_1_DIR_PORT.OUT &= ~TMC2209_1_DIR_PIN_bm )
#define TMC2209_1_STEP_ON()     ( TMC2209_1_STEP_PORT.OUT |= TMC2209_1_STEP_PIN_bm )
#define TMC2209_1_STEP_OFF()    ( TMC2209_1_STEP_PORT.OUT &= ~TMC2209_1_STEP_PIN_bm )
#define TMC2209_1_STEP_TOGGLE() ( TMC2209_1_STEP_PORT.OUT ^= 1UL << TMC2209_1_STEP_PIN_bp);

#define CONFIG_TMC2209_2_EN()       (TMC2209_2_EN_PORT.DIR |= TMC2209_2_EN_PIN_bm);
#define CONFIG_TMC2209_2_STEP()     (TMC2209_2_STEP_PORT.DIR |= TMC2209_2_STEP_PIN_bm);
#define CONFIG_TMC2209_2_DIR()      (TMC2209_2_DIR_PORT.DIR |= TMC2209_2_DIR_PIN_bm);

#define TMC2209_2_DISABLE()     ( TMC2209_2_EN_PORT.OUT |= TMC2209_2_EN_PIN_bm )
#define TMC2209_2_ENABLE()      ( TMC2209_2_EN_PORT.OUT &= ~TMC2209_2_EN_PIN_bm )
#define TMC2209_2_FORWARD()     ( TMC2209_2_DIR_PORT.OUT |= TMC2209_2_DIR_PIN_bm )
#define TMC2209_2_REVERSE()     ( TMC2209_2_DIR_PORT.OUT &= ~TMC2209_2_DIR_PIN_bm )
#define TMC2209_2_STEP_ON()     ( TMC2209_2_STEP_PORT.OUT |= TMC2209_2_STEP_PIN_bm )
#define TMC2209_2_STEP_OFF()    ( TMC2209_2_STEP_PORT.OUT &= ~TMC2209_2_STEP_PIN_bm )
#define TMC2209_2_STEP_TOGGLE() ( TMC2209_2_STEP_PORT.OUT ^= 1UL << TMC2209_2_STEP_PIN_bp);

void TMC2209_init(void);
bool tmc2209_test( char *s_id, char *s_opt, char *s_action, char *s_secs );
void tmc2209_stop(uint8_t id);
void tmc2209_run(uint8_t id, char *s_action, uint16_t secs);
void get_tmc2209_status(cnt_cb_t  *cnt_cb);

#ifdef	__cplusplus
}
#endif

#endif	/* TMC2209_H */

