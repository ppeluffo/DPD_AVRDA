
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

static bool test_modem(void);
static bool test_kill(void);
static bool test_display(void);

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
        xprintf_P( PSTR("  dlgid\r\n"));
        xprintf_P( PSTR("  timerpoll, timermedida\r\n"));
        xprintf_P( PSTR("  default, save, load\r\n"));
        xprintf_P( PSTR("  pump {0,1,2} {freq}\r\n"));
        xprintf_P( PSTR("  XCAL {0,9}, YCAL {0..9} \r\n"));
        xprintf_P( PSTR("  CAL {i,abs,cl} \r\n"));
        xprintf_P( PSTR("  calidate {yymmdd}\r\n"));
        xprintf_P( PSTR("  S0, S100 \r\n"));
        xprintf_P( PSTR("  dlgid\r\n"));
        xprintf_P( PSTR("  timerpolll\r\n"));
        xprintf_P( PSTR("  debug {wan} {true/false}\r\n"));
        xprintf_P( PSTR("  modem {apn|ip|port}\r\n"));
        
    	// HELP RESET
	} else if (!strcmp_P( strupr(argv[1]), PSTR("RESET"))) {
		xprintf_P( PSTR("-reset\r\n"));
        xprintf_P( PSTR("  memory {soft|hard}\r\n"));
		return;
        
    } else if (!strcmp_P( strupr(argv[1]), PSTR("TEST"))) {
		xprintf_P( PSTR("-test\r\n"));
        xprintf_P( PSTR("  display {ping|clear|clrbuff|read|brillo N|send row col data}\r\n"));
        xprintf_P( PSTR("  modem {prender|apagar|atmode|exitat|queryall|ids|save}\r\n"));
        xprintf_P( PSTR("  modem set [apn {apn}, apiurl {apiurl}, server {ip,port}], ftime {time_ms}]\r\n"));
        xprintf_P( PSTR("  modem write, read\r\n"));
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
        xprintf_P( PSTR("       13: llenado con reactivos\r\n"));
        xprintf_P( PSTR("       14: Lavado para calibrar\r\n"));
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
 
    // DISPLAY
    //display {ping|clear|send col row data}
    if (!strcmp_P( strupr(argv[1]), PSTR("DISPLAY"))  ) {
        if ( test_display() ) {
            pv_snprintfP_OK();
        } else {
            pv_snprintfP_ERR();
        }
        return;
    }
    
    // KILL {wan}
    if (!strcmp_P( strupr(argv[1]), PSTR("KILL"))  ) {
        if ( test_kill() ) {
            pv_snprintfP_OK();
        } else {
            pv_snprintfP_ERR();
        }
        return;
    }
        
    // MODEM
    // modem {prender|apagar|atmode|exitat}
     if (!strcmp_P( strupr(argv[1]), PSTR("MODEM"))  ) {
        if ( test_modem() ) {
            pv_snprintfP_OK();
        } else {
            pv_snprintfP_ERR();
        }
        return;
    }
    
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
      
    xprintf_P(PSTR("Date: %s\r\n"), RTC_logprint(FORMAT_LONG));
    xprintf_P(PSTR(" dlgid: %s\r\n"), systemConf.dlgid );
    xprintf_P(PSTR(" timerpoll=%d\r\n"), systemConf.timerpoll);
    xprintf_P(PSTR(" timermedida=%lu/(%lu)\r\n"), systemConf.timermedida, systemVars.time2medida);
    if ( systemVars.midiendo) {
        xprintf_P(PSTR(" status=MIDIENDO\r\n"));
    } else {
        xprintf_P(PSTR(" status=STANDBY\r\n"));
    }
    
    if ( WAN_is_online()) {
        xprintf_P(PSTR(" wan=ONLINE\r\n"));
    } else {
        xprintf_P(PSTR(" wan=offLINE\r\n"));
    }
    xprintf_P(PSTR(" tkRunning=0x%02X\r\n"), task_running);
    xprintf_P(PSTR(" watchdows=0x%02X\r\n"), sys_watchdog);
    
    xprintf_P(PSTR("Calibracion (i,x,y):\r\n"));
    xprintf_P(PSTR(" 0:[%0.3f,%0.3f]\r\n"), systemConf.xCal[0], systemConf.yCal[0]);
    xprintf_P(PSTR(" 1:[%0.3f,%0.3f]\r\n"), systemConf.xCal[1], systemConf.yCal[1]);
    for(i=2; i<CAL_MAX_POINTS; i++) {
        if ( systemConf.yCal[i] > 0 ) {
            xprintf_P(PSTR(" %d:[%0.3f,%0.3f]\r\n"), i, systemConf.xCal[i], systemConf.yCal[i]);
        }
    }
    xprintf_P(PSTR(" cali_date=%s\r\n"), &systemConf.calibration_date[0]);
    //xprintf_P(PSTR("\r\n"));
    xprintf_P(PSTR("Medidas:\r\n"));
    xprintf_P(PSTR(" S0=%d\r\n"),systemConf.S0);
    xprintf_P(PSTR(" S100=%d\r\n"),systemConf.S100);
    xprintf_P(PSTR(" absorbancia=%0.3f\r\n"), systemVars.absorbancia);
    xprintf_P(PSTR(" cloro_ppm=%0.2f\r\n"), systemVars.cloro_ppm);
    xprintf_P(PSTR(" timestamp=%s:%s\r\n"), &systemVars.ts_date[0], &systemVars.ts_time[0]);

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

    // DLGID
	if (!strcmp_P( strupr(argv[1]), PSTR("DLGID"))) {
        memset(systemConf.dlgid,'\0', sizeof(systemConf.dlgid) );
        memcpy(systemConf.dlgid, argv[2], sizeof(systemConf.dlgid));
        systemConf.dlgid[DLGID_LENGTH - 1] = '\0';
        pv_snprintfP_OK();
		return;
    }
    
    // CALIDATE yymmdd
    if (!strcmp_P( strupr(argv[1]), PSTR("CALIDATE"))) {
        memset(systemConf.calibration_date,'\0', sizeof(systemConf.calibration_date) );
        memcpy(systemConf.calibration_date, argv[2], sizeof(systemConf.calibration_date));
        systemConf.calibration_date[TIMESTAMP_SIZE - 1] = '\0';
        pv_snprintfP_OK();
		return;
    }
    
    // TIMERPOLL
    // config timerpoll val
	if (!strcmp_P( strupr(argv[1]), PSTR("TIMERPOLL")) ) {
        
        systemConf.timerpoll = atoi(argv[2]);

        if ( systemConf.timerpoll < 15 )
            systemConf.timerpoll = 15;

        if ( systemConf.timerpoll > 3600 )
            systemConf.timerpoll = 3600;
            
        pv_snprintfP_OK();
		return;
	}

    // TIMERMEDIDA
    // config timermedida val
	if (!strcmp_P( strupr(argv[1]), PSTR("TIMERMEDIDA")) ) {
        
        systemConf.timermedida = atol(argv[2]);

        if ( systemConf.timerpoll < 900 )
            systemConf.timerpoll = 900;

        if ( systemConf.timerpoll > 43200 )
            systemConf.timerpoll = 43200;
            
        pv_snprintfP_OK();
		return;
	}

    if (!strcmp_P( strupr(argv[1]), PSTR("S0")) ) {

        systemConf.S0 = atof(argv[2]);
        pv_snprintfP_OK();
		return;
        
    }
    
    if (!strcmp_P( strupr(argv[1]), PSTR("S100")) ) {

        systemConf.S100 = atof(argv[2]);
        pv_snprintfP_OK();
		return;
        
    }
    // CAL {i,abs,cl}
    if (!strcmp_P( strupr(argv[1]), PSTR("CAL"))) {  
        if ( atoi(argv[2]) < CAL_MAX_POINTS ) {
            systemConf.xCal[atoi(argv[2])] = atof(argv[3]);
            systemConf.yCal[atoi(argv[2])] = atof(argv[4]);
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
    
    if (!strcmp_P( strupr(argv[1]), PSTR("DEBUG")) ) {
        u_config_debug( argv[2], argv[3]) ? pv_snprintfP_OK() : pv_snprintfP_ERR();
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
     * Función invocada desde tkCMD para probar prender/apagar el opto
     */
    
    if ( !strcmp_P( strupr(s_action), PSTR("ON"))) {
        //action_opto_on(true);
        opto_on(true);
        return(true);
    }
    
    if ( !strcmp_P( strupr(s_action), PSTR("OFF"))) {
        //action_opto_off(true);
        opto_off(true);
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
            valve_0_open(true);
            ret = true;
            goto quit;
            break;
        case 1:
            valve_1_open(true);
            ret = true;
            goto quit;
            break;
        case 2:
            valve_2_open(true);
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
            valve_0_close(true);
            ret = true;
            goto quit;            
            break;
        case 1:
            valve_1_close(true);
            ret = true;
            goto quit; 
            break;
        case 2:
            valve_2_close(true);
            ret = true;
            goto quit; 
            break; 
        default:
            return(false);
        }
    }

    return(false);
    
quit:
            
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
                pump_0_run(true, atoi(s_secs) );
                ret = true;
                goto quit;
            }

            if (!strcmp_P( strupr(s_action), PSTR("STOP")) ) {
                pump_0_stop(true);
                ret = true;
                goto quit;
            }     
            
            return(false);
            break;
            
        case 1:
            if (!strcmp_P( strupr(s_action), PSTR("RUN")) ) {
                pump_1_run(true, atoi(s_secs) );
                ret = true;
                goto quit;
            }

            if (!strcmp_P( strupr(s_action), PSTR("STOP")) ) {
                pump_1_stop(true);
                ret = true;
                goto quit;
            }    
         
            return(false);
            break;
            
        case 2:
            if (!strcmp_P( strupr(s_action), PSTR("RUN")) ) {
                pump_2_run(true, atoi(s_secs) );
                ret = true;
                goto quit;
            }

            if (!strcmp_P( strupr(s_action), PSTR("STOP")) ) {
                pump_2_stop(true);
                ret = true;
                goto quit;
            }    

            return(false);
            break;
     }

quit:
            
    return(ret);
     
}
//------------------------------------------------------------------------------
bool cmd_test_adc( char *s_counts )
{
    //action_adc_read(true, atoi(s_counts));
    adc_read(true, atoi(s_counts));
    
    vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
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
   xprintf_P(PSTR("Cloro=%0.3f\r\n"), cloro);
   return (true);
    
}
//------------------------------------------------------------------------------
bool cmd_test_procedimientos( char *s_pid )
{
    switch(atoi(s_pid)) {
        case 0:
            action_inicio_sistema(true);
            break;
        case 1:
            action_lavado_reservorio_de_muestra(true);
            break;
        case 2:
            action_llenado_reservorio_con_muestra_a_medir(true);
            break;
        case 3:
            action_purga_canal_muestra(true);
            break;
        case 4:
            action_lavado_celda(true);
            break;
        case 5:
            action_ajustes_fotometricos(true);
            break;
        case 6:
            action_medicion(true);
            break;
        case 7:
            action_lavado_final(true);
            break;
        case 8:
            action_fin_sistema(true);
            break;
        case 9:
            action_llenar_celda_medida(true);
            break;
        case 10:
            action_vaciar_celda_medida(true);
            break;
        case 11:
            action_calibrar(true);
            break;
        case 12:
            action_medida_completa(true);
            break;
        case 13:
            action_llenado_con_reactivos(true);
            break;
        case 14:
            action_lavado_calibracion(true);
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
        systemConf.xCal[i] = atof(argv[2+i]);
    }
}
//------------------------------------------------------------------------------
void ycal_config(void)
{
uint8_t i;

    for (i=0; i<CAL_MAX_POINTS; i++) {
        systemConf.yCal[i] = atof(argv[2+i]);
    }
}
//------------------------------------------------------------------------------
static bool test_modem(void)
{
    
bool retS = false;
char *p;


    if (!strcmp_P( strupr(argv[2]), PSTR("WRITE"))  ) {
        vTaskDelay( ( TickType_t)( 5 ) );   
        xfprintf_P( fdWAN, PSTR("The quick brown fox jumps over the lazy dog \r\n"));
        vTaskDelay( ( TickType_t)( 2 ) );
        retS = true;
        goto exit;  
    }

    if (!strcmp_P( strupr(argv[2]), PSTR("READ"))  ) {
        xprintf_P(PSTR("SIZE=%d\r\n"), lBchar_GetCount(&modem_rx_lbuffer));
        p = MODEM_get_buffer_ptr(); 
        xprintf_P(PSTR("ModemRx-> %s\r\n"), p );
        retS = true;
        goto exit;  
    }

    if (!strcmp_P( strupr(argv[2]), PSTR("CLEAR"))  ) {
        MODEM_flush_rx_buffer();
        retS = true;
        goto exit; 
    }

    if (!strcmp_P( strupr(argv[2]), PSTR("APN"))  ) {
        modem_atcmd_read_apn(true);
        retS = true;
        goto exit;  
    }

    if (!strcmp_P( strupr(argv[2]), PSTR("SERVER"))  ) {
        modem_atcmd_read_server(true);
        retS = true;
        goto exit;  
    }

    if (!strcmp_P( strupr(argv[2]), PSTR("IDS"))  ) {
        modem_atcmd_read_imei(true);
        modem_atcmd_read_iccid(true);
        modem_atcmd_read_csq(true);
        retS=true;
        goto exit;
    }

    if (!strcmp_P( strupr(argv[2]), PSTR("PRENDER"))  ) {
        MODEM_prender();
        retS=true;
        goto exit;
    }
        
    if (!strcmp_P( strupr(argv[2]), PSTR("APAGAR"))  ) {
        MODEM_apagar();
        retS=true;
        goto exit;
    }
        
    if (!strcmp_P( strupr(argv[2]), PSTR("ATMODE"))  ) {
        MODEM_enter_mode_at(true);
        retS=true;
        goto exit;
    }

    if (!strcmp_P( strupr(argv[2]), PSTR("EXITAT"))  ) {
        MODEM_exit_mode_at(true);
        retS=true;
        goto exit;
    }

    if (!strcmp_P( strupr(argv[2]), PSTR("SAVE"))  ) {
        modem_atcmd_save(true);
        retS=true;
        goto exit;
    }

    if (!strcmp_P( strupr(argv[2]), PSTR("QUERYALL"))  ) {
        modem_atcmd_queryall();
        retS=true;
        goto exit;
    }

    // SET
    if (!strcmp_P( strupr(argv[2]), PSTR("SET"))  ) {
              
        if (!strcmp_P( strupr(argv[3]), PSTR("APN"))  ) {
            modem_atcmd_set_apn(argv[4], true);
            retS=true;
            goto exit;
        }
        
        if (!strcmp_P( strupr(argv[3]), PSTR("SERVER"))  ) {
            modem_atcmd_set_server(argv[4], argv[5], true);
            retS=true;
            goto exit;
        }
        
        if (!strcmp_P( strupr(argv[3]), PSTR("APIURL"))  ) {
            modem_atcmd_set_apiurl(argv[4], true);
            retS=true;
            goto exit;
        }

        if (!strcmp_P( strupr(argv[3]), PSTR("FTIME"))  ) {
            modem_atcmd_set_ftime(argv[4], true);
            retS=true;
            goto exit;
        }
    }
    
exit:
            
    return (retS);

}
//------------------------------------------------------------------------------
static bool test_kill(void)
{
    if (!strcmp_P( strupr(argv[2]), PSTR("WAN"))  ) {
            
        if ( xHandle_tkWan != NULL ) {
            vTaskSuspend( xHandle_tkWan );
            SYSTEM_ENTER_CRITICAL();
            task_running &= ~WAN_WDG_gc;
            //tk_running[TK_WAN] = false;
            SYSTEM_EXIT_CRITICAL();
            xHandle_tkWan = NULL;
            return(true);
        }  
        return(false);
    }

    if (!strcmp_P( strupr(argv[2]), PSTR("SYS"))  ) {
            
        if ( xHandle_tkSys != NULL ) {
            vTaskSuspend( xHandle_tkSys );
            SYSTEM_ENTER_CRITICAL();
            task_running &= ~SYS_WDG_gc;
            //tk_running[TK_WAN] = false;
            SYSTEM_EXIT_CRITICAL();
            xHandle_tkSys = NULL;
            return(true);
        }  
        return(false);
    }
    
    return(false);
}
//------------------------------------------------------------------------------
static bool test_display(void)
{
bool retS = false;
uint8_t row,col;


    if (!strcmp_P( strupr(argv[2]), PSTR("BRILLO"))  ) {
        lcd_cmd_brightness(atoi(argv[3]));
        retS=true;
        goto exit;
    }

    if (!strcmp_P( strupr(argv[2]), PSTR("CLRBUF"))  ) {
        lcd_cmd_clearbuff();
        retS=true;
        goto exit;
    } 
    
    if (!strcmp_P( strupr(argv[2]), PSTR("READ"))  ) {
        lcd_cmd_read();
        retS=true;
        goto exit;
    }   

    if (!strcmp_P( strupr(argv[2]), PSTR("PING"))  ) {
        lcd_cmd_ping();
        retS=true;
        goto exit;
    }

    if (!strcmp_P( strupr(argv[2]), PSTR("CLEAR"))  ) {
        lcd_cmd_clear();
        retS=true;
        goto exit;
    }

    if (!strcmp_P( strupr(argv[2]), PSTR("SEND"))  ) {
        row = atoi(argv[3]);
        col = atoi(argv[4]);
        lcd_cmd_send_data(row, col, argv[5]);
        retS=true;
        goto exit;
    }

exit:
    
    return(retS);

}
//------------------------------------------------------------------------------