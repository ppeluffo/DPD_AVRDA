/* 
 * File:   display.h
 * Author: pablo
 *
 * Created on 5 de noviembre de 2024, 11:54 AM
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

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

#define LCD_TX_BUFFER_SIZE 32
uint8_t lcd_tx_buffer[LCD_TX_BUFFER_SIZE];

#define LCD_RX_BUFFER_SIZE 16
uint8_t lcd_rx_buffer[LCD_RX_BUFFER_SIZE];
lBuffer_s lcd_rx_lbuffer;

#define MAX_LCD_DATA_LENGTH  24

typedef struct {
    uint8_t type;
    uint8_t data_length;
    uint8_t data[MAX_LCD_DATA_LENGTH];
    uint8_t CRC_H;
    uint8_t CRC_L;
} lcd_packet_t;


void FSM_processLCDrx(uint8_t c);

void LCD_flush_rx_buffer(void);
bool LCD_cmd_ping(void);
bool LCD_cmd_clear(void);
bool LCD_cmd_send_data(uint8_t row, uint8_t col, uint8_t *msg);
bool lcd_cmd_read(void);

#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAY_H */

