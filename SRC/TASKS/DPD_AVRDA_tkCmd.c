
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

static void cmdValveFunction(void);
static void cmdOptoFunction(void);
static void cmdPumpFunction(void);

static void pv_snprintfP_OK(void );
static void pv_snprintfP_ERR(void );

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
    
    FRTOS_CMD_register( "valve", cmdValveFunction );
    FRTOS_CMD_register( "opto", cmdOptoFunction );
    FRTOS_CMD_register( "pump", cmdPumpFunction );
    
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
        xprintf_P( PSTR("  save, load\r\n"));
        
    	// HELP RESET
	} else if (!strcmp_P( strupr(argv[1]), PSTR("RESET"))) {
		xprintf_P( PSTR("-reset\r\n"));
        xprintf_P( PSTR("  memory {soft|hard}\r\n"));
		return;
        
    } else if (!strcmp_P( strupr(argv[1]), PSTR("TEST"))) {
		xprintf_P( PSTR("-test\r\n"));
        xprintf_P( PSTR("  valve {0,1,2} {open|close}\r\n"));
        xprintf_P( PSTR("  cnt {read|clear|pin}\r\n"));
        xprintf_P( PSTR("  stepper {0,1,2} run secs\r\n"));
        //xprintf_P( PSTR("                  run {fw|rev} secs\r\n"));
        //xprintf_P( PSTR("  stepper {0,1,2} {en|dir} {on|off}\r\n"));
        //xprintf_P( PSTR("                  run {fw|rev} secs\r\n"));
        xprintf_P( PSTR("                  stop\r\n"));
        xprintf_P( PSTR("  opto {on|off}\r\n"));
        xprintf_P( PSTR("  adc1115 {init,start,status,read,rse,mread}\r\n"));
        xprintf_P( PSTR("  kill {wan,sys}\r\n"));
        return;
       
    } else if (!strcmp_P( strupr(argv[1]), PSTR("VALVE"))) {
        xprintf_P( PSTR("-valve {0,1,2} {open|close}\r\n"));
        return;
        
    } else if (!strcmp_P( strupr(argv[1]), PSTR("OPTO"))) {
        xprintf_P( PSTR("-opto {on|off}\r\n"));
        xprintf_P( PSTR("      read N\r\n"));

        return;

    } else if (!strcmp_P( strupr(argv[1]), PSTR("PUMP"))) {
        xprintf_P( PSTR("-pump {0,1,2} run secs\r\n"));
        xprintf_P( PSTR("              stop\r\n"));
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
        xprintf("*valve...\r\n");
        xprintf("*opto...\r\n");
        xprintf("*pump...\r\n");
       
    }
   
	xprintf("Exit help \r\n");

}
//------------------------------------------------------------------------------
static void cmdValveFunction(void)
{
    // valve {0,1,2} {open|close}
 
    FRTOS_CMD_makeArgv();
    
    valve_tests( argv[1], argv[2]) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
        
}
//------------------------------------------------------------------------------
static void cmdOptoFunction(void)
{
    // opto {on|off}
    //      read N
    
   
float res;
float res_mv = 0.0;
 
    FRTOS_CMD_makeArgv();
    
    if ( !strcmp_P( strupr(argv[1]), PSTR("ON"))) {
        opto_test( argv[1] ) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
        return;
    }

    if ( !strcmp_P( strupr(argv[1]), PSTR("OFF"))) {
        opto_test( argv[1] ) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
        return;
    }
    
    if ( !strcmp_P( strupr(argv[1]), PSTR("READ"))) {
        res = ADC1115_multiple_read(atoi(argv[2]));
        xprintf_P(PSTR("RES=%0.3f\r\n"),res );
        res_mv = 0.125 * res;
        xprintf_P(PSTR("RES_mv=%.3f\r\n"),res_mv );
        pv_snprintfP_OK();
        return;
    }

    if ( !strcmp_P( strupr(argv[1]), PSTR("MVREAD"))) {
        res = ADC1115_multiple_read_mV(atoi(argv[2]));
        xprintf_P(PSTR("RES= %lu\r\n"),res );
        pv_snprintfP_OK();
        return;
    }
    
    
}
//------------------------------------------------------------------------------
static void cmdPumpFunction(void)
{
    // pump {0,1,2} run secs
    // pump {0,1,2} stop
    FRTOS_CMD_makeArgv();
    tmc2209_test( argv[1], argv[2], argv[3], argv[4] ) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
}
//------------------------------------------------------------------------------
static void cmdTestFunction(void)
{

uint16_t res;
float volts = 0.0;

    FRTOS_CMD_makeArgv();
 
    // test adc1115 {init,start,status,read}
    if ( !strcmp_P( strupr(argv[1]), PSTR("ADC1115"))) {
        
        if ( !strcmp_P( strupr(argv[2]), PSTR("INIT"))) {
            ADC1115_init();
            //ADC_setup();
            pv_snprintfP_OK();
            return;
        }
        
        if ( !strcmp_P( strupr(argv[2]), PSTR("START"))) {
            ADC1115_start_conversion();
            pv_snprintfP_OK();
            return;
        }
        
        if ( !strcmp_P( strupr(argv[2]), PSTR("STATUS"))) {
            if ( ADC1115_is_conversion_done() ) {
                xprintf_P(PSTR("conversion done\r\n"));
            } else {
                xprintf_P(PSTR("conversion NOT done\r\n"));
            }
            pv_snprintfP_OK();
            return;  
        }  
        
        if ( !strcmp_P( strupr(argv[2]), PSTR("READ"))) {
            res = ADC1115_get_conversion_result();
            //volts = ADC_readChannel(ADS1115_COMP_0_GND);
            xprintf_P(PSTR("RES=0x%04x (%u)\r\n"),res, res );
            //xprintf_P(PSTR("VOLTS=%f\r\n"),volts );
            pv_snprintfP_OK();
            return;
        }

        if ( !strcmp_P( strupr(argv[2]), PSTR("RSE"))) {
            res = ADC1115_read_single_ended();
            xprintf_P(PSTR("RES=0x%04x (%u)\r\n"),res, res );
            pv_snprintfP_OK();
            return;
        }
        
        if ( !strcmp_P( strupr(argv[2]), PSTR("MREAD"))) {
            res = ADC1115_multiple_read(atoi(argv[3]));
            xprintf_P(PSTR("RES=0x%04x (%u)\r\n"),res, res );
            pv_snprintfP_OK();
            return;
        }
        
                    
    }
    
    if ( !strcmp_P( strupr(argv[1]), PSTR("VALVE"))) {
        valve_tests( argv[2], argv[3]) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
        return;
    }

    if ( !strcmp_P( strupr(argv[1]), PSTR("CNT"))) {
        counter_test( argv[2] ) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
        return;
    }
    
    if ( !strcmp_P( strupr(argv[1]), PSTR("STEPPER"))) {
        tmc2209_test( argv[2], argv[3], argv[4], argv[5] ) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
        return;
    }
    
    if ( !strcmp_P( strupr(argv[1]), PSTR("OPTO"))) {
        opto_test( argv[2] ) ? pv_snprintfP_OK(): pv_snprintfP_ERR();
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

vstatus_t vstatus;
steppers_cb_t steppers_cb;
uint8_t i;

    xprintf("Spymovil %s %s TYPE=%s, VER=%s %s \r\n" , HW_MODELO, FRTOS_VERSION, FW_TYPE, FW_REV, FW_DATE);
      
 //D   xprintf_P(PSTR("Date: %s\r\n"), RTC_logprint(FORMAT_LONG));
    
    xprintf_P(PSTR("Valves:\r\n"));
    get_valve_status(&vstatus);
    if (vstatus.vs0 == VALVE_OPEN ) {
        xprintf_P(PSTR(" V0:OPEN\r\n"));
    } else {
        xprintf_P(PSTR(" V0:CLOSE\r\n"));
    }
    if (vstatus.vs1 == VALVE_OPEN ) {
        xprintf_P(PSTR(" V1:OPEN\r\n"));
    } else {
        xprintf_P(PSTR(" V1:CLOSE\r\n"));
    }
    if (vstatus.vs2 == VALVE_OPEN ) {
        xprintf_P(PSTR(" V2:OPEN\r\n"));
    } else {
        xprintf_P(PSTR(" V2:CLOSE\r\n"));
    }
    
    xprintf_P(PSTR("Contador: %lu\r\n"), counter_read() );
    
    get_tmc2209_status(&steppers_cb);
    xprintf_P(PSTR("Motores:\r\n"));
    for (i=0; i<3; i++) {
        xprintf_P(PSTR(" M%d: "), i);
        
        if ( steppers_cb.enabled[i] == true ) {
             xprintf_P(PSTR("(en)  "));
        } else {
             xprintf_P(PSTR("(dis) "));
        }
        
        /*
         * Los motores siempre los muevo FW !!!
        if ( steppers_cb.dir[i] == DIR_FW ) {
            xprintf_P(PSTR("(FW)  "));
        } else {
            xprintf_P(PSTR("(REV) "));
        }
         */

        if ( steppers_cb.status[i] == RUNNING ) {
            xprintf_P(PSTR("running, %lu\r\n"), steppers_cb.cnt[i]);
        } else {
            xprintf_P(PSTR("stopped\r\n"));
        }
    }
    
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
