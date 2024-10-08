
#include "DPD_AVRDA.h"
#include "frtos_cmd.h"

static void cmdClsFunction(void);
static void cmdHelpFunction(void);
static void cmdResetFunction(void);
static void cmdStatusFunction(void);
static void cmdWriteFunction(void);
static void cmdReadFunction(void);
static void cmdConfigFunction(void);
static void cmdTestFunction(void);

static void pv_snprintfP_OK(void );
static void pv_snprintfP_ERR(void );

bool cmd_test_opto(char *s_action);
bool cmd_test_valve( char *valve_id, char *s_action);
bool cmd_test_pump( char *pump_id, char *s_action, char *s_secs);
bool cmd_test_adc( char *s_counts );
bool cmd_test_procedimientos( char *s_pid );
bool cmd_test_cloro( char *s_abs);

void xcal_config(void);
void ycal_config(void);

//------------------------------------------------------------------------------
void tkCmd(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;

    while ( ! starting_flag )
        vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );

    SYSTEM_ENTER_CRITICAL();
    task_running |= CMD_WDG_gc;
    SYSTEM_EXIT_CRITICAL();
             
	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );

uint8_t c = 0;

    FRTOS_CMD_init();

    FRTOS_CMD_register( "cls", cmdClsFunction );
	FRTOS_CMD_register( "help", cmdHelpFunction );
    FRTOS_CMD_register( "reset", cmdResetFunction );
    FRTOS_CMD_register( "status", cmdStatusFunction );
    FRTOS_CMD_register( "write", cmdWriteFunction );
    FRTOS_CMD_register( "read", cmdReadFunction );
    FRTOS_CMD_register( "config", cmdConfigFunction );
    FRTOS_CMD_register( "test", cmdTestFunction );
    
    xprintf_P(PSTR("Starting tkCmd..\r\n" ));
    xprintf_P(PSTR("Spymovil %s %s %s %s \r\n") , HW_MODELO, FRTOS_VERSION, FW_REV, FW_DATE);
      
	// loop
	for( ;; )
	{
        u_kick_wdt(CMD_WDG_gc);
         
		c = '\0';	// Lo borro para que luego del un CR no resetee siempre el timer.
		// el read se bloquea 10ms. lo que genera la espera.
		//while ( frtos_read( fdTERM, (char *)&c, 1 ) == 1 ) {
        while ( xgetc( (char *)&c ) == 1 ) {
            FRTOS_CMD_process(c);
        }
        
        // Espero 10ms si no hay caracteres en el buffer
        vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
               
	}    
}
//------------------------------------------------------------------------------
static void cmdHelpFunction(void)
{

    FRTOS_CMD_makeArgv();
        
    if ( !strcmp_P( strupr(argv[1]), PSTR("WRITE"))) {
		xprintf_P( PSTR("-write:\r\n"));
        xprintf_P( PSTR("  (ee,nvmee,rtcram) {pos string} {debug}\r\n"));
        xprintf_P( PSTR("  rtc YYMMDDhhmm\r\n"));
        
    }  else if ( !strcmp_P( strupr(argv[1]), PSTR("READ"))) {
		xprintf_P( PSTR("-read:\r\n"));
        xprintf_P( PSTR("  (ee,nvmee,rtcram) {pos} {lenght} {debug}\r\n"));
        xprintf_P( PSTR("  avrid,rtc {long,short}\r\n"));
        
    }  else if ( !strcmp_P( strupr(argv[1]), PSTR("CONFIG"))) {
		xprintf_P( PSTR("-config:\r\n"));
        xprintf_P( PSTR("  default, save, load\r\n"));
        xprintf_P( PSTR("  pump {0,1,2} {freq}\r\n"));
        xprintf_P( PSTR("  XCAL {0,9}, YCAL {0..9} \r\n"));
        xprintf_P( PSTR("  CAL {i,abs,cl} \r\n"));
        
    	// HELP RESET
	} else if (!strcmp_P( strupr(argv[1]), PSTR("RESET"))) {
		xprintf_P( PSTR("-reset\r\n"));
        xprintf_P( PSTR("  memory {soft|hard}\r\n"));
		return;
        
    } else if (!strcmp_P( strupr(argv[1]), PSTR("TEST"))) {
		xprintf_P( PSTR("-test\r\n"));
        xprintf_P( PSTR("  valve {0,1,2} {open|close}\r\n"));
        xprintf_P( PSTR("  cnt {read|clear|pin}\r\n"));
        xprintf_P( PSTR("  opto {on|off}\r\n"));
        xprintf_P( PSTR("  pump {0,1,2} {run|stop} {secs}\r\n"));
        xprintf_P( PSTR("  adc {counts} \r\n"));
        xprintf_P( PSTR("  kill {wan,sys}\r\n"));
        xprintf_P( PSTR("  cloro {abs}\r\n"));
        xprintf_P( PSTR("  proc 0: inicio_sistema\r\n"));
        xprintf_P( PSTR("       1: lavado_reservorio_de_muestra\r\n"));
        xprintf_P( PSTR("       2: llenado_reservorio_con_muestra_a_medir\r\n"));
        xprintf_P( PSTR("       3: purga_canal_muestra\r\n"));
        xprintf_P( PSTR("       4: lavado_celda\r\n"));
        xprintf_P( PSTR("       5: ajustes_fotometricos\r\n"));
        xprintf_P( PSTR("       6: medicion\r\n"));
        xprintf_P( PSTR("       7: lavado_final\r\n"));
        xprintf_P( PSTR("       8: fin_sistema\r\n"));
        xprintf_P( PSTR("       9: llenar_celda_medida\r\n"));
        xprintf_P( PSTR("       10: vaciar_celda_medida\r\n"));
        xprintf_P( PSTR("       11: Calibrar\r\n"));
        xprintf_P( PSTR("       12: medida_completa\r\n"));
        return;
        
    }  else {
        // HELP GENERAL
        xprintf("Available commands are:\r\n");
        xprintf("-cls\r\n");
        xprintf("-help\r\n");
        xprintf("-status\r\n");
        xprintf("-reset\r\n");
        xprintf("-config...\r\n");
        xprintf("-write...\r\n");
        xprintf("-read...\r\n");
        xprintf("-test...\r\n");
    }
   
	xprintf("Exit help \r\n");

}
//------------------------------------------------------------------------------
static void cmdTestFunction(void)
{

    FRTOS_CMD_makeArgv();
 
    // test cloro abs
    if ( !strcmp_P( strupr(argv[1]), PSTR("CLORO"))) {
        cmd_test_cloro( argv[2]) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
        return;
    }
    
    // test adc n
    if ( !strcmp_P( strupr(argv[1]), PSTR("ADC"))) {
        cmd_test_adc( argv[2]) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
        return;
    }
    
    // test valve {0,1,2} {open|close}
    if ( !strcmp_P( strupr(argv[1]), PSTR("VALVE"))) {
        cmd_test_valve( argv[2], argv[3]) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
        return;
    }

    if ( !strcmp_P( strupr(argv[1]), PSTR("CNT"))) {
        counter_test( argv[2] ) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
        return;
    }
    
    // test pump {0,1,2} run {secs}
    //                   stop
    if ( !strcmp_P( strupr(argv[1]), PSTR("PUMP"))) {
        cmd_test_pump( argv[2], argv[3], argv[4] ) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
        return;
    }
    
    if ( !strcmp_P( strupr(argv[1]), PSTR("OPTO"))) {
        cmd_test_opto( argv[2] ) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
        return;
    }
    
    if ( !strcmp_P( strupr(argv[1]), PSTR("PROC"))) {
        cmd_test_procedimientos( argv[2] ) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
        return;
    }
    
    pv_snprintfP_ERR();
    return;
       
}
//------------------------------------------------------------------------------
static void cmdReadFunction(void)
{
    
    FRTOS_CMD_makeArgv();       
    
    // EE
	// read ee address length
	if (!strcmp_P( strupr(argv[1]), PSTR("EE")) ) {
		EE_test_read ( argv[2], argv[3], argv[4] );
		return;
	}
    
    // RTC
	// read rtc { long | short } 
    if (!strcmp_P( strupr(argv[1]), PSTR("RTC")) ) {
        if (!strcmp_P( strupr(argv[2]), PSTR("LONG")) ) {
            RTC_read_time(FORMAT_LONG);
            pv_snprintfP_OK();
            return;
        }
        if (!strcmp_P( strupr(argv[2]), PSTR("SHORT")) ) {
            RTC_read_time(FORMAT_SHORT);
            pv_snprintfP_OK();
            return;
        }
        pv_snprintfP_ERR();
        return;
    }
        
    // NVMEE
	// read nvmee address length
	if (!strcmp_P( strupr(argv[1]), PSTR("NVMEE")) ) {
		NVMEE_test_read ( argv[2], argv[3] );
		return;
	}

	// AVRID
	// read avrid
	if (!strcmp_P( strupr(argv[1]), PSTR("AVRID"))) {
		//nvm_read_print_id();
        xprintf_P(PSTR("ID: %s\r\n"), NVM_id2str() );
        xprintf_P(PSTR("SIGNATURE: %s\r\n"), NVM_signature2str() );
		return;
	}
    
    
    // CMD NOT FOUND
	xprintf("ERROR\r\nCMD NOT DEFINED\r\n\0");
	return;
 
}
//------------------------------------------------------------------------------
static void cmdClsFunction(void)
{
	// ESC [ 2 J
	xprintf("\x1B[2J\0");
}
//------------------------------------------------------------------------------
static void cmdResetFunction(void)
{
    
    FRTOS_CMD_makeArgv();
  
    xprintf("Reset..\r\n");
    reset();
}
//------------------------------------------------------------------------------
static void cmdStatusFunction(void)
{

    // https://stackoverflow.com/questions/12844117/printing-defined-constants

uint8_t i;
    
    xprintf("Spymovil %s %s TYPE=%s, VER=%s %s \r\n" , HW_MODELO, FRTOS_VERSION, FW_TYPE, FW_REV, FW_DATE);
      
 //D   xprintf_P(PSTR("Date: %s\r\n"), RTC_logprint(FORMAT_LONG));
    
    xprintf_P(PSTR("Calibracion (i,x,y):\r\n"));
    for(i=0; i<CAL_MAX_POINTS; i++) {
        xprintf_P(PSTR(" %d:[%0.3f,%0.3f]\r\n"), i, systemVars.xCal[i], systemVars.yCal[i]);
    }
    //xprintf_P(PSTR("\r\n"));
    xprintf_P(PSTR("Medidas:\r\n"));
    xprintf_P(PSTR(" S0=%d\r\n"),S0);
    xprintf_P(PSTR(" S100=%d\r\n"),S100);
    xprintf_P(PSTR(" absorbancia=%0.3f\r\n"), absorbancia);
    xprintf_P(PSTR(" cloro_ppm=%0.3f\r\n"), cloro_ppm);

    xprintf_P(PSTR("Valves:\r\n"));
    valve_print_status();
    
    xprintf_P(PSTR("Contador: %lu\r\n"), counter_read() );
    
    xprintf_P(PSTR("Pumps:\r\n"));
    pump_print_status();
        
    if ( opto_get_status() ) {
        xprintf_P(PSTR("Opto Led: ON\r\n"));
    } else {
        xprintf_P(PSTR("Opto Led: OFF\r\n"));
    }
    
    //xprintf_P(PSTR(" date: %s\r\n"), RTC_logprint(FORMAT_LONG));
}
//------------------------------------------------------------------------------
static void cmdWriteFunction(void)
{

    FRTOS_CMD_makeArgv();
        
    // EE
	// write ee pos string
	if ((strcmp_P( strupr(argv[1]), PSTR("EE")) == 0) ) {
		( EE_test_write ( argv[2], argv[3], argv[4] ) > 0)?  pv_snprintfP_OK() : pv_snprintfP_ERR();
		return;
	}

    // RTC
	// write rtc YYMMDDhhmm
	if ( strcmp_P( strupr(argv[1]), PSTR("RTC")) == 0 ) {
		( RTC_write_time( argv[2]) > 0)?  pv_snprintfP_OK() : 	pv_snprintfP_ERR();
		return;
	}

    // NVMEE
	// write nvmee pos string
	if ( (strcmp_P( strupr(argv[1]), PSTR("NVMEE")) == 0)) {
		NVMEE_test_write ( argv[2], argv[3] );
		pv_snprintfP_OK();
		return;
	}
    
    // CMD NOT FOUND
	xprintf("ERROR\r\nCMD NOT DEFINED\r\n\0");
	return;
 
}
//------------------------------------------------------------------------------
static void cmdConfigFunction(void)
{
    
    FRTOS_CMD_makeArgv();

    // CAL {i,abs,cl}
    if (!strcmp_P( strupr(argv[1]), PSTR("CAL"))) {  
        if ( atoi(argv[2]) < CAL_MAX_POINTS ) {
            systemVars.xCal[atoi(argv[2])] = atof(argv[3]);
            systemVars.yCal[atoi(argv[2])] = atof(argv[4]);
            pv_snprintfP_OK();
        } else {
            pv_snprintfP_ERR();
        }
		return;
    }
    
    //  XCAL {0,9}, YCAL {0..9}
    if (!strcmp_P( strupr(argv[1]), PSTR("XCAL"))) {  
        xcal_config();
        pv_snprintfP_OK();
		return;
    }

    if (!strcmp_P( strupr(argv[1]), PSTR("YCAL"))) {  
        ycal_config();
        pv_snprintfP_OK();
		return;
    }
    
    // PUMP FREQ
    // pump {0,1,2} {freq}
    if (!strcmp_P( strupr(argv[1]), PSTR("PUMP"))) {   
        pump_config( argv[2], argv[3]);
        pv_snprintfP_OK();
		return;
    }
    
    
 	// SAVE
	// config save
	if (!strcmp_P( strupr(argv[1]), PSTR("SAVE"))) {       
		u_save_config_in_NVM();
		pv_snprintfP_OK();
		return;
	}
    
    // LOAD
	// config load
	if (!strcmp_P( strupr(argv[1]), PSTR("LOAD"))) {
		u_load_config_from_NVM();
		pv_snprintfP_OK();
		return;
	}
    
    // DEFAULT
	// config default
	if (!strcmp_P( strupr(argv[1]), PSTR("DEFAULT"))) {
		u_config_default();
		pv_snprintfP_OK();
		return;
	}
    
    // CMD NOT FOUND
	xprintf("ERROR\r\nCMD NOT DEFINED\r\n\0");
	return;
 
}
//------------------------------------------------------------------------------
static void pv_snprintfP_OK(void )
{
	xprintf("ok\r\n\0");
}
//------------------------------------------------------------------------------
static void pv_snprintfP_ERR(void)
{
	xprintf("error\r\n\0");
}
//------------------------------------------------------------------------------
bool cmd_test_opto(char *s_action)
{
    /*
     * Funci�n invocada desde tkCMD para probar prender/apagar el opto
     */
    
    if ( !strcmp_P( strupr(s_action), PSTR("ON"))) {
        action_opto_on(true);
        action_await();
        return(true);
    }
    
    if ( !strcmp_P( strupr(s_action), PSTR("OFF"))) {
        action_opto_off(true);
        action_await();
        return(true);
    }
    
    return(false);
}
//------------------------------------------------------------------------------
bool cmd_test_valve( char *valve_id, char *s_action)
{
     
uint8_t vid = atoi(valve_id);
bool ret = false;

    if (!strcmp_P( strupr(s_action), PSTR("OPEN")) ) {
        switch(vid) {
        case 0:
            action_valve_0_open(true);
            ret = true;
            goto quit;
            break;
        case 1:
            action_valve_1_open(true);
            ret = true;
            goto quit;
            break;
        case 2:
            action_valve_2_open(true);
            ret = true;
            goto quit;
            break; 
        default:
            return(false);
        }
    }

    if (!strcmp_P( strupr(s_action), PSTR("CLOSE")) ) {
        switch(vid) {
        case 0:
            action_valve_0_close(true);
            ret = true;
            goto quit;            
            break;
        case 1:
            action_valve_1_close(true);
            ret = true;
            goto quit; 
            break;
        case 2:
            action_valve_2_close(true);
            ret = true;
            goto quit; 
            break; 
        default:
            return(false);
        }
    }

    return(false);
    
quit:
            
    action_await();
    return(ret);
}
//------------------------------------------------------------------------------
bool cmd_test_pump( char *pump_id, char *s_action, char *s_secs)
{
    // pump {0,1,2} run secs
    // pump {0,1,2} stop
 
bool ret = false;

    switch(atoi(pump_id)) {
        case 0:
            if (!strcmp_P( strupr(s_action), PSTR("RUN")) ) {
                action_pump_0_run(true, atoi(s_secs) );
                ret = true;
                goto quit;
            }

            if (!strcmp_P( strupr(s_action), PSTR("STOP")) ) {
                action_pump_0_stop(true);
                ret = true;
                goto quit;
            }     
            
            return(false);
            break;
            
        case 1:
            if (!strcmp_P( strupr(s_action), PSTR("RUN")) ) {
                action_pump_1_run(true, atoi(s_secs) );
                ret = true;
                goto quit;
            }

            if (!strcmp_P( strupr(s_action), PSTR("STOP")) ) {
                action_pump_1_stop(true);
                ret = true;
                goto quit;
            }    
         
            return(false);
            break;
            
        case 2:
            if (!strcmp_P( strupr(s_action), PSTR("RUN")) ) {
                action_pump_2_run(true, atoi(s_secs) );
                ret = true;
                goto quit;
            }

            if (!strcmp_P( strupr(s_action), PSTR("STOP")) ) {
                action_pump_2_stop(true);
                ret = true;
                goto quit;
            }    

            return(false);
            break;
     }

quit:
            
    action_await();     
    return(ret);
     
}
//------------------------------------------------------------------------------
bool cmd_test_adc( char *s_counts )
{
    action_adc_read(true, atoi(s_counts));
    
    vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
    action_await();
    xprintf_P(PSTR("ADCres=%d\r\n"), adcCB.result);
    return(true);
    
} 
//------------------------------------------------------------------------------
bool cmd_test_cloro( char *s_abs)
{
    
float abs;
float cloro;
    
   abs = atof(s_abs);
   cloro = cloro_from_absorbancia( abs, true);
   return (true);
    
}
//------------------------------------------------------------------------------
bool cmd_test_procedimientos( char *s_pid )
{
    switch(atoi(s_pid)) {
        case 0:
            proc_inicio_sistema(true);
            break;
        case 1:
            proc_lavado_reservorio_de_muestra(true);
            break;
        case 2:
            proc_llenado_reservorio_con_muestra_a_medir(true);
            break;
        case 3:
            proc_purga_canal_muestra(true);
            break;
        case 4:
            proc_lavado_celda(true);
            break;
        case 5:
            proc_ajustes_fotometricos(true);
            break;
        case 6:
            proc_medicion(true);
            break;
        case 7:
            proc_lavado_final(true);
            break;
        case 8:
            proc_fin_sistema(true);
            break;
        case 9:
            proc_llenar_celda_medida(true);
            break;
        case 10:
            proc_vaciar_celda_medida(true);
            break;
        case 11:
            proc_calibrar(true);
            break;
        case 12:
            proc_medida_completa(true);
            break;
        default:
            return(false);           
    }
    return(true);
}
//------------------------------------------------------------------------------
void xcal_config(void)
{
uint8_t i;

    for (i=0; i<CAL_MAX_POINTS; i++) {
        systemVars.xCal[i] = atof(argv[2+i]);
    }
}
//------------------------------------------------------------------------------
void ycal_config(void)
{
uint8_t i;

    for (i=0; i<CAL_MAX_POINTS; i++) {
        systemVars.yCal[i] = atof(argv[2+i]);
    }
}
//------------------------------------------------------------------------------