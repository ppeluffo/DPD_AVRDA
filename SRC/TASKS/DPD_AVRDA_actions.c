
#include "DPD_AVRDA.h"
#include "math.h"
/*
 * Las funciones de 'action' son las invocadas por el resto del 
 * programa y le pasan los parámetros.
 * Cada action setea con sus parametros el correspondiente CB
 * y fija cual funcion va a ejecutrar tkSys para cumplir.
 *
 */

//------------------------------------------------------------------------------
/*
 * PROCEDIMIENTOS:
 * Conjunto de acciones basicas.
 * Como los disparo como action, no pueden usar internamente más actions
 * sino que deben usar functions.
 */
//------------------------------------------------------------------------------

char msg[16];

void await(uint16_t secs );

void action_inicio_sistema( bool debug);
void action_lavado_reservorio_de_muestra(bool debug);
void action_llenado_reservorio_con_muestra_a_medir(bool debug);
void action_purga_canal_muestra(bool debug);
void action_lavado_celda(bool debug);
void action_ajustes_fotometricos(bool debug);
void action_medicion(bool debug);
void action_lavado_final(bool debug);
void action_fin_sistema(bool debug);
void action_llenar_celda_medida(bool debug);
void action_vaciar_celda_medida(bool debug);
void action_calibrar(bool debug);
void action_llenado_con_reactivos(bool debug);
void action_medida_completa(bool debug);

void cbk_inicio_sistema(void);
void cbk_lavado_reservorio_de_muestra(void);
void cbk_llenado_reservorio_con_muestra_a_medir(void);
void cbk_purga_canal_muestra(void);
void cbk_lavado_celda(void);
void cbk_ajustes_fotometricos(void);
void cbk_medicion(void);
void cbk_lavado_final(void);
void cbk_fin_sistema(void);
void cbk_llenar_celda_medida(void);
void cbk_vaciar_celda_medida(void);
void cbk_llenado_con_reactivos(void);
void cbk_calibrar(void);
void cbk_medida_completa(void);


