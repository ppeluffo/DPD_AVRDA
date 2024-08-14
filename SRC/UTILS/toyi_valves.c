
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
bool valve_tests( char *valve_id, char *action)
{
     
uint8_t vid = atoi(valve_id);
    
	// valve id {open|close}

    if (!strcmp_P( strupr(action), PSTR("OPEN")) ) {
        return ( valve_open(vid));
    }

    if (!strcmp_P( strupr(action), PSTR("CLOSE")) ) {
        return ( valve_close(vid));
    }

    return(false);
}
//------------------------------------------------------------------------------
