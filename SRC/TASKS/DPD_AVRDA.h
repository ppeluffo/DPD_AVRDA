/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#ifndef F_CPU
#define F_CPU 24000000
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "croutine.h"
#include "semphr.h"
#include "timers.h"
#include "limits.h"
#include "portable.h"
#include "protected_io.h"
#include "ccp.h"

//#include <avr/io.h>
//#include <avr/builtins.h>
#include <avr/wdt.h> 
//#include <avr/pgmspace.h>
//#include <avr/fuse.h>
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
//#include "math.h"

#include "frtos-io.h"
#include "xprintf.h"
#include "xgetc.h"
#include "eeprom.h"
#include "nvm.h"
#include "led.h"
#include "pines.h"
#include "linearBuffer.h"
#include "fileSystem.h"
#include "i2c.h"
#include "rtc79410.h"
#include "contadores.h"
#include "toyi_valves.h"
#include "tmc2209.h"
#include "opto.h"
#include "adc.h"
#include "modem_lte.h"
#include "lcd_cfa533.h"

#define FW_REV "1.0.0"
#define FW_DATE "@ 20241119"
#define HW_MODELO "DPD_AVRDA FRTOS R001 HW:AVR128DA64"
#define FRTOS_VERSION "FW:FreeRTOS V202111.00"
#define FW_TYPE "DPD"

#define SYSMAINCLK 24

#define tkCtl_TASK_PRIORITY	 	( tskIDLE_PRIORITY + 1 )
#define tkCmd_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )
#define tkSys_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )
#define tkModemRX_TASK_PRIORITY	( tskIDLE_PRIORITY + 1 )
#define tkWan_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )
#define tkLcdRX_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )
#define tkLcd_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )

#define tkCtl_STACK_SIZE		384
#define tkCmd_STACK_SIZE		384
#define tkSys_STACK_SIZE		384
#define tkModemRX_STACK_SIZE	384
#define tkWan_STACK_SIZE		512
#define tkLcdRX_STACK_SIZE      384
#define tkLcd_STACK_SIZE        384

StaticTask_t tkCtl_Buffer_Ptr;
StackType_t tkCtl_Buffer [tkCtl_STACK_SIZE];

StaticTask_t tkCmd_Buffer_Ptr;
StackType_t tkCmd_Buffer [tkCmd_STACK_SIZE];

StaticTask_t tkSys_Buffer_Ptr;
StackType_t tkSys_Buffer [tkSys_STACK_SIZE];

StaticTask_t tkModemRX_Buffer_Ptr;
StackType_t tkModemRX_Buffer [tkModemRX_STACK_SIZE];

StaticTask_t tkWan_Buffer_Ptr;
StackType_t tkWan_Buffer [tkWan_STACK_SIZE];

StaticTask_t tkLcdRX_Buffer_Ptr;
StackType_t tkLcdRX_Buffer [tkLcdRX_STACK_SIZE];

StaticTask_t tkLcd_Buffer_Ptr;
StackType_t tkLcd_Buffer [tkLcdRX_STACK_SIZE];

TaskHandle_t xHandle_tkCtl, xHandle_tkCmd, xHandle_tkSys, xHandle_tkModemRX, xHandle_tkWan, xHandle_tkLcdRX, xHandle_tkLcd;

SemaphoreHandle_t sem_SYSVars;
StaticSemaphore_t SYSVARS_xMutexBuffer;
#define MSTOTAKESYSVARSSEMPH ((  TickType_t ) 10 )

void tkCtl(void * pvParameters);
void tkCmd(void * pvParameters);
void tkSys(void * pvParameters);
void tkModemRX(void * pvParameters);
void tkWan(void * pvParameters);
void tkLcdRX(void * pvParameters);
void tkLcd(void * pvParameters);

bool starting_flag;

// Estructura que tiene el valor de las medidas en el intervalo de poleo

#define CAL_MAX_POINTS      10

#define DLGID_LENGTH		12

// Mensajes entre tareas
#define SIGNAL_FRAME_READY		0x01

#define TIMESTAMP_SIZE 8

typedef struct { 
    float cloro_ppm;
    float absorbancia;
    char ts_date[TIMESTAMP_SIZE];
    char ts_time[TIMESTAMP_SIZE];
    uint32_t time2medida;
    bool midiendo;
    bool debug;
} systemVars_t;

systemVars_t systemVars;

typedef struct {
    char dlgid[DLGID_LENGTH];
    uint16_t S0, S100;
    uint16_t timerpoll;
    uint32_t timermedida;
    float adc_cal_volts;
    uint16_t adc_cal_factor;
    uint16_t pump_freq[MAX_PUMPS];
    char calibration_date[TIMESTAMP_SIZE];
    float xCal[CAL_MAX_POINTS];     // Absorbancias
    float yCal[CAL_MAX_POINTS];     // Concentracion de cloro
} systemConf_t;

systemConf_t systemConf;

