
#include "tmc2209.h"

TimerHandle_t tmc2209_xTimer;
StaticTimer_t tmc2209_xTimerBuffer;
void tmc2209_TimerCallback( TimerHandle_t xTimer );

#define TIMER_CALLBACK_MS  1
#define TIMER_FREQ_MS   ( TIMER_CALLBACK_MS * 2 )

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
        STEPPERS_CB.cnt[i] = 0;
        STEPPERS_CB.status[i] = STOPPED; 
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
    
    for (i=0; i<3; i++) {
                
        // Si el contador esta corriendo...
        if ( STEPPERS_CB.status[i] == RUNNING ) {
    
            if ( i == 0 )
                TMC2209_0_STEP_TOGGLE();

            if ( i == 1 )
                TMC2209_1_STEP_TOGGLE();
        
            if ( i == 2 )
                TMC2209_2_STEP_TOGGLE();

            if ( STEPPERS_CB.cnt[i] > 0 ) {
                // Si no llego al final, sigo contando
                STEPPERS_CB.cnt[i]--;
            } else {
                // LLego al final: lo detengo
                tmc2209_stop(i);
            }
        }
    }
    
}
//------------------------------------------------------------------------------
bool tmc2209_test( char *s_id, char *s_var1, char *s_var2, char *s_var3 )
{

    // test stepper {0,1,2} run secs
    switch(atoi(s_id)) {
        case 0:
            if (!strcmp_P( strupr(s_var1), PSTR("RUN")) ) {
                tmc2209_run(0, atoi(s_var2) );
                return(true);
            }

            if (!strcmp_P( strupr(s_var1), PSTR("STOP")) ) {
                tmc2209_stop(0);
                return(true);
            }     
            
            return(false);
            break;
            
        case 1:
            if (!strcmp_P( strupr(s_var1), PSTR("RUN")) ) {
                tmc2209_run(1, atoi(s_var2) );
                return(true);
            }

            if (!strcmp_P( strupr(s_var1), PSTR("STOP")) ) {
                tmc2209_stop(1);
                return(true);
            }     
            
            return(false);
            break;        
 
        case 2:
            if (!strcmp_P( strupr(s_var1), PSTR("RUN")) ) {
                tmc2209_run(2, atoi(s_var2) );
                return(true);
            }

            if (!strcmp_P( strupr(s_var1), PSTR("STOP")) ) {
                tmc2209_stop(2);
                return(true);
            }     
            
            return(false);
            break;  
    }

    return(false);
    
}
//------------------------------------------------------------------------------
bool tmc2209_test1( char *s_id, char *s_opt, char *s_action, char *s_secs )
{

uint16_t seconds = atoi(s_action);
    
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
                //tmc2209_run(0, s_action, atoi(s_secs));
                tmc2209_run1(0, NULL, seconds );
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
                //tmc2209_run(1, s_action, atoi(s_secs));
                tmc2209_run1(1, NULL, seconds );
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
                //tmc2209_run(2, s_action, atoi(s_secs));
                tmc2209_run1(2, NULL, seconds );
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
void tmc2209_run1(uint8_t id, char *s_action, uint16_t secs)
{
    /*
     * En el DPD siempre se mueven FORWARD !!!
     * No importa s_action
     */
    
    xprintf_P(PSTR("DEBUG: seconds = %d\r\n"), secs);

        
    switch(id) {
        case 0:
            /*
            if (!strcmp_P( strupr(s_action), PSTR("FW")) ) {
                TMC2209_0_FORWARD();
            } else {
                TMC2209_0_REVERSE();
            }
             */
            TMC2209_0_FORWARD();
            TMC2209_0_ENABLE();
            STEPPERS_CB.cnt[0] = 2 * secs * 1000 / TIMER_FREQ_MS;
            xprintf_P(PSTR("DEBUG: CB0 = %d\r\n"), STEPPERS_CB.cnt[0] );
            STEPPERS_CB.status[0] = RUNNING;
            break;
        case 1:
            /*
            if (!strcmp_P( strupr(s_action), PSTR("FW")) ) {
                TMC2209_1_FORWARD();
            } else {
                TMC2209_1_REVERSE();
            }
             */
            TMC2209_1_FORWARD();
            TMC2209_1_ENABLE();
            STEPPERS_CB.cnt[1] = 2 * secs * 1000 / TIMER_FREQ_MS;
            //xprintf_P(PSTR("DEBUG: CB1 = %d\r\n"), STEPPERS_CB.cnt[1] );
            STEPPERS_CB.status[1] = RUNNING;
            break;
        case 2:
            /*
            if (!strcmp_P( strupr(s_action), PSTR("FW")) ) {
                TMC2209_2_FORWARD();
            } else {
                TMC2209_2_REVERSE();
            }
             */
            TMC2209_2_FORWARD();
            TMC2209_2_ENABLE();
            STEPPERS_CB.cnt[2] = 2 * secs * 1000 / TIMER_FREQ_MS;
            //xprintf_P(PSTR("DEBUG: CB2 = %d\r\n"), STEPPERS_CB.cnt[2] );
            STEPPERS_CB.status[2] = RUNNING;
            break;        
    }    
}
//------------------------------------------------------------------------------
void tmc2209_run(uint8_t id, uint16_t secs)
{
    /*
     * En el DPD siempre se mueven FORWARD !!!
     * No importa s_action
     */
    
    //xprintf_P(PSTR("DEBUG: ID=%d, seconds = %d\r\n"), id, secs);

float counter = 0.0;
    
    counter = 2.0 * secs * 1000 / TIMER_FREQ_MS;
    
    switch(id) {
        case 0:
            TMC2209_0_FORWARD();
            TMC2209_0_ENABLE();
            STEPPERS_CB.cnt[0] = (uint32_t)counter;
            //xprintf_P(PSTR("DEBUG: CB0 = %lu\r\n"), STEPPERS_CB.cnt[0] );
            STEPPERS_CB.status[0] = RUNNING;
            break;
        case 1:
            TMC2209_1_FORWARD();
            TMC2209_1_ENABLE();
            STEPPERS_CB.cnt[1] = (uint32_t)counter;
            //xprintf_P(PSTR("DEBUG: CB1 = %d\r\n"), STEPPERS_CB.cnt[1] );
            STEPPERS_CB.status[1] = RUNNING;
            break;
        case 2:
            TMC2209_2_FORWARD();
            TMC2209_2_ENABLE();
            STEPPERS_CB.cnt[2] = (uint32_t)counter;
            //xprintf_P(PSTR("DEBUG: CB2 = %d\r\n"), STEPPERS_CB.cnt[2] );
            STEPPERS_CB.status[2] = RUNNING;
            break;        
    }    
}
//------------------------------------------------------------------------------

void tmc2209_stop(uint8_t id)
{
    switch(id) {
        case 0:
            TMC2209_0_DISABLE();
            STEPPERS_CB.cnt[0] = 0;
            STEPPERS_CB.status[0] = STOPPED;
            break;
        case 1:
            TMC2209_1_DISABLE();
            STEPPERS_CB.cnt[1] = 0;
            STEPPERS_CB.status[1] = STOPPED;
            break;
        case 2:
            TMC2209_2_DISABLE();
            STEPPERS_CB.cnt[2] = 0;
            STEPPERS_CB.status[2] = STOPPED;
            break;        
    }
}
//------------------------------------------------------------------------------
void get_tmc2209_status(steppers_cb_t *steppers_cb)
{
    memcpy(steppers_cb, &STEPPERS_CB, sizeof(steppers_cb_t));
}
//------------------------------------------------------------------------------
