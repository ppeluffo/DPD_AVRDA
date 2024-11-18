#include "DPD_AVRDA.h"


FSMdisplay_CB_t FSMdisplay_CB;

void FSM_display_init(void);
void FSM_display(void);
void fsm_pantalla1(void);
void fsm_pantalla2(void);
void fsm_pantalla3(void);
void fsm_pantalla4(void);
void fsm_pantalla_conf_medida(void);
void fsm_pantalla_midiendo(void);

#define BACKLITE_TIMEOUT    60

//------------------------------------------------------------------------------
void tkLcd(void * pvParameters)
{

	/*
     * Esta tarea se encarga de recibir datos del display y
     * guardarlos en un buffer lineal.
     * Si se llena, BORRA EL BUFFER Y SE PIERDE TODO
     * No debería pasar porque antes lo debería haber procesado y flusheado
     * pero por las dudas. 
     */

( void ) pvParameters;

    while ( ! starting_flag )
        vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );

    SYSTEM_ENTER_CRITICAL();
    task_running |= LCD_WDG_gc;
    SYSTEM_EXIT_CRITICAL();
        
     
    xprintf_P(PSTR("Starting tkLcd..\r\n" ));
  
    FSM_display_init();

	// loop
	for( ;; )
	{
        u_kick_wdt(LCD_WDG_gc);
       
        while(true) {
            // Maquina de estados del LCD
            FSM_display();
            vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
        } 
    }
}
//------------------------------------------------------------------------------
void FSM_display_init(void)
{
    FSMdisplay_CB.keypress = NO_KEY;
    FSMdisplay_CB.pantalla = PANTALLA1;
    FSMdisplay_CB.state = LCD_INIT;
    FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
    lcd_cmd_clear();
}
//------------------------------------------------------------------------------
void fsm_set_keypressed(uint8_t key)
{
    FSMdisplay_CB.keypress = key;
}
//------------------------------------------------------------------------------
void fsm_pantalla1(void)
{
    
uint8_t msg[16];
    
    lcd_cmd_brightness(100);
    FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
    
    lcd_cmd_clear();
    
    memset(msg,'\0',sizeof(msg));
    sprintf_P( (char *)&msg[0], PSTR("SPY-DPD"));  
    lcd_cmd_send_data(0,0, msg);

    memset(msg,'\0',sizeof(msg));
    sprintf_P( (char *)&msg[0], PSTR("CL=%0.3f ppm"), systemVars.cloro_ppm);
    lcd_cmd_send_data(1,0, msg);
    
}
//------------------------------------------------------------------------------
void fsm_pantalla2(void)
{
    
uint8_t msg[16];
    
    lcd_cmd_brightness(100);
    FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
    lcd_cmd_clear();
        
    memset(msg,'\0',sizeof(msg));
    sprintf_P( (char *)&msg[0], PSTR("Tpoll=%03d"), systemConf.timerpoll);  
    lcd_cmd_send_data(0,0, msg);

    memset(msg,'\0',sizeof(msg));
    snprintf_P( (char *)&msg[0], sizeof(msg), PSTR("Tmed=%lu/%lu"), systemConf.timermedida, systemVars.time2medida);
    lcd_cmd_send_data(1,0, msg);
    
}
//------------------------------------------------------------------------------
void fsm_pantalla3(void)
{
    
uint8_t msg[16];
    
    lcd_cmd_brightness(100);
    FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
    lcd_cmd_clear();
        
    memset(msg,'\0',sizeof(msg));
    sprintf_P( (char *)&msg[0], PSTR("S0=%03d,S100=%03d"), systemConf.S0, systemConf.S100);  
    lcd_cmd_send_data(0,0, msg);

    memset(msg,'\0',sizeof(msg));
    sprintf_P( (char *)&msg[0], PSTR("Abs=%0.3f"), systemVars.absorbancia);
    lcd_cmd_send_data(1,0, msg);
    
}
//------------------------------------------------------------------------------
void fsm_pantalla4(void)
{
    
uint8_t msg[16];
    
    lcd_cmd_brightness(100);
    FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
    lcd_cmd_clear();
    
    memset(msg,'\0',sizeof(msg));
    sprintf_P( (char *)&msg[0], PSTR("ID=%s"), systemConf.dlgid);  
    lcd_cmd_send_data(0,0, msg);

    memset(msg,'\0',sizeof(msg));
    sprintf_P( (char *)&msg[0], PSTR("VER=%s"), FW_REV);
    lcd_cmd_send_data(1,0, msg);
    
}
//------------------------------------------------------------------------------
void fsm_pantalla_conf_medida(void)
{
    
uint8_t msg[16];

    //xprintf_P(PSTR("CONF_MEDIDA\r\n"));
    lcd_cmd_brightness(100);
    FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
    lcd_cmd_clear();
    
    memset(msg,'\0',sizeof(msg));
    sprintf_P( (char *)&msg[0], PSTR("Confirma con OK"));  
    
    lcd_cmd_send_data(0,0, msg);   
}
//------------------------------------------------------------------------------
void fsm_pantalla_midiendo(void)
{
uint8_t msg[16];

    //xprintf_P(PSTR("CONF_MEDIDA\r\n"));
    lcd_cmd_brightness(100);
    FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
    lcd_cmd_clear();
    
    memset(msg,'\0',sizeof(msg));
    sprintf_P( (char *)&msg[0], PSTR("Midiendo..."));  
    
    lcd_cmd_send_data(0,0, msg);
    vTaskDelay( ( TickType_t)( 2000 / portTICK_PERIOD_MS ) );
    
}
//------------------------------------------------------------------------------
void fsm_telepronter(char *msg)
{
static uint8_t msg0[16];
static uint8_t msg1[16];

// Paso la linea 1 a la 0
    FSMdisplay_CB.timerBackliteOff = 127;
    lcd_cmd_clear();
    memset(msg0,'\0', sizeof(msg0));
    strncpy( (char *)&msg0[0], msg, sizeof(msg0)); 
    lcd_cmd_send_data(0,0, msg0);
    return;
    
    memset(msg0,'\0', sizeof(msg0));
    memcpy(msg0, msg1, sizeof(msg0));
    // Copio el mensaje en la linea 1
    memset(msg1,'\0', sizeof(msg1));
    strncpy(msg1, msg, sizeof(msg1));  
        
    lcd_cmd_send_data(0,0, msg0);
    lcd_cmd_send_data(1,0, msg1);
    
}
//------------------------------------------------------------------------------
void fsm_display_modo_medir(void)
{
    FSMdisplay_CB.state = LCD_MIDIENDO;
    FSMdisplay_CB.pantalla = PANTALLA_MIDIENDO;
    FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
    FSMdisplay_CB.keypress = NO_KEY;
    lcd_cmd_clear();
   
}
//------------------------------------------------------------------------------
void FSM_display(void)
{

    //xprintf_P(PSTR("LCD DEBUG: BKLITE %d\r\n"), FSMdisplay_CB.timerBackliteOff );
    
    // Timeout para backlite 
    if (FSMdisplay_CB.timerBackliteOff > 0) {
        FSMdisplay_CB.timerBackliteOff--;
        if ( FSMdisplay_CB.timerBackliteOff == 0 ) {
            //xprintf_P(PSTR("LCD DEBUG: BKLITE OFFr\n"));
            lcd_cmd_clear();
            lcd_cmd_brightness(0);
            FSMdisplay_CB.state = LCD_OFF;
        }
    }
        
    switch(FSMdisplay_CB.state) {
        
        case LCD_INIT:
            //xprintf_P(PSTR("LCD DEBUG: INIT\r\n"));
            FSMdisplay_CB.state = LCD_STANDBY;
            FSMdisplay_CB.pantalla = PANTALLA1;
            FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
            FSMdisplay_CB.keypress = NO_KEY;
            fsm_pantalla1();
            break;
            
        case LCD_OFF:
            if ( FSMdisplay_CB.keypress != NO_KEY ) {
                FSMdisplay_CB.state = LCD_STANDBY;
                FSMdisplay_CB.pantalla = PANTALLA1;
                FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
                FSMdisplay_CB.keypress = NO_KEY;
                fsm_pantalla1();  
            }
            break;
            
        case LCD_STANDBY:
            
            //xprintf_P(PSTR("LCD DEBUG: STANDBY\r\n"));
            
            if ( FSMdisplay_CB.keypress == KEY_ENTER_RELEASE ) {
                FSMdisplay_CB.state = LCD_CONF_MEDIDA;
                FSMdisplay_CB.pantalla = PANTALLA_CONF_MEDIDA;
                FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
                FSMdisplay_CB.keypress = NO_KEY;
                fsm_pantalla_conf_medida();
                break;
            }
            
            // Pantalla 1
            if ( ( FSMdisplay_CB.pantalla == PANTALLA1) && ( FSMdisplay_CB.keypress == KEY_UP_RELEASE) ) {
                FSMdisplay_CB.pantalla = PANTALLA4;
                FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
                FSMdisplay_CB.keypress = NO_KEY;
                fsm_pantalla4();
                break;
            } 
            
            if ( ( FSMdisplay_CB.pantalla == PANTALLA1) && ( FSMdisplay_CB.keypress == KEY_DOWN_RELEASE) ) {
                FSMdisplay_CB.pantalla = PANTALLA2;
                FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
                FSMdisplay_CB.keypress = NO_KEY;
                fsm_pantalla2();
                break;
            } 

            // Pantalla 2
            if ( ( FSMdisplay_CB.pantalla == PANTALLA2) && ( FSMdisplay_CB.keypress == KEY_DOWN_RELEASE) ) {
                FSMdisplay_CB.pantalla = PANTALLA3;
                FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
                FSMdisplay_CB.keypress = NO_KEY;
                fsm_pantalla3();
                break;
            }  

            if ( ( FSMdisplay_CB.pantalla == PANTALLA2) && ( FSMdisplay_CB.keypress == KEY_UP_RELEASE) ) {
                FSMdisplay_CB.pantalla = PANTALLA1;
                FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
                FSMdisplay_CB.keypress = NO_KEY;
                fsm_pantalla1();
                break;
            } 

            // Pantalla 3
            if ( ( FSMdisplay_CB.pantalla == PANTALLA3) && ( FSMdisplay_CB.keypress == KEY_DOWN_RELEASE) ) {
                FSMdisplay_CB.pantalla = PANTALLA4;
                FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
                FSMdisplay_CB.keypress = NO_KEY;
                fsm_pantalla4();
                break;
            }  

            if ( ( FSMdisplay_CB.pantalla == PANTALLA3) && ( FSMdisplay_CB.keypress == KEY_UP_RELEASE) ) {
                FSMdisplay_CB.pantalla = PANTALLA2;
                FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
                FSMdisplay_CB.keypress = NO_KEY;
                fsm_pantalla2();
                break;
            }             
                 
            // Pantalla 4
            
            if ( ( FSMdisplay_CB.pantalla == PANTALLA4) && ( FSMdisplay_CB.keypress == KEY_UP_RELEASE) ) {
                FSMdisplay_CB.pantalla = PANTALLA3;
                FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
                FSMdisplay_CB.keypress = NO_KEY;
                fsm_pantalla3();
                break;
            }
            
            if ( ( FSMdisplay_CB.pantalla == PANTALLA4) && ( FSMdisplay_CB.keypress == KEY_DOWN_RELEASE) ) {
                FSMdisplay_CB.pantalla = PANTALLA1;
                FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
                FSMdisplay_CB.keypress = NO_KEY;
                fsm_pantalla1();
                break;
            }
            
            break;
                 
        case LCD_CONF_MEDIDA:
            
            //xprintf_P(PSTR("LCD DEBUG: CONF_MEDIDA\r\n"));
            
            if ( FSMdisplay_CB.keypress == KEY_ENTER_RELEASE ) {
                FSMdisplay_CB.state = LCD_MIDIENDO;
                FSMdisplay_CB.pantalla = PANTALLA_MIDIENDO;
                FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
                FSMdisplay_CB.keypress = NO_KEY;
               
                fsm_pantalla_midiendo();
                action_medida_completa(true);
                break;
            }
            
            if ( FSMdisplay_CB.keypress == KEY_EXIT_RELEASE ) {
                FSMdisplay_CB.state = LCD_STANDBY;
                FSMdisplay_CB.pantalla = PANTALLA1;
                FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
                FSMdisplay_CB.keypress = NO_KEY;
                fsm_pantalla1();
                break;
            }
            
            break;
            
        case LCD_MIDIENDO:
            
            //xprintf_P(PSTR("LCD DEBUG: MIDIENDO\r\n"));
            if ( FSMdisplay_CB.keypress == KEY_DUMMY1 ) {
                FSMdisplay_CB.state = LCD_STANDBY;
                FSMdisplay_CB.pantalla = PANTALLA1;
                FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
                FSMdisplay_CB.keypress = NO_KEY;
                fsm_pantalla1();            
            }
            
        default:
            FSMdisplay_CB.state = LCD_STANDBY;
            FSMdisplay_CB.pantalla = PANTALLA1;
            FSMdisplay_CB.timerBackliteOff = BACKLITE_TIMEOUT;
            FSMdisplay_CB.keypress = NO_KEY;
            fsm_pantalla1();
            break;
            
    }
}
//------------------------------------------------------------------------------

