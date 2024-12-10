
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
void cbk_lavado_calibracion(void);
void cbk_calibrar(void);
void cbk_medida_completa(void);

int16_t compare( const void* a, const void* b);

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
void action_opto_on(bool debug)
{
    /*
     * Implementacion de la accion de prender el opto por tkSys
     */
    
    if (actionCB.standby) {
        optoCB.debug = debug;
        actionCB.fn = cbk_opto_on;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
}
//------------------------------------------------------------------------------
void action_opto_off(bool debug)
{
    if (actionCB.standby) {
        optoCB.debug = debug;   
        actionCB.fn = cbk_opto_off;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
}
//------------------------------------------------------------------------------
void action_valve_0_open(bool debug)
{
    if (actionCB.standby) {
        valveCB_0.debug = debug; 
        actionCB.fn = cbk_valve_0_open;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
}
//------------------------------------------------------------------------------
void action_valve_0_close(bool debug)
{
    if (actionCB.standby) {
        valveCB_0.debug = debug;   
        actionCB.fn = cbk_valve_0_close;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
}
//------------------------------------------------------------------------------
void action_valve_1_open(bool debug)
{
    if (actionCB.standby) {
        valveCB_1.debug = debug;
        actionCB.fn = cbk_valve_1_open;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
}
//------------------------------------------------------------------------------
void action_valve_1_close(bool debug)
{
    if (actionCB.standby) {
        valveCB_1.debug = debug;    
        actionCB.fn = cbk_valve_1_close;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
}
//------------------------------------------------------------------------------
void action_valve_2_open(bool debug)
{
    if (actionCB.standby) {
        valveCB_2.debug = debug;   
        actionCB.fn = cbk_valve_2_open;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
}
//------------------------------------------------------------------------------
void action_valve_2_close(bool debug)
{
    if (actionCB.standby) {
        valveCB_2.debug = debug;    
        actionCB.fn = cbk_valve_2_close;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
}
//------------------------------------------------------------------------------
void action_pump_0_run( bool debug, uint16_t secs)
{
    if (actionCB.standby) {
        pumpCB_0.debug = debug;
        pumpCB_0.secs = secs;   
        actionCB.fn = cbk_pump_0_run;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
}
//------------------------------------------------------------------------------
void action_pump_0_stop( bool debug)
{
    if (actionCB.standby) {
        pumpCB_0.debug = debug;   
        actionCB.fn = cbk_pump_0_stop;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
}
//------------------------------------------------------------------------------
void action_pump_1_run( bool debug, uint16_t secs)
{
    if (actionCB.standby) {
        pumpCB_1.debug = debug;
        pumpCB_1.secs = secs;    
        actionCB.fn = cbk_pump_1_run;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
}
//------------------------------------------------------------------------------
void action_pump_1_stop( bool debug)
{
    if (actionCB.standby) {
        pumpCB_1.debug = debug;   
        actionCB.fn = cbk_pump_1_stop;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
}
//------------------------------------------------------------------------------
void action_pump_2_run( bool debug, uint16_t secs)
{
    if (actionCB.standby) {
        pumpCB_2.debug = debug;
        pumpCB_2.secs = secs; 
        actionCB.fn = cbk_pump_2_run;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
}
//------------------------------------------------------------------------------
void action_pump_2_stop( bool debug)
{
    if (actionCB.standby) {
        pumpCB_2.debug = debug;   
        actionCB.fn = cbk_pump_2_stop;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
}
//------------------------------------------------------------------------------
void action_adc_read( bool debug, uint16_t counts)
{
    if (actionCB.standby) {
        adcCB.debug = debug;
        adcCB.counts = counts;
    
        actionCB.fn = cbk_adc_read;
        actionCB.standby = false;
    } else {
        xprintf_P(PSTR("Busy\r\n"));
    }
    
    // El resultado queda en adcCB.result
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
    fsm_telepronter("Inicio Sistema");
    
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
    if (debug) {
        xprintf_P(PSTR("Await 15 s...\r\n"));
    }
    vTaskDelay( ( TickType_t)( 15000 / portTICK_PERIOD_MS ) );
  
/* 
 * EXIT:
 * Led off
 * V0 close
 * V1 open
 * V2 open
 */
    
    if (debug) {
        xprintf_P(PSTR(">>Inicio_sistema: END\r\n"));
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
   
   if ( ! V1_isClosed()) {
        // Cierro V1: ( Para que se pueda llenar el reservorio)
        valve_1_close(debug);
        await(1);
        // Espero que se cierra para que el agua no se escape al abrir V0
        vTaskDelay( ( TickType_t)( 3000 / portTICK_PERIOD_MS ) );
   }
   
    // Abro V0 para llenar reservorio
    counter_clear();
    if ( V0_isClosed() ) {
        valve_0_open(debug);
        await(1);
    }
    
    // Espero que se llene 
    if (debug) {
        xprintf_P(PSTR("Llenando..\r\n"));
    }
    for ( timer=0; timer < T_LLENADO_RESERVORIO ; timer++) {
        vTaskDelay( ( TickType_t)( (1000) / portTICK_PERIOD_MS ) );
        if ( counter_read() > CNT_LLENADO_RESERVORIO ) {
            break;
        }
    }
    
    if ( timer == T_LLENADO_RESERVORIO ) {
        xprintf_P(PSTR("ERROR: timeout llenado de reservorio de muestra\r\n"));  
    }
    
    // Cierro V0 ( Para que no entre mas agua.)
    if ( ! V0_isClosed() ) {
        valve_0_close(debug);
        await(1);
    }
    
    xprintf_P(PSTR("COUNTER=%lu\r\n"), counter_read());
  
    // Abro V1 para purgar reservorio ( vacio )
    if ( V1_isClosed() ) {
        valve_1_open(debug);
        await(1);
        if (debug) {
            xprintf_P(PSTR("Await %d s...\r\n"),T_VACIADO_RESERVORIO);
        }
        // Espero vaciado de reservorio ( 30 secs )
        vTaskDelay( ( TickType_t)( (T_VACIADO_RESERVORIO*1000) / portTICK_PERIOD_MS ) );
    }
    
// EXIT
    
    if ( ! V1_isClosed() ) {
        // Cierro V1
        valve_1_close(debug);
        await(1);
        vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    }
    
    if (debug) {
        xprintf_P(PSTR(">>Lavado_reservorio_de_muestra: END\r\n"));
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
        xprintf_P(PSTR(">>Llenado_reservorio_con_muestra_a_medir: START\r\n"));
    }

    fsm_telepronter("Llenado tanque");
  
    if ( ! V1_isClosed() ) {
        // Cierro V1 ( para que no se escape agua mientras lleno)
        valve_1_close(debug);
        await(1);
        // Espero que se cierra para que el agua no se escape al abrir V0
        vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    }
    
    // Abro V0 para llenar 
    counter_clear();
    if ( V0_isClosed() ) {
        valve_0_open(debug);
        await(1);
    }
    
    // Espero que se llene
    if (debug) {
        xprintf_P(PSTR("Llenando..\r\n"));
    }
    for ( timer=0; timer < T_LLENADO_RESERVORIO ; timer++) {
        vTaskDelay( ( TickType_t)( (1000) / portTICK_PERIOD_MS ) );
        if ( counter_read() > CNT_LLENADO_RESERVORIO ) {
            break;
        }
    }
 
    if ( timer == T_LLENADO_RESERVORIO ) {
        xprintf_P(PSTR("ERROR timeout llenado de reservorio\r\n"));  
    }
    
    if ( ! V0_isClosed() ) {
        // Cierro V0 ( Para que no entre mas agua.)
        valve_0_close(debug);
        await(1);
    }
    
    xprintf_P(PSTR("COUNTER=%lu\r\n"), counter_read());

    if (debug) {
        xprintf_P(PSTR(">>Llenado_reservorio_con_muestra_a_medir: END\r\n"));
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
    if ( V2_isClosed() ) {
        valve_2_open(debug);
        await(1);
    }
    
    // Prendo M2 para purgar canal
    pump_2_run(debug, T_PURGA_CANAL );
    // Espero que purge
    vTaskDelay( ( TickType_t)( ( T_PURGA_CANAL * 1000) / portTICK_PERIOD_MS ) );
    // La bomba se apaga sola
    
    if (debug) {
        xprintf_P(PSTR(">>Purga_canal_muestra: END\r\n"));
    }
}
//------------------------------------------------------------------------------
void cbk_lavado_celda(void)
{
    /*
     * El lavado se hace una sola vez.
     * Si se requiere mas veces hay que invocar la funcion varias veces
     */
    
bool debug = actionCB.debug;

    if (debug) {
        xprintf_P(PSTR(">>Lavado_celda: START\r\n"));
    }

    fsm_telepronter("Lavado celda");
    
    // Cierro V2
    if ( ! V2_isClosed() ) {
        valve_2_close(debug);
        await(1);
        // Espero para que no se escape agua
        vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    }
    
    // Prendo M2 para llenar celda de reacción
    pump_2_run(debug, T_LAVADO_CELDA);
    // Espero
    vTaskDelay( ( TickType_t)( ( T_LAVADO_CELDA * 1000) / portTICK_PERIOD_MS ) );
    
    // Abro V2 para vaciar celda
    if ( V2_isClosed() ) {
        valve_2_open(debug);
        await(1);  
        // Espero que se vacie
        vTaskDelay( ( TickType_t)( (T_VACIADO_CELDA * 1000) / portTICK_PERIOD_MS ) );
    }
    
    
    // Cierro V2: La dejo cerrada por las dudas
    if ( ! V2_isClosed() ) {
        valve_2_close(debug);
        await(1);   
        vTaskDelay( ( TickType_t)( 3000 / portTICK_PERIOD_MS ) );
    }
    
    if (debug) {
        xprintf_P(PSTR(">>Lavado_celda: END\r\n"));
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
    if ( ! V2_isClosed() ) {
        valve_2_close(debug);
        await(1);  
        // Espero para que no se escape el agua
        vTaskDelay( ( TickType_t)( 3000 / portTICK_PERIOD_MS ) );
    }
    
    // Prendo M2 para llenar la celda con muestra
    // El agua debe superar el nivel del led (10mL)
    pump_2_run(debug, T_LLENADO_CELDA_S100);
    // Espero que purge
    vTaskDelay( ( TickType_t)( (T_LLENADO_CELDA_S100 * 1000) / portTICK_PERIOD_MS ) );
    // La bomba se apaga sola

    // Apago el opto
    opto_off(debug);
    await(1);
    
    // Registro la señal fotometrica en 128 medidas ( 0%T )
    adc_read(debug, 128);
    await(1);
    systemConf.S0 = adcCB.result;

    // Prendo el opto
    opto_on(debug);
    await(1);
    vTaskDelay( ( TickType_t)( 3000 / portTICK_PERIOD_MS ) );

    // Registro la señal fotometrica en 128 medidas ( 100%T )
    adc_read(debug, 128);
    await(1);
    systemConf.S100 = adcCB.result;
    
    // Apago el opto
    opto_off(debug);
    await(1);

    // Abro V2 para vaciar la celda de reacción
    if ( V2_isClosed() ) {
        valve_2_open(debug);
        await(1);  
        vTaskDelay( ( TickType_t)( ( T_VACIADO_CELDA * 1000 ) / portTICK_PERIOD_MS ) );
    }
    
    if ( debug) {
        xprintf_P(PSTR("S0=%d\r\n"), systemConf.S0);
        xprintf_P(PSTR("S100=%d\r\n"), systemConf.S100);
    }
    
    if (debug) {
        xprintf_P(PSTR(">>Ajustes_fotometricos: END\r\n"));
    }    
}
//------------------------------------------------------------------------------
int16_t compare( const void* a, const void* b)
{
   int16_t int_a = * ( (int16_t*) a );
   int16_t int_b = * ( (int16_t*) b );

   // an easy expression for comparing
   return (int_a > int_b) - (int_a < int_b);
}
//------------------------------------------------------------------------------
void cbk_medicion(void)
{
    /*
     * Ejecuta un ciclo (3) medidas
     * 
     */
    
uint8_t ciclo;
float denom;
//uint16_t l_adc[CICLOS_MEDIDA];
bool debug = actionCB.debug;
uint8_t i;
float tmp = 0.0;

    if(debug) {
        xprintf_P(PSTR(">>Medicion: START\r\n"));
    }

    fsm_telepronter("Medicion");    
    
    //systemVars.absorbancia = 0.0;
    
    // Prendo el OPTO
    opto_on(debug);
    await(1);
    // Espero 5 que se estabilize la señal fotometrica
    //vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    
    for (ciclo = 0; ciclo <  CICLOS_MEDIDA; ciclo++) {
           
        // Cierro V2
        if ( ! V2_isClosed() ) {
            valve_2_close(debug);
            await(1); 
            vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
        }
        
        // Prendo M0 10s para dispensar 0.5ml de reactivo DPD
        pump_0_run(debug, T_DISPENSAR_DPD);
        // Espero
        vTaskDelay( ( TickType_t)( (T_DISPENSAR_DPD * 1000) / portTICK_PERIOD_MS ) );
        
        // Prendo M1 11.5 para dispensar 0.5ml de buffer
        pump_1_run(debug, T_DISPENSAR_BUFFER);
        // Espero
        vTaskDelay( ( TickType_t)( (T_DISPENSAR_BUFFER * 1000) / portTICK_PERIOD_MS ) );
        
        // Prendo M2  para dispensar 10mL de muestra
        pump_2_run(debug, T_LLENADO_CELDA );
        // Espero
        vTaskDelay( ( TickType_t)( (T_LLENADO_CELDA *1000) / portTICK_PERIOD_MS ) );
    
        // Leo la señal fotometrica
        adc_read(debug, 128);
        await(1);
        systemVars.l_adc[ciclo] = adcCB.result;
        if (debug) {
            xprintf_P(PSTR("Ciclo=%d, ADC=%d\r\n"), ciclo, systemVars.l_adc[ciclo]);
        }
        
        // Abro V2 para vaciar la celda de reacción
        if ( V2_isClosed() ) {
            valve_2_open(debug);
            vTaskDelay( ( TickType_t)( (T_VACIADO_CELDA * 1000) / portTICK_PERIOD_MS ) );
        }
    }

    // Apago el OPTO
    opto_off(debug);
    await(1);
    
    /*
     * Calculo la absorbancia:
     * Ordeno los datos y me quedo con las (n-1) mejores medidas
     */
    
    // Ordeno
    qsort( systemVars.l_adc, CICLOS_MEDIDA, sizeof(int16_t), compare );
    for(i=0; i<CICLOS_MEDIDA; i++) {
        xprintf_P(PSTR("%d->%d\r\n"), i, systemVars.l_adc[i]);
    }

    // Promedio los N-1 mejores.
    // Como los ordene de menor a mayor, elimino el mayor !!!
    tmp = 0.0;
    for(i=0; i<(CICLOS_MEDIDA - 1); i++) {
        tmp += (float)(systemVars.l_adc[i]);
        xprintf_P(PSTR("Prom(%d): tmp=%0.3f\r\n"), i, tmp);
    }
    tmp /= (CICLOS_MEDIDA - 1);
    
    xprintf_P(PSTR("tmp=%0.3f\r\n"), tmp);
    
    // Calculamos la absorbancia
    denom = (1.0*(systemConf.S100 - systemConf.S0));
    if ( denom != 0 ) {
        systemVars.absorbancia = -1.0 * log10f ( 1.0*( tmp - systemConf.S0)/denom );   
    } else {
        systemVars.absorbancia = -1;    
    }
      
    systemVars.cloro_ppm = cloro_from_absorbancia(systemVars.absorbancia, debug);
    
    // Agrego el timestamp de cuando tome la medida
    RTC_rtc2strplain(&systemVars.ts_date[0], &systemVars.ts_time[0]);
   
    xprintf_P(PSTR("Abs=%0.3f\r\n"), systemVars.absorbancia);
    xprintf_P(PSTR("CLppm=%0.3f\r\n"), systemVars.cloro_ppm);
    
    if (debug) {
        xprintf_P(PSTR(">>Medicion: END\r\n"));
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
    for (i=0; i < CICLOS_LAVADO; i++) {
        
        if ( ! V2_isClosed() ) {
            // Cierro V2
            valve_2_close(debug);
            await(1); 
            vTaskDelay( ( TickType_t)( 3000 / portTICK_PERIOD_MS ) );
        }
        
        // Prendo M2 para llenar celda de reacción con muestra
        pump_2_run(debug, T_LLENADO_CELDA);
        // Espero
        vTaskDelay( ( TickType_t)( (T_LAVADO_CELDA*1000) / portTICK_PERIOD_MS ) );
    
        // Abro V2
        if ( V2_isClosed() ) {
            valve_2_open(debug);
            await(1); 
            // Espero que se vacie
            vTaskDelay( ( TickType_t)( (T_VACIADO_CELDA*1000) / portTICK_PERIOD_MS ) );
        }
    }

    // Dejo abierta V2
    if ( V2_isClosed() ) {
        valve_2_open(debug);
        await(1); 
    }
       
    // Abro V1 para purgar reservorio
    if ( V1_isClosed() ) {
        valve_1_open(debug);
        await(1); 
        // Espero que se vacie
        vTaskDelay( ( TickType_t)( (T_VACIADO_RESERVORIO * 1000) / portTICK_PERIOD_MS ) );
    }
        
    // Cierro V1 
    if ( ! V1_isClosed() ) {
        valve_1_close(debug);
        await(1); 
    }
        
    // Cierro V2
    if ( ! V2_isClosed() ) {
        valve_2_close(debug);
        await(1); 
    }
        
    vTaskDelay( ( TickType_t)( 3000 / portTICK_PERIOD_MS ) );
 
    if (debug) {
        xprintf_P(PSTR(">>Lavado_final: END\r\n"));
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
    if ( ! V0_isClosed() ) {
        valve_0_close(debug);
        await(1);
    }
    
    // Abro V1: Vacio el reservorio (por las dudas )
    if ( V1_isClosed() ) {
        valve_1_open(debug);
        await(1);
    }
    
    // Abro V2: Vacio el canal de muestra (por las dudas)
    if ( V2_isClosed() ) {
        valve_2_open(debug);
        await(1);
    }
    
    if (debug) {
        xprintf_P(PSTR(">>Fin_sistema: END\r\n"));
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
    if ( ! V2_isClosed() ) {
        valve_2_close(debug);
        await(1);  
        // Espero para que no se escape el agua
        vTaskDelay( ( TickType_t)( 3000 / portTICK_PERIOD_MS ) );
    }
    
    // Prendo M2 para llenar la celda con muestra
    // El agua debe superar el nivel del led (10mL)
    pump_2_run(debug, T_LLENADO_CELDA);
    // Espero que purge
    vTaskDelay( ( TickType_t)( (T_LLENADO_CELDA * 1000) / portTICK_PERIOD_MS ) );
    // La bomba se apaga sola 
  
    if (debug) {
        xprintf_P(PSTR(">>Llenar celda medida: END\r\n"));
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
    if ( V2_isClosed() ) {
        valve_2_open(debug);
        await(1);  
        vTaskDelay( ( TickType_t)( ( T_VACIADO_CELDA * 1000 ) / portTICK_PERIOD_MS ) );
    }
    
    if (debug) {
        xprintf_P(PSTR(">>Vaciar celda medida: END\r\n"));
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
        xprintf_P(PSTR(">>Llenado_con_reactivos: START\r\n"));
    }
    
    fsm_telepronter("Reactivos");
     
    // Apago el opto
    opto_off(debug);
    await(1);
     
    // Cierro V0: No entra agua de muestra
    if ( ! V0_isClosed() ) {
        valve_0_close(debug);
        await(1);
    }
    
    if ( ! V1_isClosed() ) {
        valve_1_close(debug);
        await(1);
    }
    
    // Abro V2: Vacio el canal de muestra (por las dudas)
    if ( V2_isClosed() ) {
        valve_2_open(debug);
        await(5);
    }
    
    if ( ! V2_isClosed() ) {
        valve_2_close(debug);
        await(1);
        vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
    }
    
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
        xprintf_P(PSTR(">>Llenado_con_reactivos: END\r\n"));
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
  
    cbk_inicio_sistema();
    cbk_llenado_reservorio_con_muestra_a_medir();
    // Hacemos 2 lavados previos
    cbk_lavado_celda();
    cbk_lavado_celda();
    //
    // Solicito al operario colocar el patrón
    xprintf_P(PSTR("***************************************\r\n"));
    xprintf_P(PSTR("COLOCAR PATRON DE CALIBRACION EN M2 !!!\r\n"));
    xprintf_P(PSTR("***************************************\r\n"));
    xprintf_P(PSTR("Espera 20 s...\r\n"));
    vTaskDelay( ( TickType_t)( 20000 / portTICK_PERIOD_MS ) );
    //
    
    // PURGA DE TUBOS
    if (debug) {
        xprintf_P(PSTR("Purga de tubo de M2\r\n"));
    }
    // Abro V2
    if ( V2_isClosed() ) {
        valve_2_open(debug);
        await(1);  
        // Espero para que no se escape el agua
        vTaskDelay( ( TickType_t)( 3000 / portTICK_PERIOD_MS ) );
    }
    pump_2_run(debug, T_PURGA_TUBO);
    // Espero que purge
    vTaskDelay( ( TickType_t)( (T_PURGA_TUBO * 1000) / portTICK_PERIOD_MS ) );
    // La bomba se apaga sola 
    //
    // Ajustes fotométricos
    cbk_ajustes_fotometricos();
    // Medida
    cbk_medicion();
    xprintf_P(PSTR("********************************\r\n"));
    xprintf_P(PSTR("COLOCAR AGUA DE LAVADO EN M2 !!!\r\n"));
    xprintf_P(PSTR("********************************\r\n"));
    xprintf_P(PSTR("Espera 20 s...\r\n"));
    vTaskDelay( ( TickType_t)( 20000 / portTICK_PERIOD_MS ) );
    
    // Hacemos 2 lavados al final
    cbk_lavado_celda();
    cbk_lavado_celda();
    
    cbk_fin_sistema();
    
    xprintf_P(PSTR("*********************************************************\r\n"));
    xprintf_P(PSTR("AJUSTAR CURVA DE CALIBRACION CON VALOR DE ABSORBANCIA !!!\r\n"));
    xprintf_P(PSTR("*********************************************************\r\n"));
    
    fsm_set_keypressed(KEY_DUMMY1);
     
    if (debug) {
        xprintf_P(PSTR(">>Calibrar: END\r\n"));
    }
    
    systemVars.midiendo = false; 
 
 
}
//------------------------------------------------------------------------------
void cbk_medida_completa(void)
{
    /*
     * Realiza todo el ciclo de medida
     */
    
 bool debug = actionCB.debug;
 uint8_t i;
 
    if (debug) {
        xprintf_P(PSTR(">>Medida Completa: START\r\n"));
    }
    
    fsm_display_modo_medir();
    fsm_telepronter("Ciclo medida");

    cbk_inicio_sistema();
    cbk_lavado_reservorio_de_muestra();
    cbk_llenado_reservorio_con_muestra_a_medir();
    
    // Hacemos 2 lavados antes
    for (i=0; i<LAVADOS_CELDA_PRE; i++) {
        cbk_lavado_celda();
    }
    
    cbk_ajustes_fotometricos();
    cbk_medicion();
    
    // Hacemos 2 lavados al final
    for (i=0; i<LAVADOS_CELDA_POST; i++) {
        cbk_lavado_celda();
    }
    
    cbk_fin_sistema();
    
    fsm_set_keypressed(KEY_DUMMY1);
     
    if (debug) {
        xprintf_P(PSTR(">>Medida Completa: END\r\n"));
    }
    
    systemVars.midiendo = false; 
    
    
}
//------------------------------------------------------------------------------
/*
 * ACCIONES BASICAS QUE SE REALIZAN SOBRE LOS PERIFERICOS
 */
//------------------------------------------------------------------------------
void await(uint16_t secs )
{
    while ( secs-- > 0  ) {
        vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
    }
}
//------------------------------------------------------------------------------
