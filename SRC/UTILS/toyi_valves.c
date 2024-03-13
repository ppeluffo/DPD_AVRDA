
#include "toyi_valves.h"

// -----------------------------------------------------------------------------
void TOYI_VALVES_init(void)
{
    CONFIG_VALVE1_CTRL();
    CONFIG_VALVE2_CTRL();
    CONFIG_VALVE3_CTRL();
    
    CLOSE_VALVE1();
    CLOSE_VALVE2();
    CLOSE_VALVE3();
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

        case 3:
            if (!strcmp_P( strupr(action), PSTR("OPEN")) ) {
                OPEN_VALVE3();
                return(true);
            }
            if (!strcmp_P( strupr(action), PSTR("CLOSE")) ) {
                CLOSE_VALVE3();
                return(true);
            }  
            return(false);
            break;
    }
    return(false);
}
//------------------------------------------------------------------------------
