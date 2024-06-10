
#include "opto.h"

//------------------------------------------------------------------------------
bool opto_get_status(void)
{
    return (opto_status);
}
//------------------------------------------------------------------------------
bool opto_test(char *s_action)
{
    if ( !strcmp_P( strupr(s_action), PSTR("ON"))) {
        PRENDER_OPTO();
        return(true);
    }
    
    if ( !strcmp_P( strupr(s_action), PSTR("OFF"))) {
        APAGAR_OPTO();
        return(true);
    }
    
    return(false);
}
//------------------------------------------------------------------------------
