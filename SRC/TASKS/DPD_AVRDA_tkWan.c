#include "DPD_AVRDA.h"
#include "frtos_cmd.h"
      
/*
 * El modem opera en modo continuo y solo transmite frames de PING y DATA
 * La orden que puede recibir es para que mida
 * 
 */
SemaphoreHandle_t sem_WAN;
StaticSemaphore_t WAN_xMutexBuffer;
#define MSTOTAKEWANSEMPH ((  TickType_t ) 10 )

static bool f_debug_comms = false;

typedef enum { WAN_APAGADO=0, WAN_OFFLINE, WAN_LINK, WAN_ONLINE_DATA } wan_states_t;

typedef enum { DATA=0, BLOCK, BLOCKEND } tx_type_t;

static uint8_t wan_state;

struct {
    dataRcd_s dr;
    bool dr_ready;
} drWanBuffer;

bool link_up4data;

static void wan_state_apagado(void);
static void wan_state_offline(void);
static void wan_state_link(void);
static void wan_state_online_data(void);

static bool wan_process_frame_ping(void);
static bool wan_process_rsp_ping(void);

static bool wan_process_frame_data(dataRcd_s *dr);
static bool wan_process_rsp_data(void);

void wan_prender_modem(void);
void wan_apagar_modem(void);

bool f_inicio;

static bool wan_check_response ( const char *s);
static void wan_xmit_out(void);
static void wan_print_RXbuffer(void);

fat_s l_fat1;

uint16_t uxHighWaterMark;

char tmpLocalStr[64] = { 0 };

#define PING_TRYES              5
#define RECOVERID_TRYES         2
#define DATA_TRYES              2


