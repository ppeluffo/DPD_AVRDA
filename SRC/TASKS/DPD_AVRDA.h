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

#define FW_REV "1.0.0"
#define FW_DATE "@ 20241007"
#define HW_MODELO "DPD_AVRDA FRTOS R001 HW:AVR128DA64"
#define FRTOS_VERSION "FW:FreeRTOS V202111.00"
#define FW_TYPE "DPD"

#define SYSMAINCLK 24

#define tkCtl_TASK_PRIORITY	 	( tskIDLE_PRIORITY + 1 )
#define tkCmd_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )
#define tkSys_TASK_PRIORITY 	( tskIDLE_PRIORITY + 1 )

#define tkCtl_STACK_SIZE		384
#define tkCmd_STACK_SIZE		384
#define tkSys_STACK_SIZE		384

StaticTask_t tkCtl_Buffer_Ptr;
StackType_t tkCtl_Buffer [tkCtl_STACK_SIZE];

StaticTask_t tkCmd_Buffer_Ptr;
StackType_t tkCmd_Buffer [tkCmd_STACK_SIZE];

StaticTask_t tkSys_Buffer_Ptr;
StackType_t tkSys_Buffer [tkSys_STACK_SIZE];

SemaphoreHandle_t sem_SYSVars;
StaticSemaphore_t SYSVARS_xMutexBuffer;
#define MSTOTAKESYSVARSSEMPH ((  TickType_t ) 10 )

TaskHandle_t xHandle_tkCtl, xHandle_tkCmd, xHandle_tkSys;

void tkCtl(void * pvParameters);
void tkCmd(void * pvParameters);
void tkSys(void * pvParameters);

bool starting_flag;

// Estructura que tiene el valor de las medidas en el intervalo de poleo

#define CAL_MAX_POINTS   10

struct {   
    bool debug;
    float adc_cal_volts;
    uint16_t adc_cal_factor;
    uint16_t pump_freq[MAX_PUMPS];
    float xCal[CAL_MAX_POINTS];     // Absorbancias
    float yCal[CAL_MAX_POINTS];     // Concentracion de cloro
    
    uint8_t checksum;
} systemVars;

void system_init();
void reset(void);
bool u_save_config_in_NVM(void);
bool u_load_config_from_NVM(void);
void u_config_default(void);

void SYSTEM_ENTER_CRITICAL(void);
void SYSTEM_EXIT_CRITICAL(void);
void u_kick_wdt( uint8_t wdg_gc);

uint8_t sys_watchdog;
uint8_t task_running;

#define CMD_WDG_bp     0x01
#define SYS_WDG_bp     0x02

#define CMD_WDG_gc          (0x01 << 0)
#define SYS_WDG_gc          (0x01 << 1)

struct {
    void (*fn)(void);
    bool standby;
    bool emergency_exit;
} actionCB;

#define CICLOS_MEDIDA            3
#define T_LLENADO_RESERVORIO_MAX    15
#define CNT_LLENADO_RESERVORIO  35000
#define T_PURGA_CANAL           15
#define T_VACIADO_RESERVORIO    20
#define T_LAVADO_CELDA          20
#define T_LLENADO_CELDA         25  // 10 mL
#define T_VACIADO_CELDA         10
#define T_DISPENSAR_DPD         20  //10
#define T_DISPENSAR_BUFFER      20  //12
#define CICLOS_LAVADO            4

uint16_t S0, S100;
float absorbancia;
float cloro_ppm;

void action_await(void);;

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

float cloro_from_absorbancia(float abs, bool debug);


// No habilitado PLT_WDG !!!
#define WDG_bm 0x03     // Pone todos los bits habilitados en 1
#define WDG_INIT() ( sys_watchdog = WDG_bm )

#endif	/* XC_HEADER_TEMPLATE_H */

