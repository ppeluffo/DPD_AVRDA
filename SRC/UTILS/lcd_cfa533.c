
#include "lcd_cfa533.h"

lcd_packet_t TX_pkt;

const uint16_t crcLookupTable[] PROGMEM = {
    0x00000,0x01189,0x02312,0x0329B,0x04624,0x057AD,0x06536,0x074BF,
    0x08C48,0x09DC1,0x0AF5A,0x0BED3,0x0CA6C,0x0DBE5,0x0E97E,0x0F8F7,
    0x01081,0x00108,0x03393,0x0221A,0x056A5,0x0472C,0x075B7,0x0643E,
    0x09CC9,0x08D40,0x0BFDB,0x0AE52,0x0DAED,0x0CB64,0x0F9FF,0x0E876,
    0x02102,0x0308B,0x00210,0x01399,0x06726,0x076AF,0x04434,0x055BD,
    0x0AD4A,0x0BCC3,0x08E58,0x09FD1,0x0EB6E,0x0FAE7,0x0C87C,0x0D9F5,
    0x03183,0x0200A,0x01291,0x00318,0x077A7,0x0662E,0x054B5,0x0453C,
    0x0BDCB,0x0AC42,0x09ED9,0x08F50,0x0FBEF,0x0EA66,0x0D8FD,0x0C974,
    0x04204,0x0538D,0x06116,0x0709F,0x00420,0x015A9,0x02732,0x036BB,
    0x0CE4C,0x0DFC5,0x0ED5E,0x0FCD7,0x08868,0x099E1,0x0AB7A,0x0BAF3,
    0x05285,0x0430C,0x07197,0x0601E,0x014A1,0x00528,0x037B3,0x0263A,
    0x0DECD,0x0CF44,0x0FDDF,0x0EC56,0x098E9,0x08960,0x0BBFB,0x0AA72,
    0x06306,0x0728F,0x04014,0x0519D,0x02522,0x034AB,0x00630,0x017B9,
    0x0EF4E,0x0FEC7,0x0CC5C,0x0DDD5,0x0A96A,0x0B8E3,0x08A78,0x09BF1,
    0x07387,0x0620E,0x05095,0x0411C,0x035A3,0x0242A,0x016B1,0x00738,
    0x0FFCF,0x0EE46,0x0DCDD,0x0CD54,0x0B9EB,0x0A862,0x09AF9,0x08B70,
    0x08408,0x09581,0x0A71A,0x0B693,0x0C22C,0x0D3A5,0x0E13E,0x0F0B7,
    0x00840,0x019C9,0x02B52,0x03ADB,0x04E64,0x05FED,0x06D76,0x07CFF,
    0x09489,0x08500,0x0B79B,0x0A612,0x0D2AD,0x0C324,0x0F1BF,0x0E036,
    0x018C1,0x00948,0x03BD3,0x02A5A,0x05EE5,0x04F6C,0x07DF7,0x06C7E,
    0x0A50A,0x0B483,0x08618,0x09791,0x0E32E,0x0F2A7,0x0C03C,0x0D1B5,
    0x02942,0x038CB,0x00A50,0x01BD9,0x06F66,0x07EEF,0x04C74,0x05DFD,
    0x0B58B,0x0A402,0x09699,0x08710,0x0F3AF,0x0E226,0x0D0BD,0x0C134,
    0x039C3,0x0284A,0x01AD1,0x00B58,0x07FE7,0x06E6E,0x05CF5,0x04D7C,
    0x0C60C,0x0D785,0x0E51E,0x0F497,0x08028,0x091A1,0x0A33A,0x0B2B3,
    0x04A44,0x05BCD,0x06956,0x078DF,0x00C60,0x01DE9,0x02F72,0x03EFB,
    0x0D68D,0x0C704,0x0F59F,0x0E416,0x090A9,0x08120,0x0B3BB,0x0A232,
    0x05AC5,0x04B4C,0x079D7,0x0685E,0x01CE1,0x00D68,0x03FF3,0x02E7A,
    0x0E70E,0x0F687,0x0C41C,0x0D595,0x0A12A,0x0B0A3,0x08238,0x093B1,
    0x06B46,0x07ACF,0x04854,0x059DD,0x02D62,0x03CEB,0x00E70,0x01FF9,
    0x0F78F,0x0E606,0x0D49D,0x0C514,0x0B1AB,0x0A022,0x092B9,0x08330,
    0x07BC7,0x06A4E,0x058D5,0x0495C,0x03DE3,0x02C6A,0x01EF1,0x00F78
};

bool awaiting_response = false;

