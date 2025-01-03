
#include "toyi_valves.h"

// -----------------------------------------------------------------------------
void TOYI_VALVES_init(void)
{
    CONFIG_VALVE0_CTRL();
    CONFIG_VALVE1_CTRL();
    CONFIG_VALVE2_CTRL();
    
    CLOSE_VALVE0();
    CLOSE_VALVE1();
    CLOSE_VALVE2();
    
    vstatus.vs0 = VALVE_CLOSE;
    vstatus.vs1 = VALVE_CLOSE;
    vstatus.vs2 = VALVE_CLOSE;
}
// -----------------------------------------------------------------------------
bool valve_open(uint8_t valve_id) 
{
    
    switch(valve_id) {
        case 0:
            OPEN_VALVE0();
            vstatus.vs0 = VALVE_OPEN;
            break;
        case 1:
            OPEN_VALVE1();
            vstatus.vs1 = VALVE_OPEN;
            break;
        case 2:
            OPEN_VALVE2();
            vstatus.vs2 = VALVE_OPEN;
            break;
        default:
            xprintf_P(PSTR("Valve id debe estar en el rango 0..2\r\n"));
            return(false); 
    }
    return(true);
}
// -----------------------------------------------------------------------------
bool valve_close(uint8_t valve_id) 
{
    
    switch(valve_id) {
        case 0:
            CLOSE_VALVE0();
            vstatus.vs0 = VALVE_CLOSE;
            break;
        case 1:
            CLOSE_VALVE1();
            vstatus.vs1 = VALVE_CLOSE;
            break;
        case 2:
            CLOSE_VALVE2();
            vstatus.vs2 = VALVE_CLOSE;
            break;
        default:
            xprintf_P(PSTR("Valve id debe estar en el rango 0..2\r\n"));
            return(false); 
    }
    return(true);
}
// -----------------------------------------------------------------------------
void valve_get_status(vstatus_t *lvstatus)
{
    memcpy( lvstatus, &vstatus, sizeof(vstatus_t));
}
// -----------------------------------------------------------------------------
void valve_print_status(void)
{
    if (vstatus.vs0 == VALVE_OPEN ) {
        xprintf_P(PSTR(" V0:OPEN\r\n"));
    } else {
        xprintf_P(PSTR(" V0:CLOSE\r\n"));
    }
    
    if (vstatus.vs1 == VALVE_OPEN ) {
        xprintf_P(PSTR(" V1:OPEN\r\n"));
    } else {
        xprintf_P(PSTR(" V1:CLOSE\r\n"));
    }
    
    if (vstatus.vs2 == VALVE_OPEN ) {
        xprintf_P(PSTR(" V2:OPEN\r\n"));
    } else {
        xprintf_P(PSTR(" V2:CLOSE\r\n"));
    }

    
}
// -----------------------------------------------------------------------------
void valve_0_open(bool debug)
{
    valveCB_0.debug = debug;
    cbk_valve_0_open();
}
// -----------------------------------------------------------------------------
void cbk_valve_0_open(void)
{
    if (valveCB_0.debug)
        xprintf_P(PSTR("Valve 0 open\r\n"));
    
    OPEN_VALVE0();
    vstatus.vs0 = VALVE_OPEN; 
}
//------------------------------------------------------------------------------
void valve_0_close(bool debug)
{
    valveCB_0.debug = debug;
    cbk_valve_0_close();
}
// -----------------------------------------------------------------------------
void cbk_valve_0_close(void)
{
    if (valveCB_0.debug)
        xprintf_P(PSTR("Valve 0 close\r\n"));
    
    CLOSE_VALVE0();
    vstatus.vs0 = VALVE_CLOSE; 
}
//------------------------------------------------------------------------------
void valve_1_open(bool debug)
{
    valveCB_1.debug = debug;
    cbk_valve_1_open();
}
// -----------------------------------------------------------------------------
void cbk_valve_1_open(void)
{
    if (valveCB_1.debug)
        xprintf_P(PSTR("Valve 1 open\r\n"));
    
    OPEN_VALVE1();
    vstatus.vs1 = VALVE_OPEN; 
}
//------------------------------------------------------------------------------
void valve_1_close(bool debug)
{
    valveCB_1.debug = debug;
    cbk_valve_1_close();
}
// -----------------------------------------------------------------------------
void cbk_valve_1_close(void)
{
    if (valveCB_1.debug)
        xprintf_P(PSTR("Valve 1 close\r\n"));
     
    CLOSE_VALVE1();
    vstatus.vs1 = VALVE_CLOSE; 
}
//------------------------------------------------------------------------------
void valve_2_open(bool debug)
{
    valveCB_2.debug = debug;
    cbk_valve_2_open();
}
// -----------------------------------------------------------------------------
void cbk_valve_2_open(void)
{
    if (valveCB_2.debug)
        xprintf_P(PSTR("Valve 2 open\r\n"));
     
    OPEN_VALVE2();
    vstatus.vs2 = VALVE_OPEN;
    
}
//------------------------------------------------------------------------------
void valve_2_close(bool debug)
{
    valveCB_2.debug = debug;
    cbk_valve_2_close();
}
// -----------------------------------------------------------------------------
void cbk_valve_2_close(void)
{
    if (valveCB_2.debug)
        xprintf_P(PSTR("Valve 2 close\r\n"));
    
    CLOSE_VALVE2();
    vstatus.vs2 = VALVE_CLOSE; 
}
//------------------------------------------------------------------------------
bool V0_isClosed(void)
{
bool retS;
    
    (vstatus.vs0 == VALVE_CLOSE)? (retS = true): (retS = false);  
    return (retS);
}
//------------------------------------------------------------------------------
bool V1_isClosed(void)
{
bool retS;
    
    (vstatus.vs1 == VALVE_CLOSE)? (retS = true): (retS = false);  
    return (retS);
}
//------------------------------------------------------------------------------
bool V2_isClosed(void)
{
bool retS;
    
    (vstatus.vs2 == VALVE_CLOSE)? (retS = true): (retS = false);  
    return (retS);  
}
//------------------------------------------------------------------------------
