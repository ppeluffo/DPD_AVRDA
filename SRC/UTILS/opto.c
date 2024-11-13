
#include "opto.h"

//------------------------------------------------------------------------------
bool opto_get_status(void)
{
    return (opto_status);
}
//------------------------------------------------------------------------------
void opto_on( bool debug)
{
    optoCB.debug = debug;
    cbk_opto_on();
}
//------------------------------------------------------------------------------
void opto_off( bool debug)
{
    optoCB.debug = debug;
    cbk_opto_off();
}
//------------------------------------------------------------------------------
void cbk_opto_on(void)
{
    if (optoCB.debug) {
        xprintf_P(PSTR("Led on\r\n"));
    }
    
    PRENDER_OPTO();
    opto_status = true;
}
//------------------------------------------------------------------------------
void cbk_opto_off(void)
{
    if (optoCB.debug) {
        xprintf_P(PSTR("Led off\r\n"));
    }
    
    APAGAR_OPTO();
    opto_status = false;
}
//------------------------------------------------------------------------------
