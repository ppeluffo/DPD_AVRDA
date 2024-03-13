
#include "tmc2209.h"

TimerHandle_t tmc2209_xTimer;
StaticTimer_t tmc2209_xTimerBuffer;
void tmc2209_TimerCallback( TimerHandle_t xTimer );

#define TIMER_CALLBACK_MS  5
#define TIMER_FREQ_MS   ( TIMER_CALLBACK_MS * 2 )


cnt_cb_t CNT_CB;

//------------------------------------------------------------------------------
void TMC2209_init(void)
{
uint8_t i;
    
    CONFIG_TMC2209_0_EN();
    CONFIG_TMC2209_0_STEP();
    CONFIG_TMC2209_0_DIR();

    CONFIG_TMC2209_1_EN();
    CONFIG_TMC2209_1_STEP();
    CONFIG_TMC2209_1_DIR();

    CONFIG_TMC2209_2_EN();
    CONFIG_TMC2209_2_STEP();
    CONFIG_TMC2209_2_DIR();
    
    TMC2209_0_DISABLE();
    TMC2209_1_DISABLE();
    TMC2209_2_DISABLE();
    
    TMC2209_0_FORWARD();
    TMC2209_1_FORWARD();
    TMC2209_2_FORWARD();
    
    TMC2209_0_STEP_OFF();
    TMC2209_1_STEP_OFF();
    TMC2209_2_STEP_OFF();
    
    for (i=0; i<3; i++) {
        CNT_CB.cnt[i] = 0;
        CNT_CB.status[i] = STOPPED;        
    }
    
    tmc2209_xTimer = xTimerCreateStatic (
        "TMC2209",
        pdMS_TO_TICKS( TIMER_CALLBACK_MS ),
		pdTRUE,
		( void * ) 0,
		tmc2209_TimerCallback,
		&tmc2209_xTimerBuffer
	);
    
    xTimerStart( tmc2209_xTimer, 10 );
    
}
// -----------------------------------------------------------------------------
void tmc2209_TimerCallback( TimerHandle_t xTimer )
{
    
uint8_t i;
    
	// Genera un pulso.
    TMC2209_0_STEP_TOGGLE();
    TMC2209_1_STEP_TOGGLE();
    TMC2209_2_STEP_TOGGLE();
    
    for (i=0; i<3; i++) {
        // Si el contador esta corriendo...
        if ( CNT_CB.status[i] == RUNNING ) {
    
            if ( CNT_CB.cnt[i] > 0 ) {
                // Si no llego al final, sigo contando
                CNT_CB.cnt[i]--;
            } else {
                // LLego al final: lo detengo
                tmc2209_stop(i);
            }
        }
    }
    
}
//------------------------------------------------------------------------------
bool tmc2209_test( char *s_id, char *s_opt, char *s_action, char *s_secs )
{

 	// tmc {id} {en|dir} {on|off}
    switch(atoi(s_id)) {
        case 0:
            if (!strcmp_P( strupr(s_opt), PSTR("EN")) ) {
                if (!strcmp_P( strupr(s_action), PSTR("ON")) ) {
                    TMC2209_0_ENABLE();
                    return(true);
                }
                if (!strcmp_P( strupr(s_action), PSTR("OFF")) ) {
                    TMC2209_0_DISABLE();
                    return(true);
                }
                return(false);
            }
            
            if (!strcmp_P( strupr(s_opt), PSTR("DIR")) ) {
                if (!strcmp_P( strupr(s_action), PSTR("ON")) ) {
                    TMC2209_0_FORWARD();
                    return(true);
                }
                if (!strcmp_P( strupr(s_action), PSTR("OFF")) ) {
                    TMC2209_0_REVERSE();
                    return(true);
                }
                return(false);
            }
            
            if (!strcmp_P( strupr(s_opt), PSTR("RUN")) ) {
                tmc2209_run(0, s_action, atoi(s_secs));
                return(true);
            }

            if (!strcmp_P( strupr(s_opt), PSTR("STOP")) ) {
                tmc2209_stop(0);
                return(true);
            }     
            
            return(false);
            break;
            
        case 1:
             if (!strcmp_P( strupr(s_opt), PSTR("EN")) ) {
                if (!strcmp_P( strupr(s_action), PSTR("ON")) ) {
                    TMC2209_1_ENABLE();
                    return(true);
                }
                if (!strcmp_P( strupr(s_action), PSTR("OFF")) ) {
                    TMC2209_1_DISABLE();
                    return(true);
                }
                return(false);
            }
            
            if (!strcmp_P( strupr(s_opt), PSTR("DIR")) ) {
                if (!strcmp_P( strupr(s_action), PSTR("ON")) ) {
                    TMC2209_1_FORWARD();
                    return(true);
                }
                if (!strcmp_P( strupr(s_action), PSTR("OFF")) ) {
                    TMC2209_1_REVERSE();
                    return(true);
                }
                return(false);
            }
             
            if (!strcmp_P( strupr(s_opt), PSTR("RUN")) ) {
                tmc2209_run(1, s_action, atoi(s_secs));
                return(true);
            }

            if (!strcmp_P( strupr(s_opt), PSTR("STOP")) ) {
                tmc2209_stop(1);
                return(true);
            } 
            
            return(false);
            break;           
 
        case 2:
             if (!strcmp_P( strupr(s_opt), PSTR("EN")) ) {
                if (!strcmp_P( strupr(s_action), PSTR("ON")) ) {
                    TMC2209_2_ENABLE();
                    return(true);
                }
                if (!strcmp_P( strupr(s_action), PSTR("OFF")) ) {
                    TMC2209_2_DISABLE();
                    return(true);
                }
                return(false);
            }
            
            if (!strcmp_P( strupr(s_opt), PSTR("DIR")) ) {
                if (!strcmp_P( strupr(s_action), PSTR("ON")) ) {
                    TMC2209_2_FORWARD();
                    return(true);
                }
                if (!strcmp_P( strupr(s_action), PSTR("OFF")) ) {
                    TMC2209_2_REVERSE();
                    return(true);
                }
                return(false);
            }
            
            if (!strcmp_P( strupr(s_opt), PSTR("RUN")) ) {
                tmc2209_run(2, s_action, atoi(s_secs));
                return(true);
            }

            if (!strcmp_P( strupr(s_opt), PSTR("STOP")) ) {
                tmc2209_stop(2);
                return(true);
            } 
             
            return(false);
            break; 
    }

    return(false);
    
}
//------------------------------------------------------------------------------
void tmc2209_run(uint8_t id, char *s_action, uint16_t secs)
{
    
    switch(id) {
        case 0:
            if (!strcmp_P( strupr(s_action), PSTR("FW")) ) {
                TMC2209_0_FORWARD();
            } else {
                TMC2209_0_REVERSE();
            }
            TMC2209_0_ENABLE();
            CNT_CB.cnt[0] = secs * 1000 / TIMER_FREQ_MS;
            CNT_CB.status[0] = RUNNING;
            break;
        case 1:
            if (!strcmp_P( strupr(s_action), PSTR("FW")) ) {
                TMC2209_1_FORWARD();
            } else {
                TMC2209_1_REVERSE();
            }
            TMC2209_1_ENABLE();
            CNT_CB.cnt[1] = secs * 1000 / TIMER_FREQ_MS;
            CNT_CB.status[1] = RUNNING;
            break;
        case 2:
            if (!strcmp_P( strupr(s_action), PSTR("FW")) ) {
                TMC2209_2_FORWARD();
            } else {
                TMC2209_2_REVERSE();
            }
            TMC2209_2_ENABLE();
            CNT_CB.cnt[2] = secs * 1000 / TIMER_FREQ_MS;
            CNT_CB.status[2] = RUNNING;
            break;        
    }    
}
//------------------------------------------------------------------------------
void tmc2209_stop(uint8_t id)
{
    switch(id) {
        case 0:
            TMC2209_0_DISABLE();
            CNT_CB.cnt[0] = 0;
            CNT_CB.status[0] = STOPPED;
            break;
        case 1:
            TMC2209_1_DISABLE();
            CNT_CB.cnt[1] = 0;
            CNT_CB.status[1] = STOPPED;
            break;
        case 2:
            TMC2209_2_DISABLE();
            CNT_CB.cnt[2] = 0;
            CNT_CB.status[2] = STOPPED;
            break;        
    }
}
//------------------------------------------------------------------------------
void get_tmc2209_status(cnt_cb_t  *cnt_cb)
{
    memcpy(cnt_cb, &CNT_CB, sizeof(cnt_cb_t));
}
//------------------------------------------------------------------------------
