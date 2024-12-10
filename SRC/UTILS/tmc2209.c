
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
    TCB1.CCMP = configCPU_CLOCK_HZ / pumpCB_0.freq;
    TCB1.INTCTRL = TCB_CAPT_bm; 
    TCB1.CTRLA = TCB_ENABLE_bm;
    
    pumpCB_0.enabled = true;
    pumpCB_0.running = false;
                
}
// ---------------------------------------------------------------
ISR( TCB1_INT_vect )
{
    CLR_INT( TCB1_INTFLAGS, TCB_CAPT_bm );
    PUMP0_STEP_TOGGLE();
    if ( pumpCB_0.ticks-- == 0 ) {
       // Stop
        TCB1.CTRLA = 0x00;
        PUMP0_STEP_OFF();
        PUMP0_DISABLE();
        pumpCB_0.running = false;
        pumpCB_0.enabled = false;
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
    TCB2.CCMP = configCPU_CLOCK_HZ /pumpCB_1.freq;
    TCB2.INTCTRL = TCB_CAPT_bm; 
    TCB2.CTRLA = TCB_ENABLE_bm;
    
    pumpCB_1.enabled = true;
    pumpCB_1.running = false;               
}
// ---------------------------------------------------------------
ISR( TCB2_INT_vect )
{
    CLR_INT( TCB2_INTFLAGS, TCB_CAPT_bm );
    PUMP1_STEP_TOGGLE();
    if ( pumpCB_1.ticks-- == 0 ) {
       // Stop
        TCB2.CTRLA = 0x00;
        PUMP1_STEP_OFF();
        PUMP1_DISABLE();
        pumpCB_1.running = false;
        pumpCB_1.enabled = false;
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
    TCB3.CCMP = configCPU_CLOCK_HZ / pumpCB_2.freq;
    TCB3.INTCTRL = TCB_CAPT_bm; 
    TCB3.CTRLA = TCB_ENABLE_bm;
     
    pumpCB_2.enabled = true;
    pumpCB_2.running = false;
}
// ---------------------------------------------------------------
ISR( TCB3_INT_vect )
{
    CLR_INT( TCB3_INTFLAGS, TCB_CAPT_bm );
    PUMP2_STEP_TOGGLE();
    if ( pumpCB_2.ticks-- == 0 ) {
       // Stop
        TCB3.CTRLA = 0x00;
        PUMP2_STEP_OFF();
        PUMP2_DISABLE();
        pumpCB_2.running = false;
        pumpCB_2.enabled = false;
    }
}
//---------------------------------------------------------------
void pump_print_status(void)
{
         
    xprintf_P(PSTR(" P0: Freq=%d,"), pumps_conf.pump0_freq);
    if ( pumpCB_0.enabled == true ) {
        xprintf_P(PSTR("(en),  "));
    } else {
        xprintf_P(PSTR("(dis), "));
    }
        
    if ( pumpCB_0.running ) {
        xprintf_P(PSTR("running (%lu)\r\n"), pumpCB_0.ticks);
    } else {
        xprintf_P(PSTR("stopped\r\n"));
    }
    //
    xprintf_P(PSTR(" P1: Freq=%d,"), pumps_conf.pump1_freq);
    if ( pumpCB_1.enabled == true ) {
        xprintf_P(PSTR("(en),  "));
    } else {
        xprintf_P(PSTR("(dis), "));
    }
        
    if ( pumpCB_1.running ) {
        xprintf_P(PSTR("running (%lu)\r\n"), pumpCB_1.ticks);
    } else {
        xprintf_P(PSTR("stopped\r\n"));
    }
    //
    xprintf_P(PSTR(" P2: Freq=%d,"), pumps_conf.pump2_freq);
    if ( pumpCB_2.enabled == true ) {
        xprintf_P(PSTR("(en),  "));
    } else {
        xprintf_P(PSTR("(dis), "));
    }
        
    if ( pumpCB_2.running ) {
        xprintf_P(PSTR("running (%lu)\r\n"), pumpCB_2.ticks);
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
            pumps_conf.pump0_freq = pump_freq;
            pump_0_stop(false);
            TCB1.CCMP = configCPU_CLOCK_HZ / pump_freq;
            return(true);
            break;
        case 1:
            pumps_conf.pump1_freq = pump_freq;
            pump_1_stop(false);
            TCB2.CCMP = configCPU_CLOCK_HZ / pump_freq;
            return(true);
            break;
        case 2:
            pumps_conf.pump2_freq = pump_freq;
            pump_2_stop(false);
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
    pumps_conf.pump0_freq = 1200;    
    pump_0_stop(false);
    TCB1.CCMP = configCPU_CLOCK_HZ / pumps_conf.pump0_freq;

    pumps_conf.pump1_freq = 1200;    
    pump_1_stop(false);
    TCB2.CCMP = configCPU_CLOCK_HZ / pumps_conf.pump1_freq;

    pumps_conf.pump2_freq = 12800;    
    pump_2_stop(false);
    TCB3.CCMP = configCPU_CLOCK_HZ / pumps_conf.pump2_freq;
    
}
//------------------------------------------------------------------------------
// ACCIONES BASICAS
//------------------------------------------------------------------------------
void pump_0_run(bool debug, uint16_t secs)
{
    
    pumpCB_0.secs = secs;
    pumpCB_0.debug = debug;

    cbk_pump_0_run(); 
}
// -----------------------------------------------------------------------------
void cbk_pump_0_run(void)
{
    
    if (pumpCB_0.debug)
        xprintf_P(PSTR("Pump 0 run %d secs\r\n"), pumpCB_0.secs);
    
    pumpCB_0.freq = pumps_conf.pump0_freq;
    //pumpCB_0.ticks = (uint32_t)(pumpCB_0.secs * pumpCB_0.freq);
    pumpCB_0.ticks = pumpCB_0.secs;
    pumpCB_0.ticks *= pumpCB_0.freq;

    //xprintf_P(PSTR("PUMP0 secs %d\r\n"), pumpCB_0.secs);
    //xprintf_P(PSTR("PUMP0 freq %d\r\n"), pumpCB_0.freq);
    //xprintf_P(PSTR("PUMP0 ticks %lu\r\n"), pumpCB_0.ticks);
    
    PUMP0_ENABLE();
    PUMP0_FORWARD();
    TCB1.CCMP = configCPU_CLOCK_HZ / pumpCB_0.freq;
    TCB1.INTFLAGS = TCB_CAPT_bm;                         
    TCB1.CTRLA = TCB_ENABLE_bm; 
    
    pumpCB_0.running = true;
    pumpCB_0.enabled = true;

}
// -----------------------------------------------------------------------------
void pump_0_stop(bool debug)
{
    pumpCB_0.debug = debug;
    cbk_pump_0_stop();
}
// -----------------------------------------------------------------------------
void cbk_pump_0_stop(void)
{
    if (pumpCB_0.debug)
        xprintf_P(PSTR("Pump 0 stop\r\n"));
    
    TCB1.CTRLA = 0x00;
    PUMP0_STEP_OFF();
    PUMP0_DISABLE();
    
    pumpCB_0.running = false;
    pumpCB_0.enabled = false;
}
// -----------------------------------------------------------------------------
void pump_1_run(bool debug, uint16_t secs)
{
    
    pumpCB_1.secs = secs;
    pumpCB_1.debug = debug;

    cbk_pump_1_run(); 
}
// -----------------------------------------------------------------------------
void cbk_pump_1_run(void)
{
    if (pumpCB_1.debug)
        xprintf_P(PSTR("Pump 1 run %d secs\r\n"), pumpCB_1.secs);
    
    pumpCB_1.freq = pumps_conf.pump1_freq;
    //pumpCB_1.ticks = (uint32_t)(pumpCB_1.secs * pumpCB_1.freq);
    pumpCB_1.ticks = pumpCB_1.secs;
    pumpCB_1.ticks *= pumpCB_1.freq;
    
    //xprintf_P(PSTR("PUMP1 secs %d\r\n"), pumpCB_1.secs);
    //xprintf_P(PSTR("PUMP1 freq %d\r\n"), pumpCB_1.freq);
    //xprintf_P(PSTR("PUMP1 ticks %lu\r\n"), pumpCB_1.ticks);
    
    PUMP1_ENABLE();
    PUMP1_FORWARD();
    TCB2.CCMP = configCPU_CLOCK_HZ / pumpCB_1.freq;
    TCB2.INTFLAGS = TCB_CAPT_bm;                         
    TCB2.CTRLA = TCB_ENABLE_bm; 
    
    pumpCB_1.running = true;
    pumpCB_1.enabled = true;
    
}
// -----------------------------------------------------------------------------
void pump_1_stop(bool debug)
{
    pumpCB_1.debug = debug;
    cbk_pump_1_stop();
}
// -----------------------------------------------------------------------------
void cbk_pump_1_stop(void)
{
    if (pumpCB_1.debug)
        xprintf_P(PSTR("Pump 1 stop\r\n"));
    
    TCB2.CTRLA = 0x00;
    PUMP1_STEP_OFF();
    PUMP1_DISABLE();
    
    pumpCB_1.running = false;
    pumpCB_1.enabled = false;
}
// -----------------------------------------------------------------------------
void pump_2_run(bool debug, uint16_t secs)
{

    pumpCB_2.secs = secs;
    pumpCB_2.debug = debug;

    cbk_pump_2_run();
    
}
// -----------------------------------------------------------------------------
void cbk_pump_2_run(void)
{
    
 //   uint32_t gvar;
    
    if (pumpCB_2.debug)
        xprintf_P(PSTR("Pump 2 run %d secs\r\n"), pumpCB_2.secs);
        
    pumpCB_2.freq = pumps_conf.pump2_freq;
    
    pumpCB_2.ticks = pumpCB_2.secs;
    pumpCB_2.ticks *= pumpCB_2.freq;   
   // pumpCB_2.ticks = (uint32_t)(pumpCB_2.secs * pumpCB_2.freq);
    
    //xprintf_P(PSTR("PUMP2 secs %d\r\n"), pumpCB_2.secs);
    //xprintf_P(PSTR("PUMP2 freq %d\r\n"), pumpCB_2.freq);
    //xprintf_P(PSTR("PUMP2 ticks %lu\r\n"), pumpCB_2.ticks);
    
    PUMP2_ENABLE();
    PUMP2_FORWARD();
    TCB3.CCMP = configCPU_CLOCK_HZ / pumpCB_2.freq;
    TCB3.INTFLAGS = TCB_CAPT_bm;                         
    TCB3.CTRLA = TCB_ENABLE_bm; 
    
    pumpCB_2.running = true;
    pumpCB_2.enabled = true;
    
}
// -----------------------------------------------------------------------------
void pump_2_stop(bool debug)
{
    pumpCB_2.debug = debug;
    cbk_pump_2_stop();
}
// -----------------------------------------------------------------------------
void cbk_pump_2_stop(void)
{
    if (pumpCB_2.debug)
        xprintf_P(PSTR("Pump 2 stop\r\n"));
    
    TCB2.CTRLA = 0x00;
    PUMP2_STEP_OFF();
    PUMP2_DISABLE();
    
    pumpCB_2.running = false;
    pumpCB_2.enabled = false;
}
// -----------------------------------------------------------------------------
/*
void pump0_run(uint16_t secs)
{
    xprintf_P(PSTR("PUMP0 run %d\r\n"), secs);
    PUMP0_ENABLE();
    PUMP0_FORWARD();
    
    TCB1.INTFLAGS = TCB_CAPT_bm;                         
    TCB1.CTRLA = TCB_ENABLE_bm; 
    
    //pump0.ticks = secs * pump0.freq;
    pumpCB_0.ticks = (uint32_t)secs * pumpCB_0.freq;
    //xprintf_P(PSTR("PUMP0 ticks %lu\r\n"), pump0.ticks);
    pumpCB_0.running = true;
    pumpCB_0.enabled = true;
}
 */
//----------------------------------------------------------------
/*
void pump0_stop(void)
{
    xprintf_P(PSTR("PUMP0 stop\r\n"));
    TCB1.CTRLA = 0x00;
    PUMP0_STEP_OFF();
    PUMP0_DISABLE();
    
    pumpCB_0.running = false;
    pumpCB_0.enabled = false;
}
 */
/*
void pump1_run(uint16_t secs)
{
    xprintf_P(PSTR("PUMP1 run %d\r\n"), secs);
    PUMP1_ENABLE();
    PUMP1_FORWARD();
    
    TCB2.INTFLAGS = TCB_CAPT_bm;                         
    TCB2.CTRLA = TCB_ENABLE_bm;  
    //pumpCB_1.ticks = secs * pumpCB_1.freq;
    pumpCB_1.ticks = (uint32_t)secs * pumpCB_1.freq;
    //xprintf_P(PSTR("PUMP1 ticks %lu\r\n"), pumpCB_1.ticks);
    pumpCB_1.running = true;
    pumpCB_1.enabled = true;
}
 */
//----------------------------------------------------------------
/*
void pump1_stop(void)
{
    xprintf_P(PSTR("PUMP1 stop\r\n"));
    TCB2.CTRLA = 0x00;
    PUMP1_STEP_OFF();
    PUMP1_DISABLE();
    pumpCB_1.running = false;
    pumpCB_1.enabled = false;
}
 */
// ---------------------------------------------------------------
/*
void pump2_run(uint16_t secs)
{
    xprintf_P(PSTR("PUMP2 run %d\r\n"), secs);
    PUMP2_ENABLE();
    PUMP2_FORWARD();
    
    TCB3.INTFLAGS = TCB_CAPT_bm;                         
    TCB3.CTRLA = TCB_ENABLE_bm;  
    //pumpCB_2.ticks = secs * pumpCB_2.freq;
    pumpCB_2.ticks = (uint32_t)secs * pumpCB_2.freq;
    //xprintf_P(PSTR("PUMP2 ticks %lu\r\n"), pumpCB_2.ticks);
    pumpCB_2.running = true;
    pumpCB_2.enabled = true;   
}
 */
//----------------------------------------------------------------
/*
void pump2_stop(void)
{
    xprintf_P(PSTR("PUMP2 stop\r\n"));
    TCB3.CTRLA = 0x00;
    PUMP2_STEP_OFF();
    PUMP2_DISABLE();
    pumpCB_2.running = false;
    pumpCB_2.enabled = false;
}
 */
// ---------------------------------------------------------------
