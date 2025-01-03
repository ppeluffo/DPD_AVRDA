/* 
 * File:   lte.h
 * Author: pablo
 *
 * Created on 25 de enero de 2024, 10:23 AM
 */

#ifndef LTE_H
#define	LTE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include "stdbool.h"
#include "linearBuffer.h"
#include "frtos-io.h"
#include "xprintf.h"
#include "string.h"
#include "xgetc.h"
#include "ctype.h"
//--------------------------------------------------------------------------
// PUSR WH-LTE-7S1

// LTE_PWR
#define LTE_PWR_PORT         PORTG
#define LTE_PWR              2
#define LTE_PWR_PIN_bm       PIN2_bm
#define LTE_PWR_PIN_bp       PIN2_bp
#define CLEAR_LTE_PWR()     ( LTE_PWR_PORT.OUT |= LTE_PWR_PIN_bm )
#define SET_LTE_PWR()       ( LTE_PWR_PORT.OUT &= ~LTE_PWR_PIN_bm )
#define CONFIG_LTE_PWR()     LTE_PWR_PORT.DIR |= LTE_PWR_PIN_bm;

// LTE_EN_VCAP
#define LTE_EN_VCAP_PORT         PORTG
#define LTE_EN_VCAP              3
#define LTE_EN_VCAP_PIN_bm       PIN3_bm
#define LTE_EN_VCAP_PIN_bp       PIN3_bp
#define SET_LTE_EN_VCAP()        ( LTE_EN_VCAP_PORT.OUT |= LTE_EN_VCAP_PIN_bm )
#define CLEAR_LTE_EN_VCAP()      ( LTE_EN_VCAP_PORT.OUT &= ~LTE_EN_VCAP_PIN_bm )
#define CONFIG_LTE_EN_VCAP()     LTE_EN_VCAP_PORT.DIR |= LTE_EN_VCAP_PIN_bm;

    // LTE_RELOAD
#define LTE_RELOAD_PORT         PORTG
#define LTE_RELOAD              4
#define LTE_RELOAD_PIN_bm       PIN4_bm
#define LTE_RELOAD_PIN_bp       PIN4_bp
#define CLEAR_LTE_RELOAD()      ( LTE_RELOAD_PORT.OUT |= LTE_RELOAD_PIN_bm )
#define SET_LTE_RELOAD()        ( LTE_RELOAD_PORT.OUT &= ~LTE_RELOAD_PIN_bm )
#define CONFIG_LTE_RELOAD()     LTE_RELOAD_PORT.DIR |= LTE_RELOAD_PIN_bm;
 
// LTE_RESET
#define LTE_RESET_PORT         PORTG
#define LTE_RESET              5
#define LTE_RESET_PIN_bm       PIN5_bm
#define LTE_RESET_PIN_bp       PIN5_bp
#define CLEAR_LTE_RESET()      ( LTE_RESET_PORT.OUT |= LTE_RESET_PIN_bm )
#define SET_LTE_RESET()        ( LTE_RESET_PORT.OUT &= ~LTE_RESET_PIN_bm )
#define CONFIG_LTE_RESET()     LTE_RESET_PORT.DIR |= LTE_RESET_PIN_bm;

#define WAN_TX_BUFFER_SIZE 255
char wan_tx_buffer[WAN_TX_BUFFER_SIZE];

#define MODEM_RX_BUFFER_SIZE 512
char modem_rx_buffer[MODEM_RX_BUFFER_SIZE];
lBuffer_s modem_rx_lbuffer;

#define LTE_BUFFER_SIZE 255
char lte_buffer[LTE_BUFFER_SIZE];
lBuffer_s lte_lbuffer;

void LTE_process_buffer( char c);

typedef enum { LTE_PWR_OFF=0, LTE_PWR_ON} t_lte_pwr_status;
    
typedef enum { BAUD9600=0, BAUD115200 } t_lte_baudrate;

#define APN_LENGTH  24
#define SERVER_IP_LENGTH 24
#define SERVER_PORT_LENGTH 12

typedef struct {
    char apn[APN_LENGTH];
	char server_ip[SERVER_IP_LENGTH];
	char server_port[SERVER_PORT_LENGTH];
} modem_conf_t;

modem_conf_t modem_conf;

t_lte_pwr_status LTE_get_pwr_status(void);
char *LTE_buffer_ptr(void);
void LTE_flush_buffer(void);
void lte_test_link(void);

void MODEM_init(void);
void MODEM_prender(void);
void MODEM_apagar(void);
int MODEM_txmit( char *tx_buffer[] );
char *MODEM_get_buffer_ptr(void);
void MODEM_flush_rx_buffer(void);
bool MODEM_enter_mode_at(bool verbose);
void MODEM_exit_mode_at(bool verbose);

char *modem_atcmd(char *s_cmd, bool verbose);
void modem_atcmd_save(bool verbose);
void modem_atcmd_queryall(void);
void modem_atcmd_read_iccid(bool verbose);
char *modem_atcmd_get_iccid(void);
void modem_atcmd_read_imei(bool verbose);
char *modem_atcmd_get_imei(void);
void modem_atcmd_read_csq(bool verbose);
uint8_t modem_atcmd_get_csq(void);
char *modem_atcmd_get_apn(void);
void modem_atcmd_read_apn(bool verbose);
void modem_atcmd_set_apn( char *apn, bool verbose);
void modem_atcmd_set_server( char *ip, char *port, bool verbose);
char *modem_atcmd_get_server_ip(void);
char *modem_atcmd_get_server_port(void);
void modem_atcmd_read_server(bool verbose);
void modem_atcmd_set_apiurl( char *apiurl, bool verbose);
void modem_atcmd_set_ftime( char *time_ms, bool verbose);

bool modem_check_and_reconfig(bool verbose);

void modem_print_configuration( void );
bool modem_config( char *s_arg, char *s_value );
void modem_config_defaults( char *s_arg );




#ifdef	__cplusplus
}
#endif

#endif	/* LTE_H */

