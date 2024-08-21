
#include "tmc2209.h"
#include "porthardware.h"
#include "avr/interrupt.h"
#include "DPD_AVRDA.h"


//---------------------------------------------------------------
void pump0_init(void)
{
    PUMP0_EN_CONFIG();
    PUMP0_STEP_CONFIG();
    PUMP0_DIR_CONFIG();
    
    PUMP0_DISABLE();
    PUMP0_FORWARD();
    PUMP0_STEP_OFF();
       
    // Por default arranca en 1Khz
    TCB1.CCMP = configCPU_CLOCK_HZ / pump0.freq;
    TCB1.INTCTRL = TCB_CAPT_bm; 
    TCB1.CTRLA = TCB_ENABLE_bm;
    
    pump0.enabled = true;
    pump0.running = false;
                
}
// ---------------------------------------------------------------
void pump0_run(uint16_t secs)
{
    xprintf_P(PSTR("PUMP0 run %d\r\n"), secs);
    PUMP0_ENABLE();
    PUMP0_FORWARD();
    
    TCB1.INTFLAGS = TCB_CAPT_bm;                         
    TCB1.CTRLA = TCB_ENABLE_bm; 
    
    //pump0.ticks = secs * pump0.freq;
    pump0.ticks = (uint32_t)secs * pump0.freq;
    //xprintf_P(PSTR("PUMP0 ticks %lu\r\n"), pump0.ticks);
    pump0.running = true;
    pump0.enabled = true;
}
//----------------------------------------------------------------
void pump0_stop(void)
{
    xprintf_P(PSTR("PUMP0 stop\r\n"));
    TCB1.CTRLA = 0x00;
    PUMP0_STEP_OFF();
    PUMP0_DISABLE();
    
    pump0.running = false;
    pump0.enabled = false;
}
// ---------------------------------------------------------------
ISR( TCB1_INT_vect )
{
    CLR_INT( TCB1_INTFLAGS, TCB_CAPT_bm );
    PUMP0_STEP_TOGGLE();
    if ( pump0.ticks-- == 0 ) {
       // Stop
        TCB1.CTRLA = 0x00;
        PUMP0_STEP_OFF();
        PUMP0_DISABLE();
        pump0.running = false;
        pump0.enabled = false;
    }
}
//---------------------------------------------------------------
void pump1_init(void)
{
    PUMP1_EN_CONFIG();
    PUMP1_STEP_CONFIG();
    PUMP1_DIR_CONFIG();
    
    PUMP1_DISABLE();
    PUMP1_FORWARD();
    PUMP1_STEP_OFF();
       
    // Por default arranca en 1Khz
    TCB2.CCMP = configCPU_CLOCK_HZ /pump1.freq;
    TCB2.INTCTRL = TCB_CAPT_bm; 
    TCB2.CTRLA = TCB_ENABLE_bm;
    
    pump1.enabled = true;
    pump1.running = false;               
}
// ---------------------------------------------------------------
void pump1_run(uint16_t secs)
{
    xprintf_P(PSTR("PUMP1 run %d\r\n"), secs);
    PUMP1_ENABLE();
    PUMP1_FORWARD();
    
    TCB2.INTFLAGS = TCB_CAPT_bm;                         
    TCB2.CTRLA = TCB_ENABLE_bm;  
    //pump1.ticks = secs * pump1.freq;
    pump1.ticks = (uint32_t)secs * pump1.freq;
    //xprintf_P(PSTR("PUMP1 ticks %lu\r\n"), pump1.ticks);
    pump1.running = true;
    pump1.enabled = true;
}
//----------------------------------------------------------------
void pump1_stop(void)
{
    xprintf_P(PSTR("PUMP1 stop\r\n"));
    TCB2.CTRLA = 0x00;
    PUMP1_STEP_OFF();
    PUMP1_DISABLE();
    pump1.running = false;
    pump1.enabled = false;
}
// ---------------------------------------------------------------
ISR( TCB2_INT_vect )
{
    CLR_INT( TCB2_INTFLAGS, TCB_CAPT_bm );
    PUMP1_STEP_TOGGLE();
    if ( pump1.ticks-- == 0 ) {
       // Stop
        TCB2.CTRLA = 0x00;
        PUMP1_STEP_OFF();
        PUMP1_DISABLE();
        pump1.running = false;
        pump1.enabled = false;
    }
}
//---------------------------------------------------------------
void pump2_init(void)
{
    PUMP2_EN_CONFIG();
    PUMP2_STEP_CONFIG();
    PUMP2_DIR_CONFIG();
    
    PUMP2_DISABLE();
    PUMP2_FORWARD();
    PUMP2_STEP_OFF();
       
    // Por default arranca en 1Khz
    TCB3.CCMP = configCPU_CLOCK_HZ / pump2.freq;
    TCB3.INTCTRL = TCB_CAPT_bm; 
    TCB3.CTRLA = TCB_ENABLE_bm;
     
    pump2.enabled = true;
    pump2.running = false;
}
// ---------------------------------------------------------------
void pump2_run(uint16_t secs)
{
    xprintf_P(PSTR("PUMP2 run %d\r\n"), secs);
    PUMP2_ENABLE();
    PUMP2_FORWARD();
    
    TCB3.INTFLAGS = TCB_CAPT_bm;                         
    TCB3.CTRLA = TCB_ENABLE_bm;  
    //pump2.ticks = secs * pump2.freq;
    pump2.ticks = (uint32_t)secs * pump2.freq;
    //xprintf_P(PSTR("PUMP2 ticks %lu\r\n"), pump2.ticks);
    pump2.running = true;
    pump2.enabled = true;   
}
//----------------------------------------------------------------
void pump2_stop(void)
{
    xprintf_P(PSTR("PUMP2 stop\r\n"));
    TCB3.CTRLA = 0x00;
    PUMP2_STEP_OFF();
    PUMP2_DISABLE();
    pump2.running = false;
    pump2.enabled = false;
}
// ---------------------------------------------------------------
ISR( TCB3_INT_vect )
{
    CLR_INT( TCB3_INTFLAGS, TCB_CAPT_bm );
    PUMP2_STEP_TOGGLE();
    if ( pump2.ticks-- == 0 ) {
       // Stop
        TCB3.CTRLA = 0x00;
        PUMP2_STEP_OFF();
        PUMP2_DISABLE();
        pump2.running = false;
        pump2.enabled = false;
    }
}
//---------------------------------------------------------------
bool pump( char *s_id, char *s_cmd, char *s_param )
{

    // pump {0,1,2} run secs
    // pump {0,1,2} stop
    
    switch(atoi(s_id)) {
        case 0:
            if (!strcmp_P( strupr(s_cmd), PSTR("RUN")) ) {
                pump0_run(atoi(s_param) );
                return(true);
            }

            if (!strcmp_P( strupr(s_cmd), PSTR("STOP")) ) {
                pump0_stop();
                return(true);
            }     
            
            return(false);
            break;
            
        case 1:
            if (!strcmp_P( strupr(s_cmd), PSTR("RUN")) ) {
                pump1_run(atoi(s_param) );
                return(true);
            }

            if (!strcmp_P( strupr(s_cmd), PSTR("STOP")) ) {
                pump1_stop();
                return(true);
            }    
         
            return(false);
            break;
            
        case 2:
            if (!strcmp_P( strupr(s_cmd), PSTR("RUN")) ) {
                pump2_run(atoi(s_param) );
                return(true);
            }

            if (!strcmp_P( strupr(s_cmd), PSTR("STOP")) ) {
                pump2_stop();
                return(true);
            }    

            return(false);
            break;
     }

    return(false);
    
}
//----------------------------------------------------------------
bool pump_tests( char *s_id, char *s_cmd, char *s_param )
{

    // test stepper {0,1,2} {en|step|dir} {on|off}
    switch(atoi(s_id)) {
        case 0:
            if (!strcmp_P( strupr(s_cmd), PSTR("EN")) ) {
                 if (!strcmp_P( strupr(s_param), PSTR("ON")) ) {
                    PUMP0_DISABLE();
                    return(true);
                 }
                 if (!strcmp_P( strupr(s_param), PSTR("OFF")) ) {
                    PUMP0_ENABLE();
                    return(true);
                 }
                 return(false);
            }
            
            if (!strcmp_P( strupr(s_cmd), PSTR("STEP")) ) {
                 if (!strcmp_P( strupr(s_param), PSTR("ON")) ) {
                     PUMP0_STEP_ON();
                     return(true);
                 }
                 if (!strcmp_P( strupr(s_param), PSTR("OFF")) ) {
                     PUMP0_STEP_OFF();
                     return(true);
                 }
                 if (!strcmp_P( strupr(s_param), PSTR("TG")) ) {
                     PUMP0_STEP_TOGGLE();
                     return(true);
                 }
                 return(false);
            }
            
            if (!strcmp_P( strupr(s_cmd), PSTR("DIR")) ) {
                 if (!strcmp_P( strupr(s_param), PSTR("ON")) ) {
                     PUMP0_FORWARD();
                     return(true);
                 }
                 if (!strcmp_P( strupr(s_param), PSTR("OFF")) ) {
                     PUMP0_REVERSE();
                     return(true);
                 }
                 return(false);
            }
            
            return(false);
            break; 
            
        case 1:
            if (!strcmp_P( strupr(s_cmd), PSTR("EN")) ) {
                 if (!strcmp_P( strupr(s_param), PSTR("ON")) ) {
                    PUMP1_DISABLE();
                    return(true);
                 }
                 if (!strcmp_P( strupr(s_param), PSTR("OFF")) ) {
                    PUMP1_ENABLE();
                    return(true);
                 }
                 return(false);
            }
            
            if (!strcmp_P( strupr(s_cmd), PSTR("STEP")) ) {
                 if (!strcmp_P( strupr(s_param), PSTR("ON")) ) {
                     PUMP1_STEP_ON();
                     return(true);
                 }
                 if (!strcmp_P( strupr(s_param), PSTR("OFF")) ) {
                     PUMP1_STEP_OFF();
                     return(true);
                 }
                 if (!strcmp_P( strupr(s_param), PSTR("TG")) ) {
                     PUMP1_STEP_TOGGLE();
                     return(true);
                 }
                 return(false);
            }
            
            if (!strcmp_P( strupr(s_cmd), PSTR("DIR")) ) {
                 if (!strcmp_P( strupr(s_param), PSTR("ON")) ) {
                     PUMP1_FORWARD();
                     return(true);
                 }
                 if (!strcmp_P( strupr(s_param), PSTR("OFF")) ) {
                     PUMP1_REVERSE();
                     return(true);
                 }
                 return(false);
            }
            
            return(false);
            break; 
            
        case 2:
            if (!strcmp_P( strupr(s_cmd), PSTR("EN")) ) {
                 if (!strcmp_P( strupr(s_param), PSTR("ON")) ) {
                    PUMP2_DISABLE();
                    return(true);
                 }
                 if (!strcmp_P( strupr(s_param), PSTR("OFF")) ) {
                    PUMP2_ENABLE();
                    return(true);
                 }
                 return(false);
            }
            
            if (!strcmp_P( strupr(s_cmd), PSTR("STEP")) ) {
                 if (!strcmp_P( strupr(s_param), PSTR("ON")) ) {
                     PUMP2_STEP_ON();
                     return(true);
                 }
                 if (!strcmp_P( strupr(s_param), PSTR("OFF")) ) {
                     PUMP2_STEP_OFF();
                     return(true);
                 }
                 if (!strcmp_P( strupr(s_param), PSTR("TG")) ) {
                     PUMP2_STEP_TOGGLE();
                     return(true);
                 }
                 return(false);
            }
            
            if (!strcmp_P( strupr(s_cmd), PSTR("DIR")) ) {
                 if (!strcmp_P( strupr(s_param), PSTR("ON")) ) {
                     PUMP2_FORWARD();
                     return(true);
                 }
                 if (!strcmp_P( strupr(s_param), PSTR("OFF")) ) {
                     PUMP2_REVERSE();
                     return(true);
                 }
                 return(false);
            }
            
            return(false);
            break; 
    }

    return(false);
    
}
//------------------------------------------------------------------------------
void pump_print_status(void)
{
         
    xprintf_P(PSTR(" P0: Freq=%d,"), pump0.freq);
    if ( pump0.enabled == true ) {
        xprintf_P(PSTR("(en),  "));
    } else {
        xprintf_P(PSTR("(dis), "));
    }
        
    if ( pump0.running ) {
        xprintf_P(PSTR("running (%lu)\r\n"), pump0.ticks);
    } else {
        xprintf_P(PSTR("stopped\r\n"));
    }
    //
    xprintf_P(PSTR(" P1: Freq=%d,"), pump1.freq);
    if ( pump1.enabled == true ) {
        xprintf_P(PSTR("(en),  "));
    } else {
        xprintf_P(PSTR("(dis), "));
    }
        
    if ( pump1.running ) {
        xprintf_P(PSTR("running (%lu)\r\n"), pump1.ticks);
    } else {
        xprintf_P(PSTR("stopped\r\n"));
    }
    //
    xprintf_P(PSTR(" P2: Freq=%d,"), pump2.freq);
    if ( pump2.enabled == true ) {
        xprintf_P(PSTR("(en),  "));
    } else {
        xprintf_P(PSTR("(dis), "));
    }
        
    if ( pump2.running ) {
        xprintf_P(PSTR("running (%lu)\r\n"), pump2.ticks);
    } else {
        xprintf_P(PSTR("stopped\r\n"));
    }        
    
}
//------------------------------------------------------------------------------
bool pump_config( char *s_id, char *s_freq )
{
 
    uint8_t pump_id = atoi(s_id);
    uint16_t pump_freq = atoi(s_freq);
    
    switch(pump_id) {
        case 0:
            pump0.freq = pump_freq;
            pump0_stop();
            TCB1.CCMP = configCPU_CLOCK_HZ / pump_freq;
            return(true);
            break;
        case 1:
            pump1.freq = pump_freq;
            pump1_stop();
            TCB2.CCMP = configCPU_CLOCK_HZ / pump_freq;
            return(true);
            break;
        case 2:
            pump2.freq = pump_freq;
            pump2_stop();
            TCB3.CCMP = configCPU_CLOCK_HZ / pump_freq;
            return(true);
            break;
        default:
            return(false);
    }
        
    return(false);
}
//------------------------------------------------------------------------------
void pump_config_default(void)
{
    pump0.freq = 1000;    
    pump0_stop();
    TCB1.CCMP = configCPU_CLOCK_HZ / pump0.freq;

    pump1.freq = 1000;    
    pump1_stop();
    TCB2.CCMP = configCPU_CLOCK_HZ / pump1.freq;

    pump2.freq = 1000;    
    pump2_stop();
    TCB3.CCMP = configCPU_CLOCK_HZ / pump2.freq;
    
}
//------------------------------------------------------------------------------
void pump_update_config(void)
{

    pump0.freq = systemVars.pump_freq[0];    
    pump0_stop();
    TCB1.CCMP = configCPU_CLOCK_HZ / pump0.freq;

    pump1.freq = systemVars.pump_freq[1];     
    pump1_stop();
    TCB2.CCMP = configCPU_CLOCK_HZ / pump1.freq;

    pump2.freq = systemVars.pump_freq[2];     
    pump2_stop();
    TCB3.CCMP = configCPU_CLOCK_HZ / pump2.freq;
    
}
//------------------------------------------------------------------------------