// Tipo que define la estrucutra de las medidas tomadas.
typedef struct {
    float absorbancia;
    float cloro_ppm;
    uint16_t S0;
    uint16_t S100;
    char ts_date[TIMESTAMP_SIZE];
    char ts_time[TIMESTAMP_SIZE];
    float bt12v;
    RtcTimeType_t rtc;	    
} dataRcd_s;

typedef enum { LCD_INIT=0, LCD_OFF, LCD_STANDBY, LCD_CONF_MEDIDA, LCD_MIDIENDO } t_fsm_display_states;
typedef enum { PANTALLA1=0, PANTALLA2, PANTALLA3, PANTALLA4, PANTALLA_CONF_MEDIDA, PANTALLA_MIDIENDO } t_pantallas;
 
typedef struct {
    t_fsm_display_states state;
    t_pantallas pantalla;
    int8_t timerBackliteOff;
    key_t keypress;
} FSMdisplay_CB_t;

void fsm_telepronter(char *msg);
void fsm_display_modo_medir(void);

void system_init();
void reset(void);
bool u_save_config_in_NVM(void);
bool u_load_config_from_NVM(void);
void u_config_default(void);

void SYSTEM_ENTER_CRITICAL(void);
void SYSTEM_EXIT_CRITICAL(void);
void u_kick_wdt( uint8_t wdg_gc);
bool u_config_debug( char *tipo, char *valor);
void u_data_resync_clock( char *str_time, bool force_adjust);
int16_t u_days2calibrate(void);

bool WAN_process_data_rcd( dataRcd_s *dataRcd);
void WAN_config_debug(bool debug );
bool WAN_read_debug(void);
bool WAN_is_online(void);

struct {
    void (*fn)(void);
    bool standby;
    bool emergency_exit;
    bool debug;
} actionCB;

#define TIMER2MEDIDA_INICIAL    1800
#define CICLOS_MEDIDA            3
#define T_LLENADO_RESERVORIO_MAX    15
#define CNT_LLENADO_RESERVORIO  25000
#define T_PURGA_CANAL           15
#define T_VACIADO_RESERVORIO    20
#define T_LAVADO_CELDA          25
#define T_LLENADO_CELDA         20  //10 mL
#define T_VACIADO_CELDA         10
#define T_DISPENSAR_DPD         13  //0.5 mL
#define T_DISPENSAR_BUFFER      13  //0.5 mL
#define CICLOS_LAVADO            4

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
void action_lavado_calibracion(bool debug);
void action_llenado_con_reactivos(bool debug);
void action_medida_completa(bool debug);

void action_await(void);

void action_opto_on(bool debug);
void action_opto_off(bool debug);

void action_valve_0_open(bool debug);
void action_valve_0_close(bool debug);
void action_valve_1_open(bool debug);
void action_valve_1_close(bool debug);
void action_valve_2_open(bool debug);
void action_valve_2_close(bool debug);

void action_pump_0_run( bool debug, uint16_t secs);
void action_pump_0_stop( bool debug);
void action_pump_1_run( bool debug, uint16_t secs);
void action_pump_1_stop( bool debug);
void action_pump_2_run( bool debug, uint16_t secs);
void action_pump_2_stop( bool debug);

void action_adc_read( bool debug, uint16_t counts);

void proc_inicio_sistema(bool debug);
void proc_lavado_reservorio_de_muestra(bool debug);
void proc_llenado_reservorio_con_muestra_a_medir(bool debug);
void proc_purga_canal_muestra(bool debug);
void proc_lavado_celda(bool debug);
void proc_llenar_celda_medida(bool debug);
void proc_vaciar_celda_medida(bool debug);
void proc_ajustes_fotometricos(bool debug);
void proc_medicion(bool debug);
void proc_lavado_final(bool debug);
void proc_fin_sistema(bool debug);
void proc_calibrar(bool debug);
void proc_medida_completa(bool debug);
void proc_llenado_con_reactivos(bool debug);

float cloro_from_absorbancia(float abs, bool debug);

// No habilitado PLT_WDG !!!




uint8_t sys_watchdog;
uint8_t task_running;

// NO CONSIDERO SYS_WDG

#define WDG_bm 0x1F     // Pone todos los bits habilitados en 1
#define WDG_INIT() ( sys_watchdog = WDG_bm )

#define NRO_WDG      5

#define CMD_WDG_bp     0x01
#define MRX_WDG_bp     0x02
#define WAN_WDG_bp     0x03
#define LRX_WDG_bp     0x04
#define LCD_WDG_bp     0x05
#define SYS_WDG_bp     0x06

#define CMD_WDG_gc          (0x01 << 0)
#define MRX_WDG_gc          (0x01 << 1)
#define WAN_WDG_gc          (0x01 << 2)
#define LRX_WDG_gc          (0x01 << 3)
#define LCD_WDG_gc          (0x01 << 4)
#define SYS_WDG_gc          (0x01 << 5)


#endif	/* XC_HEADER_TEMPLATE_H */

