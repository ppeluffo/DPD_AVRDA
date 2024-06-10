
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
}
// -----------------------------------------------------------------------------
void get_valve_status(vstatus_t *lvstatus)
{
    memcpy( lvstatus, &vstatus, sizeof(vstatus_t));
}
// -----------------------------------------------------------------------------
bool valve_tests( char *valve_id, char *action)
{
     
	// valve id {open|close}

    switch( atoi(valve_id)) {
        case 0:
            if (!strcmp_P( strupr(action), PSTR("OPEN")) ) {
                OPEN_VALVE0();
                return(true);
            }
            if (!strcmp_P( strupr(action), PSTR("CLOSE")) ) {
                CLOSE_VALVE0();
                return(true);
            }  
            return(false);
            break;
            
        case 1:
            if (!strcmp_P( strupr(action), PSTR("OPEN")) ) {
                OPEN_VALVE1();
                return(true);
            }
            if (!strcmp_P( strupr(action), PSTR("CLOSE")) ) {
                CLOSE_VALVE1();
                return(true);
            }  
            return(false);
            break;

        case 2:
            if (!strcmp_P( strupr(action), PSTR("OPEN")) ) {
                OPEN_VALVE2();
                return(true);
            }
            if (!strcmp_P( strupr(action), PSTR("CLOSE")) ) {
                CLOSE_VALVE2();
                return(true);
            }  
            return(false);
            break;
    }
    return(false);
}
//------------------------------------------------------------------------------
