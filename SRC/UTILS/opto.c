
#include "opto.h"

//------------------------------------------------------------------------------
bool opto_get_status(void)
{
    return (opto_status);
}
//------------------------------------------------------------------------------
void fn_opto_on(void)
{
    if (optoCB.debug) {
        xprintf_P(PSTR("Led on\r\n"));
    }
    
    PRENDER_OPTO();
    opto_status = true;
}
//------------------------------------------------------------------------------
void fn_opto_off(void)
{
    if (optoCB.debug) {
        xprintf_P(PSTR("Led off\r\n"));
    }
    
    APAGAR_OPTO();
    opto_status = false;
}
//------------------------------------------------------------------------------
