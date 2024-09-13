
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
// ACCIONES BASICAS
//------------------------------------------------------------------------------
void f_opto_on(bool f_debug, uint16_t dummyarg0, uint16_t dummyarg1)
{
    //if (f_debug)
    //    xprintf_P(PSTR("Led on\r\n"));
    
    PRENDER_OPTO();
}
//------------------------------------------------------------------------------
void f_opto_off(bool f_debug, uint16_t dummyarg0, uint16_t dummyarg1)
{
    //if (f_debug)
    //    xprintf_P(PSTR("Led off\r\n"));
    
    APAGAR_OPTO();
}
//------------------------------------------------------------------------------
