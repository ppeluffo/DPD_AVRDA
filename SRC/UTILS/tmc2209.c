
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
    /*
     * Los valores son los indicados por quimica como los optimos
     * para nuestras bombas.
     *
     */
    pump0.freq = 1200;    
    pump0_stop();
    TCB1.CCMP = configCPU_CLOCK_HZ / pump0.freq;

    pump1.freq = 1200;    
    pump1_stop();
    TCB2.CCMP = configCPU_CLOCK_HZ / pump1.freq;

    pump2.freq = 12800;    
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
// ACCIONES BASICAS
//------------------------------------------------------------------------------
void fn_pump_0_run(void)
{
    
uint16_t secs;
    
    secs = pumpCB_0.secs;

    if (pumpCB_0.debug)
        xprintf_P(PSTR("Pump 0 run %d secs\r\n"), secs);
    
    PUMP0_ENABLE();
    PUMP0_FORWARD();
    
    TCB1.INTFLAGS = TCB_CAPT_bm;                         
    TCB1.CTRLA = TCB_ENABLE_bm;  
    //pump1.ticks = secs * pump1.freq;
    pump0.ticks = (uint32_t)secs * pump0.freq;
    //xprintf_P(PSTR("PUMP1 ticks %lu\r\n"), pump1.ticks);
    pump0.running = true;
    pump0.enabled = true;

}
// -----------------------------------------------------------------------------
void fn_pump_0_stop(void)
{
    if (pumpCB_0.debug)
        xprintf_P(PSTR("Pump 0 stop\r\n"));
    
    TCB1.CTRLA = 0x00;
    PUMP0_STEP_OFF();
    PUMP0_DISABLE();
    
    pump0.running = false;
    pump0.enabled = false;
}
// -----------------------------------------------------------------------------
void fn_pump_1_run(void)
{
    
uint16_t secs;
    
    secs = pumpCB_1.secs;

    if (pumpCB_1.debug)
        xprintf_P(PSTR("Pump 1 run %d secs\r\n"), secs);
    
    PUMP1_ENABLE();
    PUMP1_FORWARD();
    
    TCB2.INTFLAGS = TCB_CAPT_bm;                         
    TCB2.CTRLA = TCB_ENABLE_bm;  
    pump1.ticks = (uint32_t)secs * pump1.freq;
    pump1.running = true;
    pump1.enabled = true;

}
// -----------------------------------------------------------------------------
void fn_pump_1_stop(void)
{
    if (pumpCB_1.debug)
        xprintf_P(PSTR("Pump 1 stop\r\n"));
    
    TCB2.CTRLA = 0x00;
    PUMP1_STEP_OFF();
    PUMP1_DISABLE();
    
    pump1.running = false;
    pump1.enabled = false;
}
// -----------------------------------------------------------------------------
void fn_pump_2_run(void)
{
    
uint16_t secs;
    
    secs = pumpCB_2.secs;

    if (pumpCB_2.debug)
        xprintf_P(PSTR("Pump 2 run %d secs\r\n"), secs);
    
    PUMP2_ENABLE();
    PUMP2_FORWARD();
    
    TCB3.INTFLAGS = TCB_CAPT_bm;                         
    TCB3.CTRLA = TCB_ENABLE_bm;  
    pump2.ticks = (uint32_t)secs * pump2.freq;
    pump2.running = true;
    pump2.enabled = true;

}
// -----------------------------------------------------------------------------
void fn_pump_2_stop(void)
{
    if (pumpCB_2.debug)
        xprintf_P(PSTR("Pump 2 stop\r\n"));
    
    TCB3.CTRLA = 0x00;
    PUMP2_STEP_OFF();
    PUMP2_DISABLE();
    
    pump2.running = false;
    pump2.enabled = false;
}
// -----------------------------------------------------------------------------