//------------------------------------------------------------------------------
void tkWan(void * pvParameters)
{

	/*
     * Esta tarea el la FSM de la WAN.
     * Cuando hay datos para transmitir recibe una señal y los transmite.
     * De otro modo, espera datos ( respuestas ) y las procesa.
     * El servidor solo envia respuestas: no trasmite por iniciativa.
     */

( void ) pvParameters;

    while ( ! starting_flag )
        vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );
    
    SYSTEM_ENTER_CRITICAL();
    //tk_running[TK_WAN] = true;
    SYSTEM_EXIT_CRITICAL();
    
    sem_WAN = xSemaphoreCreateMutexStatic( &WAN_xMutexBuffer );
    
    xprintf_P(PSTR("Starting tkWan..\r\n" ));
    vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );
    
    wan_state = WAN_APAGADO;

    f_inicio = true;
    
	// loop
	for( ;; )
	{       
        switch(wan_state) {
            case WAN_APAGADO:
                wan_state_apagado();
                break;
            case WAN_OFFLINE:
                wan_state_offline();
                break;
            case WAN_LINK:
                wan_state_link();
                break;
            case WAN_ONLINE_DATA:
                wan_state_online_data();
                break;
            default:
                xprintf_P(PSTR("ERROR: WAN STATE ??\r\n"));
                wan_state = WAN_APAGADO;
                break;
        }
        vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
	}    
}
//------------------------------------------------------------------------------
static void wan_state_apagado(void)
{
    /*
     * Apago el modem y espero.
     * Al salir siempre prendo el modem.
     */
    
uint32_t waiting_secs;

//    uxHighWaterMark = SPYuxTaskGetStackHighWaterMark( NULL );
//    xprintf_P(PSTR("STACK apagado_in = %d\r\n"), uxHighWaterMark );
    
    //u_kick_wdt(TK_WAN);
    
    xprintf_P(PSTR("WAN:: State APAGADO\r\n"));
    // Siempre al entrar debo apagar el modem.
    wan_apagar_modem();
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
    // Cuando inicio me conecto siempre sin esperar para configurarme y vaciar la memoria.
    if ( f_inicio ) {
        f_inicio = false;
        goto exit;
    }
       
    // Veo cuanto esperar con el modem apagado.
    waiting_secs = 30;
    vTaskDelay( ( TickType_t)( waiting_secs * 1000 / portTICK_PERIOD_MS ) );

exit:
    
    // Salimos prendiendo el modem 
    wan_prender_modem();
    wan_state = WAN_OFFLINE;
    
//    uxHighWaterMark = SPYuxTaskGetStackHighWaterMark( NULL );
//    xprintf_P(PSTR("STACK apagado_out = %d\r\n"), uxHighWaterMark );

    return;
  
}
//------------------------------------------------------------------------------
static void wan_state_offline(void)
{
    /*
     * Espera que exista link.
     * Para esto envia cada 10s un PING hacia el servidor.
     * Cuando recibe respuesta (PONG), pasa a la fase de enviar
     * los frames de re-configuracion.
     * Las respuestas del server las maneja el callback de respuestas.
     * Este prende flags para indicar al resto que tipo de respuesta llego.
     */
 
uint8_t i;
bool atmode = false;
bool save_dlg_config = false;

    //u_kick_wdt(TK_WAN);
    
    xprintf_P(PSTR("WAN:: State OFFLINE\r\n"));
    
    // Entro en modo AT
    for (i=0; i<3; i++) {
        vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
        xprintf_P(PSTR("WAN:: State OFFLINE modem enter AT (%d)\r\n"),i);
        atmode = MODEM_enter_mode_at(false);
        if (atmode)
            break;
    }
    
    // Si en 3 intentos no entre en modo AT, apago.
    if ( ! atmode ) {
        wan_state = WAN_APAGADO;
        goto quit;
    }
    
    // Estoy en modo AT
    save_dlg_config = modem_check_and_reconfig(true);  
    if ( save_dlg_config ) {
        u_save_config_in_NVM();
    }
    vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
            
    // Leo el CSQ, IMEI, ICCID:
    modem_atcmd_read_iccid(true);
    modem_atcmd_read_imei(true);
    modem_atcmd_read_csq(true);
    
    MODEM_exit_mode_at(false);
          
    wan_state = WAN_LINK;
    
quit:
   
    return;
}
//------------------------------------------------------------------------------
static void wan_state_link(void)
{
    /*
     * Espera que exista link.
     * Para esto envia cada 10s un PING hacia el servidor.
     * Cuando recibe respuesta (PONG), pasa a la fase de enviar
     * los frames de re-configuracion.
     * Las respuestas del server las maneja el callback de respuestas.
     * Este prende flags para indicar al resto que tipo de respuesta llego.
     */
    
    //u_kick_wdt(TK_WAN);
    
    xprintf_P(PSTR("WAN:: State LINK\r\n"));
        
    if ( ! wan_process_frame_ping() ) {
        wan_state = WAN_APAGADO;
        goto quit;
    }
      
    wan_state = WAN_ONLINE_DATA;
    
quit:
         
    return;
}
//------------------------------------------------------------------------------
static void wan_state_online_data(void)
{
    /*
     * Este estado es cuando estamos en condiciones de enviar frames de datos
     * y procesar respuestas.
     * Al entrar, si hay datos en memoria los transmito todos.
     * Luego, si estoy en modo continuo, no hago nada: solo espero.
     * Si estoy en modo discreto, salgo
     */

bool res;

    //u_kick_wdt(TK_WAN);

    xprintf_P(PSTR("WAN:: State ONLINE_DATA\r\n"));
    
    link_up4data = true;
   
// ENTRY:
       
    // En modo continuo me quedo esperando por datos para transmitir. 
    while( 1 ) {
                      
        if ( drWanBuffer.dr_ready ) {
            // Hay datos para transmitir
            res =  wan_process_frame_data( &drWanBuffer.dr);
            if (res) {
                drWanBuffer.dr_ready = false;
            } else {
                // Cayo el enlace ???
                wan_state = WAN_LINK;
                drWanBuffer.dr_ready = false;
                vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );
                goto quit;
            }
        }
        
        //xprintf_P(PSTR("WAN:: Loop\r\n"));
        
        // Espero que hayan mas datos
        // Vuelvo a chequear el enlace cada 1 min( tickeless & wdg ).
        //u_kick_wdt(TK_WAN);
        ulTaskNotifyTake( pdTRUE, ( TickType_t)( (60000) / portTICK_PERIOD_MS) );
    }
   
