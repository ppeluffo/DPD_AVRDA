
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
 * Conjunto de acciones basicas
 */
//------------------------------------------------------------------------------
void proc_inicio_sistema(bool debug)
{
    /*
     */
    
    if (debug) {
        xprintf_P(PSTR(">>Inicio_sistema: START\r\n"));
    }
    
    // Apago el opto
    action_opto_off(debug);
    action_await();
     
    // Cierro V0: No entra agua de muestra
    action_valve_0_close(debug);
    action_await();
    
    // Abro V1: Vacio el reservorio (por las dudas )
    action_valve_1_open(debug);
    action_await();
    
    // Abro V2: Vacio el canal de muestra (por las dudas)
    action_valve_2_open(debug);
    action_await();
    
    // Espero 15s que se vacie todo
    vTaskDelay( ( TickType_t)( 15000 / portTICK_PERIOD_MS ) );
    
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void proc_lavado_reservorio_de_muestra(bool debug)
{
    /*
     * LLena el tanque0 y lo vacia. 
     * Es solo para limpieza
     * 
     */
 
uint16_t timer;
    
    if (debug) {
        xprintf_P(PSTR(">>Lavado_reservorio_de_muestra: START\r\n"));
    }
 
    // Cierro V1: ( Para que se pueda llenar el reservorio)
    action_valve_1_close(debug);
    action_await();
    // Espero que se cierra para que el agua no se escape al abrir V0
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );

    // Abro V0 para llenar reservorio
    counter_clear();
    action_valve_0_open(debug);
    action_await();
  
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
    action_valve_0_close(debug);
    action_await();
    xprintf_P(PSTR("COUNTER2=%lu\r\n"), counter_read());
  
    // Abro V1 para purgar reservorio ( vacio )
    action_valve_1_open(debug);
    action_await();
    
    // Espero vaciado de reservorio ( 30 secs )
    vTaskDelay( ( TickType_t)( (T_VACIADO_RESERVORIO*1000) / portTICK_PERIOD_MS ) );
    
    // Cierro V1
    action_valve_1_close(debug);
    action_await();
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void proc_llenado_reservorio_con_muestra_a_medir(bool debug)
{
    /*
     * Termina con el reservorio (Tanque0) lleno de agua
     */
 
uint16_t timer;

    if (debug) {
        xprintf_P(PSTR(">>Llenado_reservorio_con_muestra_a_medir:START\r\n"));
    }
 
    // Cierro V1 ( para que no se escape agua mientras lleno)
    action_valve_1_close(debug);
    action_await();
    // Espero que se cierra para que el agua no se escape al abrir V0
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );

    // Abro V0 para llenar 
    counter_clear();
    action_valve_0_open(debug);
    action_await();
    
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
    action_valve_0_close(debug);
    action_await();
        
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void proc_purga_canal_muestra(bool debug)
{
 
    /*
     * Pasa agua al canal de medida durante 30s para limpiarlo
     * V2 esta abierta
     * 
     */
    
    if (debug) {
        xprintf_P(PSTR(">>Purga_canal_muestra: START\r\n"));
    }

    // Abro V2 para que se purgue
    action_valve_2_open(debug);
    action_await();
    
    // Prendo M2 para purgar canal
    action_pump_2_run(debug, T_PURGA_CANAL );
    // Espero que purge
    vTaskDelay( ( TickType_t)( ( T_PURGA_CANAL * 1000) / portTICK_PERIOD_MS ) );
    // La bomba se apaga sola
    
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void proc_lavado_celda(bool debug)
{
    if (debug) {
        xprintf_P(PSTR(">>Lavado_celda: START\r\n"));
    }

    // Cierro V2
    action_valve_2_close(debug);
    action_await();
    // Espero para que no se escape agua
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
    // Prendo M2 para llenar celda de reacción
    action_pump_2_run(debug, T_LAVADO_CELDA);
    // Espero
    vTaskDelay( ( TickType_t)( ( T_LAVADO_CELDA * 1000) / portTICK_PERIOD_MS ) );
    
    // Abro V2 para vaciar celda
    action_valve_2_open(debug);
    action_await();  
    
    // Espero que se vacie
    vTaskDelay( ( TickType_t)( (T_VACIADO_RESERVORIO * 1000) / portTICK_PERIOD_MS ) );
    
    // Cierro V2: La dejo cerrada por las dudas
    action_valve_2_close(debug);
    action_await();   
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );

    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }    
}
//------------------------------------------------------------------------------
void proc_ajustes_fotometricos(bool debug)
{
        
    if (debug) {
        xprintf_P(PSTR(">>Ajustes_fotometricos: START\r\n"));
    }
 
    // Cierro V2
    action_valve_2_close(debug);
    action_await();  
    // Espero para que no se escape el agua
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );

    // Prendo M2 para llenar la celda con muestra
    // El agua debe superar el nivel del led (10mL)
    action_pump_2_run(debug, T_LLENADO_CELDA);
    // Espero que purge
    vTaskDelay( ( TickType_t)( (T_LLENADO_CELDA * 1000) / portTICK_PERIOD_MS ) );
    // La bomba se apaga sola

    // Apago el opto
    action_opto_off(debug);
    action_await();
    
    // Registro la señal fotometrica en 128 medidas ( 0%T )
    action_adc_read(debug, 128);
    action_await();
    systemVars.S0 = adcCB.result;

    // Prendo el opto
    action_opto_on(debug);
    action_await();
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );

    // Registro la señal fotometrica en 128 medidas ( 100%T )
    action_adc_read(debug, 128);
    action_await();
    systemVars.S100 = adcCB.result;
    
    // Apago el opto
    action_opto_off(debug);
    action_await();

    // Abro V2 para vaciar la celda de reacción
    action_valve_2_open(debug);
    action_await();  
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
void proc_medicion(bool debug)
{
    
uint8_t ciclo;
uint16_t lp = 0;
float pabs[CICLOS_MEDIDA];

    if(debug) {
        xprintf_P(PSTR(">>Medicion: START\r\n"));
    }

    systemVars.absorbancia = 0.0;
    
    for (ciclo = 0; ciclo <  CICLOS_MEDIDA; ciclo++) {
           
        // Cierro V2
        action_valve_2_close(debug);
        action_await(); 
        vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
        
        // Prendo M0 10s para dispensar 0.5ml de reactivo DPD
        action_pump_0_run(debug, T_DISPENSAR_DPD);
        // Espero
        vTaskDelay( ( TickType_t)( (T_DISPENSAR_DPD * 1000) / portTICK_PERIOD_MS ) );
        
        // Prendo M1 11.5 para dispensar 0.5ml de buffer
        action_pump_1_run(debug, T_DISPENSAR_BUFFER);
        // Espero
        vTaskDelay( ( TickType_t)( (T_DISPENSAR_BUFFER * 1000) / portTICK_PERIOD_MS ) );
        
        // Prendo M2  para dispensar 10mL de muestra
        action_pump_2_run(debug, T_LLENADO_CELDA );
        // Espero
        vTaskDelay( ( TickType_t)( (T_LLENADO_CELDA *1000) / portTICK_PERIOD_MS ) );
    
        // Prendo el OPTO
        action_opto_on(debug);
        action_await();
        // Espero 5 que se estabilize la señal fotometrica
        vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
        
        // Leo la señal fotometrica
        action_adc_read(debug, 128);
        action_await();
        lp = adcCB.result;
        
        // Calculamos la absorbancia
        pabs[ciclo] = -1.0 * log10f ( 1.0*( lp - systemVars.S0)/(1.0*(systemVars.S100 - systemVars.S0)) );
        systemVars.absorbancia += pabs[ciclo];
        
        if (debug) {
            xprintf_P(PSTR("Ciclo=%d, LP=%d, abs=%0.4f\r\n"), ciclo, lp, pabs[ciclo]);
        }
        
        // Abro V2 para vaciar la celda de reacción
        action_valve_2_open(debug);
        action_await(); 
        vTaskDelay( ( TickType_t)( (T_VACIADO_CELDA * 1000) / portTICK_PERIOD_MS ) );
    
    }

    // Apago el OPTO
    action_opto_off(debug);
    action_await();
    
    systemVars.absorbancia /= CICLOS_MEDIDA;
    systemVars.cloro_ppm = cloro_from_absorbancia(systemVars.absorbancia, debug);
            
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }     
}
//------------------------------------------------------------------------------
void proc_lavado_final(bool debug)
{
 
uint8_t i;

    if (debug) {
        xprintf_P(PSTR(">>Lavado_final: START\r\n"));
    }

    // Repito 4 veces el ciclo de lavado
    for (i=0; i<CICLOS_LAVADO; i++) {
        
        // Cierro V2
        action_valve_2_close(debug);
        action_await(); 
        vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
        // Prendo M2 para llenar celda de reacción con muestra
        action_pump_2_run(debug, T_LLENADO_CELDA);
        // Espero
        vTaskDelay( ( TickType_t)( (T_LLENADO_CELDA*1000) / portTICK_PERIOD_MS ) );
    
        // Abro V2
        action_valve_2_open(debug);
        action_await(); 
        
        // Espero que se vacie
        vTaskDelay( ( TickType_t)( 10000 / portTICK_PERIOD_MS ) );
    }

    // Dejo abierta V2
    action_valve_2_open(debug);
    action_await(); 
    
     // Abro V1 para purgar reservorio
    action_valve_1_open(debug);
    action_await(); 
    
    // Espero que se vacie
    vTaskDelay( ( TickType_t)( (T_VACIADO_RESERVORIO * 1000) / portTICK_PERIOD_MS ) );
    
    // Cierro V1 
    action_valve_1_close(debug);
    action_await(); 

    // Cierro V2
    action_valve_2_close(debug);
    action_await(); 
    
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
 
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void proc_fin_sistema(bool debug)
{
    /*
     * Dejo todo en su lugar
     */
    
    if (debug) {
        xprintf_P(PSTR(">>Fin_sistema: START\r\n"));
    }
    
    // Apago el opto
    action_opto_off(debug);
    action_await();
     
    // Cierro V0: No entra agua de muestra
    action_valve_0_close(debug);
    action_await();
    
    // Abro V1: Vacio el reservorio (por las dudas )
    action_valve_1_open(debug);
    action_await();
    
    // Abro V2: Vacio el canal de muestra (por las dudas)
    action_valve_2_open(debug);
    action_await();
    
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }
}
//------------------------------------------------------------------------------
void proc_llenar_celda_medida(bool debug)
{
        
    if (debug) {
        xprintf_P(PSTR(">>Llenar celda medida: START\r\n"));
    }
 
    // Cierro V2
    action_valve_2_close(debug);
    action_await();  
    // Espero para que no se escape el agua
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );

    // Prendo M2 para llenar la celda con muestra
    // El agua debe superar el nivel del led (10mL)
    action_pump_2_run(debug, T_LLENADO_CELDA);
    // Espero que purge
    vTaskDelay( ( TickType_t)( (T_LLENADO_CELDA * 1000) / portTICK_PERIOD_MS ) );
    // La bomba se apaga sola    
}
//------------------------------------------------------------------------------
void proc_vaciar_celda_medida(bool debug)
{
    if (debug) {
        xprintf_P(PSTR(">>Vaciar celda medida: START\r\n"));
    }
  
    // Abro V2 para vaciar la celda de reacción
    action_valve_2_open(debug);
    action_await();  
    vTaskDelay( ( TickType_t)( ( T_VACIADO_CELDA * 1000 ) / portTICK_PERIOD_MS ) );
   
}
//------------------------------------------------------------------------------
void proc_calibrar(bool debug)
{
    /*
     * Realiza todo el ciclo de medida
     */
    if (debug) {
        xprintf_P(PSTR(">>Calibrar: START\r\n"));
    }
        // Apago el opto
    action_opto_off(debug);
    action_await();
     
    // Cierro V0: No entra agua de muestra
    action_valve_0_close(debug);
    action_valve_1_close(debug);
    action_await();
    // Abro V2: Vacio el canal de muestra (por las dudas)
    action_valve_2_open(debug);
    action_await();

    //proc_inicio_sistema(debug);
    //proc_lavado_reservorio_de_muestra(debug);
    //proc_llenado_reservorio_con_muestra_a_medir(debug);
    //proc_purga_canal_muestra(debug);
    proc_lavado_celda(debug);
    proc_ajustes_fotometricos(debug);
    proc_medicion(debug);
   
    // Apago el opto
    action_opto_off(debug);
    action_await();
     
    // Cierro V0: No entra agua de muestra
    action_valve_0_close(debug);
    action_await();
       
    // Abro V2: Vacio el canal de muestra (por las dudas)
    action_valve_2_open(debug);
    action_await();
 
}
//------------------------------------------------------------------------------
void proc_llenado_con_reactivos(bool debug)
{
    /*
     * Realiza todo el ciclo de medida
     */
    if (debug) {
        xprintf_P(PSTR(">>:lenado_con_reactivos: START\r\n"));
    }
        // Apago el opto
    action_opto_off(debug);
    action_await();
     
    // Cierro V0: No entra agua de muestra
    action_valve_0_close(debug);
    action_valve_1_close(debug);
    action_await();
    // Abro V2: Vacio el canal de muestra (por las dudas)
    action_valve_2_open(debug);
    action_await();
    action_valve_2_close(debug);
    action_await();
    
    vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
        
    // Prendo M0 10s para dispensar 0.5ml de reactivo DPD
    action_pump_0_run(debug, T_DISPENSAR_DPD);
    vTaskDelay( ( TickType_t)( (T_DISPENSAR_DPD * 1000) / portTICK_PERIOD_MS ) );
        
    // Prendo M1 11.5 para dispensar 0.5ml de buffer
    action_pump_1_run(debug, T_DISPENSAR_BUFFER);
    vTaskDelay( ( TickType_t)( (T_DISPENSAR_BUFFER * 1000) / portTICK_PERIOD_MS ) );
        
    // Prendo M2  para dispensar 10mL de muestra
    action_pump_2_run(debug, T_LLENADO_CELDA );
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
 
}
//------------------------------------------------------------------------------
void proc_medida_completa(bool debug)
{
    /*
     * Realiza todo el ciclo de medida
     */
    if (debug) {
        xprintf_P(PSTR(">>Medida Completa: START\r\n"));
    }
    
    proc_inicio_sistema(debug);
    proc_lavado_reservorio_de_muestra(debug);
    proc_llenado_reservorio_con_muestra_a_medir(debug);
    //proc_purga_canal_muestra(debug);
    proc_lavado_celda(debug);
    proc_ajustes_fotometricos(debug);
    proc_medicion(debug);
    proc_fin_sistema(debug);
    
    if (debug) {
        xprintf_P(PSTR(">>END\r\n"));
    }    
}
//------------------------------------------------------------------------------
/*
 * ACCIONES BASICAS QUE SE REALIZAN SOBRE LOS PERIFERICOS
 */