//------------------------------------------------------------------------------
void action_inicio_sistema( bool debug)
{
    if ( systemVars.midiendo == true) {
        xprintf_P(PSTR("ERROR: Sistema midiendo\r\n"));
    }
    
    actionCB.fn = cbk_inicio_sistema;
    actionCB.standby = false;
    actionCB.debug = debug;
}
//------------------------------------------------------------------------------
void action_lavado_reservorio_de_muestra(bool debug)
{
    if ( systemVars.midiendo == true) {
        xprintf_P(PSTR("ERROR: Sistema midiendo\r\n"));
    }
    
    actionCB.fn = cbk_lavado_reservorio_de_muestra;
    actionCB.standby = false;
    actionCB.debug = debug;    
}
//------------------------------------------------------------------------------
void action_llenado_reservorio_con_muestra_a_medir(bool debug)
{
    if ( systemVars.midiendo == true) {
        xprintf_P(PSTR("ERROR: Sistema midiendo\r\n"));
    }
    
    actionCB.fn = cbk_llenado_reservorio_con_muestra_a_medir;
    actionCB.standby = false;
    actionCB.debug = debug;    
}
//------------------------------------------------------------------------------
void action_purga_canal_muestra(bool debug)
{
    if ( systemVars.midiendo == true) {
        xprintf_P(PSTR("ERROR: Sistema midiendo\r\n"));
    }
    
    actionCB.fn = cbk_purga_canal_muestra;
    actionCB.standby = false;
    actionCB.debug = debug;    
}
//------------------------------------------------------------------------------
void action_lavado_celda(bool debug)
{
    if ( systemVars.midiendo == true) {
        xprintf_P(PSTR("ERROR: Sistema midiendo\r\n"));
    }
    
    actionCB.fn = cbk_lavado_celda;
    actionCB.standby = false;
    actionCB.debug = debug;    
}
//------------------------------------------------------------------------------
void action_ajustes_fotometricos(bool debug)
{
    if ( systemVars.midiendo == true) {
        xprintf_P(PSTR("ERROR: Sistema midiendo\r\n"));
    }
    
    actionCB.fn = cbk_ajustes_fotometricos;
    actionCB.standby = false;
    actionCB.debug = debug;    
}
//------------------------------------------------------------------------------
void action_medicion(bool debug)
{
    if ( systemVars.midiendo == true) {
        xprintf_P(PSTR("ERROR: Sistema midiendo\r\n"));
    }
    
    actionCB.fn = cbk_medicion;
    actionCB.standby = false;
    actionCB.debug = debug;    
}
//------------------------------------------------------------------------------
void action_lavado_final(bool debug)
{
    if ( systemVars.midiendo == true) {
        xprintf_P(PSTR("ERROR: Sistema midiendo\r\n"));
    }
    
    actionCB.fn = cbk_lavado_final;
    actionCB.standby = false;
    actionCB.debug = debug;    
}
//------------------------------------------------------------------------------
void action_fin_sistema(bool debug)
{
    if ( systemVars.midiendo == true) {
        xprintf_P(PSTR("ERROR: Sistema midiendo\r\n"));
    }
    
    actionCB.fn = cbk_fin_sistema;
    actionCB.standby = false;
    actionCB.debug = debug;    
}
//------------------------------------------------------------------------------
void action_llenar_celda_medida(bool debug)
{
    if ( systemVars.midiendo == true) {
        xprintf_P(PSTR("ERROR: Sistema midiendo\r\n"));
    }
    
    actionCB.fn = cbk_llenar_celda_medida;
    actionCB.standby = false;
    actionCB.debug = debug;    
}
//------------------------------------------------------------------------------
void action_vaciar_celda_medida(bool debug)
{
    if ( systemVars.midiendo == true) {
        xprintf_P(PSTR("ERROR: Sistema midiendo\r\n"));
    }
    
    actionCB.fn = cbk_vaciar_celda_medida;
    actionCB.standby = false;
    actionCB.debug = debug;    
}
//------------------------------------------------------------------------------
void action_calibrar(bool debug)
{
    if ( systemVars.midiendo == true) {
        xprintf_P(PSTR("ERROR: Sistema midiendo\r\n"));
    }
    
    actionCB.fn = cbk_calibrar;
    actionCB.standby = false;
    actionCB.debug = debug;    
}
//------------------------------------------------------------------------------
void action_llenado_con_reactivos(bool debug)
{
    if ( systemVars.midiendo == true) {
        xprintf_P(PSTR("ERROR: Sistema midiendo\r\n"));
    }
    
    actionCB.fn = cbk_llenado_con_reactivos;
    actionCB.standby = false;
    actionCB.debug = debug;    
}
//------------------------------------------------------------------------------
void action_medida_completa(bool debug)
{
    SYSTEM_ENTER_CRITICAL();
    if ( systemVars.midiendo == false ) {
        systemVars.midiendo = true;  
        actionCB.fn = cbk_medida_completa;
        actionCB.standby = false;
        actionCB.debug = debug; 
    } else {
        xprintf_P(PSTR("ERROR: Sistema midiendo\r\n"));
    }
    SYSTEM_EXIT_CRITICAL();
}
//------------------------------------------------------------------------------
void cbk_inicio_sistema(void)
{
    /*
     */
    
bool debug = actionCB.debug;
    
    if (debug) {
        xprintf_P(PSTR(">>Inicio_sistema: START\r\n"));
    }
    fsm_telepronter("Inicio Medidas");
    
    // Apago el opto
    opto_off(debug);
    await(1);
     
    // Cierro V0: No entra agua de muestra
    valve_0_close(debug);
    await(1);
    
    // Abro V1: Vacio el reservorio (por las dudas )
    valve_1_open(debug);
    await(1);
    
    // Abro V2: Vacio el canal de muestra (por las dudas)
    valve_2_open(debug);
    await(1);
    
    // Espero 15s que se vacie todo
    vTaskDelay( ( TickType_t)( 15000 / portTICK_PERIOD_MS ) );
    
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void cbk_lavado_reservorio_de_muestra(void)
{
    /*
     * LLena el tanque0 y lo vacia. 
     * Es solo para limpieza
     * 
     */
 
uint16_t timer;
bool debug = actionCB.debug;

    if (debug) {
        xprintf_P(PSTR(">>Lavado_reservorio_de_muestra: START\r\n"));
    }

   fsm_telepronter("Lavado tanque");

    // Cierro V1: ( Para que se pueda llenar el reservorio)
    valve_1_close(debug);
    await(1);
    
    // Espero que se cierra para que el agua no se escape al abrir V0
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );

    // Abro V0 para llenar reservorio
    counter_clear();
    valve_0_open(debug);
    await(1);
  
    // Espero que se llene ( Espero 15 secs maximo )
    for ( timer=0; timer <T_LLENADO_RESERVORIO_MAX ; timer++) {
        vTaskDelay( ( TickType_t)( (1000) / portTICK_PERIOD_MS ) );
        if ( counter_read() > CNT_LLENADO_RESERVORIO ) {
            break;
        }
    }
    
    if ( timer == T_LLENADO_RESERVORIO_MAX ) {
        xprintf_P(PSTR("ERROR timeout llenado de reservorio, V0 open\r\n"));  
    }
    
    // Cierro V0 ( Para que no entre mas agua.)
    valve_0_close(debug);
    await(1);
    
    xprintf_P(PSTR("COUNTER2=%lu\r\n"), counter_read());
  
    // Abro V1 para purgar reservorio ( vacio )
    valve_1_open(debug);
    await(1);
  
    // Espero vaciado de reservorio ( 30 secs )
    vTaskDelay( ( TickType_t)( (T_VACIADO_RESERVORIO*1000) / portTICK_PERIOD_MS ) );
    
    // Cierro V1
    valve_1_close(debug);
    await(1);
    
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void cbk_llenado_reservorio_con_muestra_a_medir(void)
{
    /*
     * Termina con el reservorio (Tanque0) lleno de agua
     */
 
uint16_t timer;
bool debug = actionCB.debug;

    if (debug) {
        xprintf_P(PSTR(">>Llenado_reservorio_con_muestra_a_medir:START\r\n"));
    }

    fsm_telepronter("Llenado tanque");
    
    // Cierro V1 ( para que no se escape agua mientras lleno)
    valve_1_close(debug);
    await(1);
    
    // Espero que se cierra para que el agua no se escape al abrir V0
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );

    // Abro V0 para llenar 
    counter_clear();
    valve_0_open(debug);
    await(1);
    
    // Espero que se llene ( Espero 15 secs maximo )
    for ( timer=0; timer <T_LLENADO_RESERVORIO_MAX ; timer++) {
        vTaskDelay( ( TickType_t)( (1000) / portTICK_PERIOD_MS ) );
        if ( counter_read() > CNT_LLENADO_RESERVORIO ) {
            break;
        }
    }
    
    if ( timer == T_LLENADO_RESERVORIO_MAX ) {
        xprintf_P(PSTR("ERROR timeout llenado de reservorio, V0 open\r\n"));  
    }
    
    // Cierro V0 ( Para que no entre mas agua.)
    valve_0_close(debug);
    await(1);
        
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void cbk_purga_canal_muestra(void)
{
 
    /*
     * Pasa agua al canal de medida durante 30s para limpiarlo
     * V2 esta abierta
     * 
     */
 
bool debug = actionCB.debug;

    if (debug) {
        xprintf_P(PSTR(">>Purga_canal_muestra: START\r\n"));
    }

    fsm_telepronter("Purga");

    // Abro V2 para que se purgue
    valve_2_open(debug);
    await(1);
    
    // Prendo M2 para purgar canal
    pump_2_run(debug, T_PURGA_CANAL );
    // Espero que purge
    vTaskDelay( ( TickType_t)( ( T_PURGA_CANAL * 1000) / portTICK_PERIOD_MS ) );
    // La bomba se apaga sola
    
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void cbk_lavado_celda(void)
{
    
bool debug = actionCB.debug;

    if (debug) {
        xprintf_P(PSTR(">>Lavado_celda: START\r\n"));
    }

    fsm_telepronter("Lavado celda");
    
    // Cierro V2
    valve_2_close(debug);
    await(1);
    
    // Espero para que no se escape agua
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
    // Prendo M2 para llenar celda de reacción
    pump_2_run(debug, T_LAVADO_CELDA);
    // Espero
    vTaskDelay( ( TickType_t)( ( T_LAVADO_CELDA * 1000) / portTICK_PERIOD_MS ) );
    
    // Abro V2 para vaciar celda
    valve_2_open(debug);
    await(1);  
    
    // Espero que se vacie
    vTaskDelay( ( TickType_t)( (T_VACIADO_RESERVORIO * 1000) / portTICK_PERIOD_MS ) );
    
    // Cierro V2: La dejo cerrada por las dudas
    valve_2_close(debug);
    await(1);   
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );

    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }    
}
//------------------------------------------------------------------------------
void cbk_ajustes_fotometricos(void)
{
 
bool debug = actionCB.debug;

    if (debug) {
        xprintf_P(PSTR(">>Ajustes_fotometricos: START\r\n"));
    }
 
    fsm_telepronter("Ajustes opto");
    
    // Cierro V2
    valve_2_close(debug);
    await(1);  
    // Espero para que no se escape el agua
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );

    // Prendo M2 para llenar la celda con muestra
    // El agua debe superar el nivel del led (10mL)
    pump_2_run(debug, T_LLENADO_CELDA);
    // Espero que purge
    vTaskDelay( ( TickType_t)( (T_LLENADO_CELDA * 1000) / portTICK_PERIOD_MS ) );
    // La bomba se apaga sola

    // Apago el opto
    opto_off(debug);
    await(1);
    
    // Registro la señal fotometrica en 128 medidas ( 0%T )
    adc_read(debug, 128);
    await(1);
    systemVars.S0 = adcCB.result;

    // Prendo el opto
    opto_on(debug);
    await(1);
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );

    // Registro la señal fotometrica en 128 medidas ( 100%T )
    adc_read(debug, 128);
    await(1);
    systemVars.S100 = adcCB.result;
    
    // Apago el opto
    opto_off(debug);
    await(1);

    // Abro V2 para vaciar la celda de reacción
    valve_2_open(debug);
    await(1);  
    vTaskDelay( ( TickType_t)( ( T_VACIADO_CELDA * 1000 ) / portTICK_PERIOD_MS ) );

    if ( debug) {
        xprintf_P(PSTR("S0=%d\r\n"), systemVars.S0);
        xprintf_P(PSTR("S100=%d\r\n"), systemVars.S100);
    }
    
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }    
}
//------------------------------------------------------------------------------
void cbk_medicion(void)
{
    
uint8_t ciclo;
uint16_t lp = 0;
float denom;
float pabs[CICLOS_MEDIDA];
bool debug = actionCB.debug;

    if(debug) {
        xprintf_P(PSTR(">>Medicion: START\r\n"));
    }

    fsm_telepronter("Medicion");    
    
    systemVars.absorbancia = 0.0;
    
    for (ciclo = 0; ciclo <  CICLOS_MEDIDA; ciclo++) {
           
        // Cierro V2
        valve_2_close(debug);
        await(1); 
        vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
        
        // Prendo M0 10s para dispensar 0.5ml de reactivo DPD
        pump_0_run(debug, T_DISPENSAR_DPD);
        // Espero
        vTaskDelay( ( TickType_t)( (T_DISPENSAR_DPD * 1000) / portTICK_PERIOD_MS ) );
        
        // Prendo M1 11.5 para dispensar 0.5ml de buffer
        pump_1_run(debug, T_DISPENSAR_BUFFER);
        // Espero
        vTaskDelay( ( TickType_t)( (T_DISPENSAR_BUFFER * 1000) / portTICK_PERIOD_MS ) );
        
        // Prendo M2  para dispensar 10mL de muestra
        action_pump_2_run(debug, T_LLENADO_CELDA );
        // Espero
        vTaskDelay( ( TickType_t)( (T_LLENADO_CELDA *1000) / portTICK_PERIOD_MS ) );
    
        // Prendo el OPTO
        opto_on(debug);
        await(1);
        // Espero 5 que se estabilize la señal fotometrica
        vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
        
        // Leo la señal fotometrica
        adc_read(debug, 128);
        await(1);
        lp = adcCB.result;
        
        // Calculamos la absorbancia
        denom = (1.0*(systemVars.S100 - systemVars.S0));
        if ( denom != 0 ) {
            pabs[ciclo] = -1.0 * log10f ( 1.0*( lp - systemVars.S0)/denom );
            systemVars.absorbancia += pabs[ciclo];
        
            if (debug) {
                xprintf_P(PSTR("Ciclo=%d, LP=%d, abs=%0.4f\r\n"), ciclo, lp, pabs[ciclo]);
            }
            
        } else {
           pabs[ciclo] = -1; 
           systemVars.absorbancia += pabs[ciclo];
           xprintf_P(PSTR("ERROR: S0==S100, abs=%0.4f\r\n"), pabs[ciclo]);
           
        }
        
        // Abro V2 para vaciar la celda de reacción
        valve_2_open(debug);
        await(1); 
        vTaskDelay( ( TickType_t)( (T_VACIADO_CELDA * 1000) / portTICK_PERIOD_MS ) );
    
    }

    // Apago el OPTO
    opto_off(debug);
    await(1);
    
    systemVars.absorbancia /= CICLOS_MEDIDA;
    
    systemVars.cloro_ppm = cloro_from_absorbancia(systemVars.absorbancia, debug);
    
    // Agrego el timestamp de cuando tome la medida
    RTC_rtc2strplain(&systemVars.timestamp[0]);
   
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }     
}
//------------------------------------------------------------------------------
void cbk_lavado_final(void)
{
 
uint8_t i;
bool debug = actionCB.debug;

    if (debug) {
        xprintf_P(PSTR(">>Lavado_final: START\r\n"));
    }

    fsm_telepronter("Lavado");
     
    // Repito 4 veces el ciclo de lavado
    for (i=0; i<CICLOS_LAVADO; i++) {
        
        // Cierro V2
        valve_2_close(debug);
        await(1); 
        vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
        // Prendo M2 para llenar celda de reacción con muestra
        pump_2_run(debug, T_LLENADO_CELDA);
        // Espero
        vTaskDelay( ( TickType_t)( (T_LLENADO_CELDA*1000) / portTICK_PERIOD_MS ) );
    
        // Abro V2
        valve_2_open(debug);
        await(1); 
        
        // Espero que se vacie
        vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );
    }

    // Dejo abierta V2
    valve_2_open(debug);
    await(1); 
    
     // Abro V1 para purgar reservorio
    valve_1_open(debug);
    await(1); 
    
    // Espero que se vacie
    vTaskDelay( ( TickType_t)( (T_VACIADO_RESERVORIO * 1000) / portTICK_PERIOD_MS ) );
    
    // Cierro V1 
    valve_1_close(debug);
    await(1); 

    // Cierro V2
    valve_2_close(debug);
    await(1); 
    
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
 
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void cbk_fin_sistema(void)
{
    /*
     * Dejo todo en su lugar
     */
 
bool debug = actionCB.debug;

    if (debug) {
        xprintf_P(PSTR(">>Fin_sistema: START\r\n"));
    }
   
    fsm_telepronter("Finalización");
     
    // Apago el opto
    opto_off(debug);
    await(1);
     
    // Cierro V0: No entra agua de muestra
    valve_0_close(debug);
    await(1);
    
    // Abro V1: Vacio el reservorio (por las dudas )
    valve_1_open(debug);
    await(1);
    
    // Abro V2: Vacio el canal de muestra (por las dudas)
    valve_2_open(debug);
    await(1);
    
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void cbk_llenar_celda_medida(void)
{
 
bool debug = actionCB.debug;

    if (debug) {
        xprintf_P(PSTR(">>Llenar celda medida: START\r\n"));
    }
 
    fsm_telepronter("Llenado celda");
    
    // Cierro V2
    valve_2_close(debug);
    await(1);  
    // Espero para que no se escape el agua
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );

    // Prendo M2 para llenar la celda con muestra
    // El agua debe superar el nivel del led (10mL)
    pump_2_run(debug, T_LLENADO_CELDA);
    // Espero que purge
    vTaskDelay( ( TickType_t)( (T_LLENADO_CELDA * 1000) / portTICK_PERIOD_MS ) );
    // La bomba se apaga sola 
  
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void cbk_vaciar_celda_medida(void)
{
    
bool debug = actionCB.debug;

    if (debug) {
        xprintf_P(PSTR(">>Vaciar celda medida: START\r\n"));
    }
 
    fsm_telepronter("Vaciado tanque");
    
    // Abro V2 para vaciar la celda de reacción
    valve_2_open(debug);
    await(1);  
    vTaskDelay( ( TickType_t)( ( T_VACIADO_CELDA * 1000 ) / portTICK_PERIOD_MS ) );
 
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void cbk_llenado_con_reactivos(void)
{
    /*
     * Realiza todo el ciclo de medida
     */

bool debug = actionCB.debug;

    if (debug) {
        xprintf_P(PSTR(">>:lenado_con_reactivos: START\r\n"));
    }
    
    fsm_telepronter("Reactivos");
     
    // Apago el opto
    opto_off(debug);
    await(1);
     
    // Cierro V0: No entra agua de muestra
    valve_0_close(debug);
    valve_1_close(debug);
    await(1);
    // Abro V2: Vacio el canal de muestra (por las dudas)
    valve_2_open(debug);
    await(1);
    valve_2_close(debug);
    await(1);
    
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
        
    // Prendo M0 10s para dispensar 0.5ml de reactivo DPD
    pump_0_run(debug, T_DISPENSAR_DPD);
    vTaskDelay( ( TickType_t)( (T_DISPENSAR_DPD * 1000) / portTICK_PERIOD_MS ) );
        
    // Prendo M1 11.5 para dispensar 0.5ml de buffer
    pump_1_run(debug, T_DISPENSAR_BUFFER);
    vTaskDelay( ( TickType_t)( (T_DISPENSAR_BUFFER * 1000) / portTICK_PERIOD_MS ) );
        
    // Prendo M2  para dispensar 10mL de muestra
    pump_2_run(debug, T_LLENADO_CELDA );
    vTaskDelay( ( TickType_t)( (T_LLENADO_CELDA *1000) / portTICK_PERIOD_MS ) );

    //proc_inicio_sistema(debug);
    //proc_lavado_reservorio_de_muestra(debug);
    //proc_llenado_reservorio_con_muestra_a_medir(debug);
    //proc_purga_canal_muestra(debug);
    //proc_lavado_celda(debug);
    //proc_ajustes_fotometricos(debug);
    //proc_medicion(debug);
   
    // Apago el opto
    //action_opto_off(debug);
    //action_await();
     
    // Cierro V0: No entra agua de muestra
    //action_valve_0_close(debug);
    //action_await();
       
    // Abro V2: Vacio el canal de muestra (por las dudas)
    //action_valve_2_open(debug);
    //action_await();
 
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void cbk_calibrar(void)
{
   
    /*
     * Realiza todo el ciclo de medida
     */
 
 bool debug = actionCB.debug;
 
    if (debug) {
        xprintf_P(PSTR(">>Calibrar: START\r\n"));
    }
    
    fsm_telepronter("Calibrar");
    
    // Apago el opto
    opto_off(debug);
    await(1);
     
    // Cierro V0: No entra agua de muestra
    valve_0_close(debug);
    valve_1_close(debug);
    await(1);
    // Abro V2: Vacio el canal de muestra (por las dudas)
    valve_2_open(debug);
    await(1);

    //proc_inicio_sistema(debug);
    //proc_lavado_reservorio_de_muestra(debug);
    //proc_llenado_reservorio_con_muestra_a_medir(debug);
    //proc_purga_canal_muestra(debug);
    cbk_lavado_celda();
    cbk_ajustes_fotometricos();
    cbk_medicion();
   
    // Apago el opto
    opto_off(debug);
    await(1);
     
    // Cierro V0: No entra agua de muestra
    valve_0_close(debug);
    await(1);
       
    // Abro V2: Vacio el canal de muestra (por las dudas)
    valve_2_open(debug);
    await(1);
 
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void cbk_medida_completa(void)
{
    /*
     * Realiza todo el ciclo de medida
     */
    
 bool debug = actionCB.debug;
 
    if (debug) {
        xprintf_P(PSTR(">>Medida Completa: START\r\n"));
    }
    
    fsm_display_modo_medir();
    fsm_telepronter("Medida");

    cbk_inicio_sistema();
    cbk_lavado_reservorio_de_muestra();
    cbk_llenado_reservorio_con_muestra_a_medir();
    //proc_purga_canal_muestra(debug);
    cbk_lavado_celda();
    cbk_ajustes_fotometricos();
    cbk_medicion();
    cbk_fin_sistema();
    
    fsm_set_keypressed(KEY_DUMMY1);
     
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
    
    systemVars.midiendo = false; 
    
    
}
//------------------------------------------------------------------------------
/*
 * ACCIONES BASICAS QUE SE REALIZAN SOBRE LOS PERIFERICOS
 */
//------------------------------------------------------------------------------
void __action_await(void)
{
    while ( ! actionCB.standby ) {
        vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
    }
}
//------------------------------------------------------------------------------
void await(uint16_t secs )
{
    while ( secs-- > 0  ) {
        vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
    }
}
//------------------------------------------------------------------------------
void action_opto_on(bool debug)
{
    /*
     * Implementacion de la accion de prender el opto por tkSys
     */
    
    optoCB.debug = debug;
    actionCB.fn = cbk_opto_on;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_opto_off(bool debug)
{
    optoCB.debug = debug;   
    actionCB.fn = cbk_opto_off;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_valve_0_open(bool debug)
{
    valveCB_0.debug = debug; 
    actionCB.fn = cbk_valve_0_open;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_valve_0_close(bool debug)
{
    valveCB_0.debug = debug;   
    actionCB.fn = cbk_valve_0_close;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_valve_1_open(bool debug)
{
    valveCB_1.debug = debug;
    actionCB.fn = cbk_valve_1_open;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_valve_1_close(bool debug)
{
    valveCB_1.debug = debug;    
    actionCB.fn = cbk_valve_1_close;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_valve_2_open(bool debug)
{
    valveCB_2.debug = debug;   
    actionCB.fn = cbk_valve_2_open;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_valve_2_close(bool debug)
{
    valveCB_2.debug = debug;    
    actionCB.fn = cbk_valve_2_close;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_pump_0_run( bool debug, uint16_t secs)
{
    pumpCB_0.debug = debug;
    pumpCB_0.secs = secs;   
    actionCB.fn = cbk_pump_0_run;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_pump_0_stop( bool debug)
{
    pumpCB_0.debug = debug;   
    actionCB.fn = cbk_pump_0_stop;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_pump_1_run( bool debug, uint16_t secs)
{
    pumpCB_1.debug = debug;
    pumpCB_1.secs = secs;    
    actionCB.fn = cbk_pump_1_run;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_pump_1_stop( bool debug)
{
    pumpCB_1.debug = debug;   
    actionCB.fn = cbk_pump_1_stop;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_pump_2_run( bool debug, uint16_t secs)
{
    pumpCB_2.debug = debug;
    pumpCB_2.secs = secs; 
    actionCB.fn = cbk_pump_2_run;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_pump_2_stop( bool debug)
{
    pumpCB_2.debug = debug;   
    actionCB.fn = cbk_pump_2_stop;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_adc_read( bool debug, uint16_t counts)
{
    adcCB.debug = debug;
    adcCB.counts = counts;
    
    actionCB.fn = cbk_adc_read;
    actionCB.standby = false;
    
    // El resultado queda en adcCB.result
}
//------------------------------------------------------------------------------