void FSM_processLCDrx(uint8_t c)
{
    /*
     * Se invoca desde la tarea de recepcion del LCD
     * Solo lo uso para analizar las teclas que se presionaron
     * por lo que solo debo analizar el primer byte.
     */
    
uint16_t ptr;
    
    // Guardo el byte en el rx buffer.
    if ( ! lBchar_Put( &lcd_rx_lbuffer, c) ) {
        // Si el buffer esta lleno, la borro !!!!
        lBchar_Flush(&lcd_rx_lbuffer);
    }

    ptr = lBchar_GetCount(&lcd_rx_lbuffer);
    
    if (ptr == 1)  {
        switch( lcd_rx_buffer[0]) {
            case 0x40:
                // ping response.
                //xprintf_P(PSTR("PING response\r\n"));
                awaiting_response = false;
                break;
            case 0x46:
                // Clear response
                //xprintf_P(PSTR("CLEAR response\r\n"));
                awaiting_response = false;
                break; 
            case 0x5F:
                // Data response
                //xprintf_P(PSTR("DATA response\r\n"));
                awaiting_response = false;
                break;
        }
        return;
    }
   
    if (ptr < 5) {
        return;
    }
        
    if (ptr == 5) {
        // Key activity (frame completo):        
        if (lcd_rx_buffer[0] == 0x80 ) {
          
            switch(lcd_rx_buffer[2]) {
                case KEY_UP_PRESS:
                    //xprintf_P(PSTR("KEY_UP_PRESS\r\n"));
                    break;
                case KEY_DOWN_PRESS:
                    //xprintf_P(PSTR("KEY_DOWN_PRESS\r\n"));
                    break;
                case KEY_LEFT_PRESS:
                    //xprintf_P(PSTR("KEY_LEFT_PRESS\r\n"));
                    break;
                case KEY_RIGHT_PRESS:
                    //xprintf_P(PSTR("KEY_RIGHT_PRESS\r\n"));
                    break;
                case KEY_ENTER_PRESS:
                    //xprintf_P(PSTR("KEY_ENTER_PRESS\r\n"));
                    break;
                case KEY_EXIT_PRESS:
                    //xprintf_P(PSTR("KEY_EXIT_PRESS\r\n"));
                    break;
                case KEY_UP_RELEASE:
                    //xprintf_P(PSTR("KEY_UP_RELEASE\r\n"));
                    break;
                case KEY_DOWN_RELEASE:
                    //xprintf_P(PSTR("KEY_DOWN_RELEASE\r\n"));
                    break;
                case KEY_LEFT_RELEASE:
                    //xprintf_P(PSTR("KEY_LEFT_RELEASE\r\n"));
                    break;
                case KEY_RIGHT_RELEASE:
                    //xprintf_P(PSTR("KEY_RIGHT_RELEASE\r\n"));
                    break;
                case KEY_ENTER_RELEASE:
                    //xprintf_P(PSTR("KEY_ENTER_RELEASE\r\n"));
                    break;
                case KEY_EXIT_RELEASE:
                    //xprintf_P(PSTR("KEY_EXIT_RELEASE\r\n"));
                    break;
            }
            
            // Aviso a FSM display que llego una tecla
            fsm_set_keypressed(lcd_rx_buffer[2]);
            
            lBchar_Flush(&lcd_rx_lbuffer);
        }
    }
    
}
//------------------------------------------------------------------------------
void lcd_flush_rx_buffer(void)
{
    lBchar_Flush(&lcd_rx_lbuffer);
}
//------------------------------------------------------------------------------
void lcd_send_pkt( lcd_packet_t *pkt)
{
    /*
     * Calcula el CRC del paquete
     * Armamos el TX buffer(lineal) que vamos a transmitir.
     */
    
uint8_t i;
uint16_t crc;
uint8_t idx;
        
    awaiting_response = true;
    
    // Preparo el buffer con los datos del paquete
    memset(&lcd_tx_buffer, '\0', LCD_TX_BUFFER_SIZE );
    lcd_tx_buffer[0] = pkt->type;
    lcd_tx_buffer[1] = pkt->data_length;
    
    //memcpy(&display_tx_buffer[2], pkt->data, strlen(pkt->data));
    for (i=0; i < pkt->data_length; i++) {
        lcd_tx_buffer[2+i] = pkt->data[i];
    }
        
    crc = CRC16( &lcd_tx_buffer[0], (2 + pkt->data_length) );
    //xprintf_P(PSTR("CRC=0x%04x\r\n"), crc);
 
    // Calculo el CRC
    idx = 2 + pkt->data_length;
    
    lcd_tx_buffer[idx++] = (uint8_t)( crc & 0x00FF );           // CRC Low
    lcd_tx_buffer[idx++] = (uint8_t)( (crc & 0xFF00) >> 8 );	// CRC High
    
    
    // Muestro  
    //xprintf_P(PSTR("LCD TXBUFFER (%d):\r\n"), idx);
    
    //for(i=0; i<idx; i++) {
    //    xprintf_P(PSTR("[0x%02X]"), lcd_tx_buffer[i]);
    //}
    //xprintf_P(PSTR("\r\n"));
    
    // Transmito
    // Como pueden haber 0x00, debo usar xnprintf !!!
    lcd_flush_rx_buffer();
    // La funcion xnprintf_MBUS maneja el control de flujo.
	i = xnprintf( fdLCD, &lcd_tx_buffer[0], idx );
    vTaskDelay( ( TickType_t)( 250 / portTICK_PERIOD_MS ) );

}
//------------------------------------------------------------------------------
bool lcd_check_response(uint8_t rsp_code)
{
    uint8_t i;
    
    xprintf_P(PSTR("LCD RXBUFFER: "));
    for (i=0; i < LCD_RX_BUFFER_SIZE; i++) {
        xprintf_P(PSTR("[0x%02X]"), lcd_rx_buffer[i]);
    }
    xprintf_P(PSTR("\r\n"));
    
    return (true);
}
//------------------------------------------------------------------------------
bool lcd_cmd_ping(void)
{
    
    // Armo el paquete de PING
    TX_pkt.type = 0x00;
    TX_pkt.data_length = 0x00;
    TX_pkt.data[0] = '\0';
   
    lcd_send_pkt(&TX_pkt);
    
    //retS = lcd_check_response(0x40);
    //return retS;
    lcd_flush_rx_buffer();
    return (true);
}
//------------------------------------------------------------------------------
bool lcd_cmd_clear(void)
{
    // Armo el paquete de CLEAR SCREEN

    TX_pkt.type = 0x06;
    TX_pkt.data_length = 0x00;
    TX_pkt.data[0] = '\0';
   
    lcd_send_pkt(&TX_pkt);
    
    //retS = lcd_check_response(0x46);
    //return retS;   
    lcd_flush_rx_buffer();
    return (true);
}
//------------------------------------------------------------------------------
bool lcd_cmd_send_data(uint8_t row, uint8_t col, uint8_t *msg)
{
    // Envio datos a una linea !!

    memset( &TX_pkt, '\0', sizeof(TX_pkt));

    TX_pkt.type = 0x1F;
    TX_pkt.data[0] = col;
    TX_pkt.data[1] = row;
    memcpy(&TX_pkt.data[2], msg, strlen(msg));
    //xprintf_P(PSTR("MSG=%s, msg_length=%d\r\n"),msg, strlen(msg));
    TX_pkt.data_length = 2 + strlen(msg);
    //xprintf_P(PSTR("DATA_LENGTH=%d\r\n"), TX_pkt.data_length );

    lcd_send_pkt(&TX_pkt);
    
    //retS = lcd_check_response(0x5F);
    //return retS; 
    lcd_flush_rx_buffer();
    return (true);    

}
//------------------------------------------------------------------------------
bool lcd_cmd_brightness(uint8_t brillo)
{

    TX_pkt.type = 0x0E;
    TX_pkt.data_length = 0x01;
    TX_pkt.data[0] = brillo;
   
    lcd_send_pkt(&TX_pkt);
     
    //retS = lcd_check_response(0x4E);
    //return retS;   
    lcd_flush_rx_buffer();
    return (true);
}
//------------------------------------------------------------------------------
bool lcd_cmd_read(void)
{
    uint8_t i;
    
    xprintf_P(PSTR("LCD RXBUFFER: "));
    for (i=0; i < LCD_RX_BUFFER_SIZE; i++) {
        xprintf_P(PSTR("[0x%02X]"), lcd_rx_buffer[i]);
    }
    xprintf_P(PSTR("\r\n"));
    
    return (true);
    
}
//-----------------------------------------------------------------------------
bool lcd_cmd_clearbuff(void)
{
    lcd_flush_rx_buffer();
    return(true);
}
//-----------------------------------------------------------------------------
uint16_t CRC16(uint8_t *bufptr, uint16_t len) 
{
uint16_t newCrc;
uint16_t idx;
uint16_t seed;

    newCrc=0xFFFF;
    //This algorithm is based on the IrDA LAP example.
    while(len--) {
        idx = (newCrc ^ *bufptr++) & 0xff;
        seed = (PGM_P) pgm_read_word_far ( &crcLookupTable[idx] );
        newCrc = (newCrc >> 8) ^ seed;
    }

    //Make this crc match the one?s complement that is sent in the packet.
    return(~newCrc);
}
//------------------------------------------------------------------------------