//------------------------------------------------------------------------------
void action_await(void)
{
    while ( ! actionCB.standby ) {
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
    
    actionCB.fn = fn_opto_on;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_opto_off(bool debug)
{
    optoCB.debug = debug;
    
    actionCB.fn = fn_opto_off;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_valve_0_open(bool debug)
{
    valveCB_0.debug = debug;
    
    actionCB.fn = fn_valve_0_open;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_valve_0_close(bool debug)
{
    valveCB_0.debug = debug;
    
    actionCB.fn = fn_valve_0_close;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_valve_1_open(bool debug)
{
    valveCB_1.debug = debug;

    actionCB.fn = fn_valve_1_open;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_valve_1_close(bool debug)
{
    valveCB_1.debug = debug;
    
    actionCB.fn = fn_valve_1_close;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_valve_2_open(bool debug)
{
    valveCB_2.debug = debug;
    
    actionCB.fn = fn_valve_2_open;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_valve_2_close(bool debug)
{
    valveCB_2.debug = debug;
    
    actionCB.fn = fn_valve_2_close;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_pump_0_run( bool debug, uint16_t secs)
{
    pumpCB_0.debug = debug;
    pumpCB_0.secs = secs;
    
    actionCB.fn = fn_pump_0_run;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_pump_0_stop( bool debug)
{
    pumpCB_0.debug = debug;
    
    actionCB.fn = fn_pump_0_stop;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_pump_1_run( bool debug, uint16_t secs)
{
    pumpCB_1.debug = debug;
    pumpCB_1.secs = secs;
    
    actionCB.fn = fn_pump_1_run;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_pump_1_stop( bool debug)
{
    pumpCB_1.debug = debug;
    
    actionCB.fn = fn_pump_1_stop;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_pump_2_run( bool debug, uint16_t secs)
{
    pumpCB_2.debug = debug;
    pumpCB_2.secs = secs;
    
    actionCB.fn = fn_pump_2_run;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_pump_2_stop( bool debug)
{
    pumpCB_2.debug = debug;
    
    actionCB.fn = fn_pump_2_stop;
    actionCB.standby = false;
}
//------------------------------------------------------------------------------
void action_adc_read( bool debug, uint16_t counts)
{
    adcCB.debug = debug;
    adcCB.counts = counts;
    
    actionCB.fn = fn_adc_read;
    actionCB.standby = false;
    
    // El resultado queda en adcCB.result
}
//------------------------------------------------------------------------------