quit:
    
    link_up4data = false;
    return;        
}
//------------------------------------------------------------------------------
static bool wan_process_frame_ping(void)
{
    /*
     * Envia los PING para determinar si el enlace está establecido
     * Intento durante 2 minutos mandando un ping cada 10s.
     */
    
uint16_t tryes;
uint16_t timeout;
bool retS = false;

    xprintf_P(PSTR("WAN:: LINK.\r\n"));
 
    // Armo el frame
    while ( xSemaphoreTake( sem_WAN, MSTOTAKEWANSEMPH ) != pdTRUE )
        vTaskDelay( ( TickType_t)( 1 ) );
    
    memset(wan_tx_buffer, '\0', WAN_TX_BUFFER_SIZE);
    sprintf_P( wan_tx_buffer, PSTR("ID=%s&TYPE=%s&VER=%s&CLASS=PING"), systemConf.dlgid, FW_TYPE, FW_REV );
        
    // Proceso
    tryes = PING_TRYES;
    while (tryes-- > 0) {
        
        // Envio el frame
        wan_xmit_out();
        // Espero respuesta chequeando cada 1s durante 15s.
        timeout = 15;
        while ( timeout-- > 0) {
            vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
            if ( wan_check_response("</html>") ) {
                wan_print_RXbuffer();
                if ( wan_check_response("CLASS=PONG")) {        
                    wan_process_rsp_ping();  
                    retS = true;
                    goto exit_;
                }
            }
        }
    }
    
    // Expiro el tiempo sin respuesta del server.
    xprintf_P(PSTR("WAN:: Link up timeout !!\r\n"));
    retS = false;
    
exit_:
               
    xSemaphoreGive( sem_WAN );
    return(retS);
 
}
//------------------------------------------------------------------------------
static bool wan_process_rsp_ping(void)
{
    xprintf_P(PSTR("WAN:: Link up.\r\n"));
    return(true);
}
//------------------------------------------------------------------------------
static bool wan_process_frame_data(dataRcd_s *dr)
{
   /*
    * Armo el wan_tx_buffer.
    * Transmite los datos pasados en la estructura dataRcd por el
    * puerto definido como WAN
    */ 

uint8_t tryes = 0;
uint8_t timeout = 0;
bool retS = false;
int16_t fptr;

    xprintf_P(PSTR("WAN:: DATA.\r\n"));

    while ( xSemaphoreTake( sem_WAN, MSTOTAKEWANSEMPH ) != pdTRUE )
        vTaskDelay( ( TickType_t)( 1 ) );
    
    // Formateo(escribo) el dr en el wan_tx_buffer    
    memset(wan_tx_buffer, '\0', WAN_TX_BUFFER_SIZE);
    //sprintf_P( wan_tx_buffer, PSTR("ID=%s&TYPE=%s&VER=%s&CLASS=DATA"), systemConf.dlgid, FW_TYPE, FW_REV); 
    fptr = 0;
    fptr = sprintf_P( &wan_tx_buffer[fptr], PSTR("ID=%s&TYPE=%s&VER=%s&CLASS=DATA"), systemConf.dlgid, FW_TYPE, FW_REV);   
    // Clock
    fptr += sprintf_P( &wan_tx_buffer[fptr], PSTR("&DATE=%02d%02d%02d"), dr->rtc.year,dr->rtc.month, dr->rtc.day );
    fptr += sprintf_P( &wan_tx_buffer[fptr], PSTR("&TIME=%02d%02d%02d"), dr->rtc.hour,dr->rtc.min, dr->rtc.sec);
    
    // Medidas de CLORO:
    fptr += sprintf_P( &wan_tx_buffer[fptr], PSTR("&S0=%d&S100=%d"), dr->S0, dr->S100);
    fptr += sprintf_P( &wan_tx_buffer[fptr], PSTR("&ABS=%0.3f&CLPPM=%0.3f"), dr->absorbancia, dr->cloro_ppm);

    // Proceso. Envio hasta 2 veces el frame y espero hasta 10s la respuesta
    tryes = DATA_TRYES;
    while (tryes-- > 0) {
        
        wan_xmit_out();
        // Espero respuesta chequeando cada 1s durante 10s.
        timeout = 15;
        while ( timeout-- > 0) {
            vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
            if ( wan_check_response("</html>") ) {
                wan_print_RXbuffer();  
                if ( wan_check_response("CLASS=DATA")) {          
                    wan_process_rsp_data(); 
                    retS = true;
                    goto exit_;
                }
            }
        }  
        
        xprintf_P(PSTR("WAN:: DATA RETRY\r\n"));       
    }
 
    // Expiro el tiempo sin respuesta del server.
    xprintf_P(PSTR("WAN:: DATA ERROR: Timeout en server rsp.!!\r\n"));
    retS = false;
    
exit_:

    xSemaphoreGive( sem_WAN );
    return(retS);
    
}
//------------------------------------------------------------------------------
static bool wan_process_rsp_data(void)
{
   /*
     * Procesa las respuestas a los frames DATA
     * Podemos recibir CLOCK, RESET, PILOTO
     * <html>CLASS=DATA&CLOCK=2411050900;</html>
     */
    
char *ts = NULL;
char *stringp = NULL;
char *token = NULL;
char *delim = "&,;:=><";
char *p;

    p = MODEM_get_buffer_ptr();
    //p = lBchar_get_buffer(&wan_rx_lbuffer);

    vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
    //xprintf_P(PSTR("WAN:: response\r\n") );
    
    if ( strstr( p, "CLOCK") != NULL ) {
        memset(tmpLocalStr,'\0',sizeof(tmpLocalStr));
        ts = strstr( p, "CLOCK=");
        strncpy(tmpLocalStr, ts, sizeof(tmpLocalStr));
        stringp = tmpLocalStr;
        token = strsep(&stringp,delim);			// CLOCK
        token = strsep(&stringp,delim);			// 1910120345

        // Error en el string recibido
        if ( strlen(token) < 10 ) {
            // Hay un error en el string que tiene la fecha.
            // No lo reconfiguro
            xprintf_P(PSTR("WAN:: data_resync_clock ERROR:[%s]\r\n"), token );
            return(false);
        } else {
            u_data_resync_clock( token, false );
        }
    }
       
    if ( strstr( p, "RESET") != NULL ) {
        xprintf_P(PSTR("WAN:: RESET order from Server !!\r\n"));
        vTaskDelay( ( TickType_t)( 2000 / portTICK_PERIOD_MS ) );
        reset();
    }
    
    if ( strstr( p, "MEDIR") != NULL ) {
        xprintf_P(PSTR("WAN:: MEDIR order from Server !!\r\n"));
        vTaskDelay( ( TickType_t)( 2000 / portTICK_PERIOD_MS ) );
        
    }
    
    return(true);  
}
//------------------------------------------------------------------------------
static void wan_xmit_out(void )
{
    /* 
     * Transmite el buffer de tx por el puerto comms configurado para wan.
     * 2024-08-13: Hay un problema con el modem que si mando mas de 150 caracters
     * los recorta !!.
     * Por ahora solo hago un control.
     */
    
    //if ( strlen(wan_tx_buffer) > 150 ) {
    //    xprintf_P( PSTR("ALERT WAN BUFFER SIZE > 150 !!!\r\n"));
    //}
    // Antes de trasmitir siempre borramos el Rxbuffer
    //lBchar_Flush(&wan_rx_lbuffer);
    MODEM_flush_rx_buffer();
    //xfprintf_P( fdWAN, PSTR("%s"), wan_tx_buffer); 
    MODEM_txmit(&wan_tx_buffer[0]);
    
    //if (f_debug_comms ) {
        xfprintf_P( fdTERM, PSTR("Xmit-> %s\r\n"), &wan_tx_buffer);
    //}
    
}
//------------------------------------------------------------------------------
static bool wan_check_response ( const char *s)
{
    /*
     * Retorna true/false si encuentra el patron *s en el buffer de rx wan.
     */
    
char *p;
    
    p = MODEM_get_buffer_ptr();
    //p = lBchar_get_buffer(&wan_rx_lbuffer);
        
    if ( strstr( p, s) != NULL ) {
        if (f_debug_comms) {
            xprintf_P( PSTR("rxbuff-> %s\r\n"), p);
        }
        return(true);
    }
    return (false);
}
//------------------------------------------------------------------------------
static void wan_print_RXbuffer(void)
{

char *p;
            
    p = MODEM_get_buffer_ptr();
    //p = lBchar_get_buffer(&wan_rx_lbuffer);
    //if (f_debug_comms) {
        xprintf_P(PSTR("Rcvd-> %s\r\n"), p );
    //}
}
//------------------------------------------------------------------------------
void wan_prender_modem(void)
{
    /*
     * Se prende el modem y se ve su configuracion. Si no es correcta
     * se intenta reconfigurarlo
     * Solo se hacen 3 intentos
     * 
     */
    xprintf_P(PSTR("WAN:: PRENDER MODEM\r\n"));
    
    MODEM_prender();
    
    // Espero que se estabilize
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    return;
             
}
//------------------------------------------------------------------------------
void wan_apagar_modem(void)
{
    MODEM_apagar();
    
    xprintf_P(PSTR("WAN:: APAGAR MODEM\r\n"));
}
//------------------------------------------------------------------------------
//
// FUNCIONES PUBLICAS
//
//------------------------------------------------------------------------------
bool WAN_process_data_rcd( dataRcd_s *dataRcd)
{
    /*
     * El procesamiento implica transmitir si estoy con el link up, o almacenarlo
     * en memoria para luego transmitirlo.
     * Copio el registro a uno local y prendo la flag que indica que hay datos 
     * para transmitir.
     * Si hay un dato aun en el buffer, lo sobreescribo !!!
     * 
     */
   
bool retS = false;

    // link up
    if ( link_up4data  ) {
        /*
         * Hay enlace. 
         * Guardo el dato en el buffer.
         * Indico que hay un dato listo para enviar
         * Aviso (despierto) para que se transmita.
         */
        if (f_debug_comms) {
            xprintf_P(PSTR("WAN:: New dataframe.\r\n"));
        }
        
        memcpy( &drWanBuffer.dr, dataRcd, sizeof(dataRcd_s));
        drWanBuffer.dr_ready = true;    
        // Aviso al estado online que hay un frame listo
        while ( xTaskNotify(xHandle_tkWan, SIGNAL_FRAME_READY , eSetBits ) != pdPASS ) {
			vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );
		}
        retS = true;
        
    } else {

        xprintf_P(PSTR("WAN:: TX Fail. Link Down.\r\n"));
   
    }

    return(retS);
}
//------------------------------------------------------------------------------
void WAN_print_configuration(void)
{
    
    xprintf_P(PSTR("Comms:\r\n"));
    xprintf_P(PSTR(" debug: "));
    f_debug_comms ? xprintf_P(PSTR("on\r\n")) : xprintf_P(PSTR("off\r\n"));
    
}
//------------------------------------------------------------------------------
void WAN_config_debug(bool debug )
{
    if ( debug ) {
        f_debug_comms = true;
    } else {
        f_debug_comms = false;
    }
}
//------------------------------------------------------------------------------
bool WAN_read_debug(void)
{
    return (f_debug_comms);
}
//------------------------------------------------------------------------------
