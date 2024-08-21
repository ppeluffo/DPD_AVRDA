
#include "ads1115.h"

//------------------------------------------------------------------------------

/** Read single word from a 16-bit device register.
 * @param byRegAddr Register byRegAddr to read from
 * @param pwData Container for word value read from device
 * @return esp_err_t of read operation
 */
uint16_t ads1115_readReg(uint8_t RegAddr, bool debug) 
{
  uint8_t byOut[2];
  uint16_t data;
  int16_t rcode;
  
  rcode =  I2C_read( fdI2C0, DEVADDRESS_ADS1115, RegAddr, 0x01, (char *)&byOut, 0x02, debug );
  if(rcode < 0)
  {
   xprintf("ERROR:read_I2C\r\n");  
   return 0xffff;
  }

  // first byte transmitted from ADS1115 is most significant byte from the register
  data = byOut[1] << 8 | byOut[0];  // switch bytes
  //xprintf("HB: %x\r\n",byOut[1]);
  //xprintf("LB: %x\r\n",byOut[0]);
  
  return data;
}

uint16_t ads1115_readBitInReg(uint8_t RegAddr, uint8_t BitPos, bool debug) 
{
  uint16_t wReadWord, bit_value;
  
  wReadWord = ads1115_readReg(RegAddr, debug);
  bit_value = wReadWord & (1 << BitPos);
  return (bit_value>>BitPos);
}

uint16_t ads1115_readBitsInReg(uint8_t RegAddr, uint8_t BitPosStart, uint8_t Length, bool debug)
{
  // 1101011001101001 read byte
  // fedcba9876543210 bit numbers
  //    xxx           args: byBitStart=12, byLength=3
  //    010           masked
  //           -> 010 shifted
  uint16_t wReadWord;
  uint16_t wMask;

  wReadWord = ads1115_readReg(RegAddr, debug);
    
  wMask = ((1 << Length) - 1) << (BitPosStart - Length + 1);
  wReadWord &= wMask;
  wReadWord >>= (BitPosStart - Length + 1);
  
  return wReadWord;
}

int16_t ads1115_writeReg(uint8_t RegAddr, uint16_t RegWord, bool debug) 
{
  uint8_t data[2];
  int8_t rcode;
  
  // first byte to be send goes in the most significant byte of the register
  data[1] = RegWord >> 8;    // get 8 greater bits
  data[0] = RegWord & 0xFF;  // get 8 lower bits

  rcode =  I2C_write ( fdI2C0, DEVADDRESS_ADS1115, RegAddr, 0x01, (char *)&data, 2, debug );
  if(rcode < 0)
   {
    xprintf("ERROR:write_I2C\r\n");  
    return rcode;
   }
  
  return rcode;
}

int16_t ads1115_writeBitInReg(uint8_t RegAddr, uint8_t BitPos, bool BitValue, bool debug) 
{
  uint16_t wReadWord;
  
  wReadWord = ads1115_readReg(RegAddr,debug);
  //                                    set bit                          clear bit
  wReadWord =  (BitValue != 0) ? (wReadWord | (1 << BitPos)) : (wReadWord & ~(1 << BitPos)) ;
  
  return ads1115_writeReg(RegAddr, wReadWord, debug);  
}

int16_t ads1115_writeBitsInReg(uint8_t RegAddr, uint8_t BitPosStart, uint8_t Length, uint16_t Wvalue, bool debug)
{
  //              010 value to write
  // fedcba9876543210 bit numbers
  //    xxx           args: byBitStart=12, byLength=3
  // 0001110000000000 wMask word
  // 1010111110010110 original value (sample)
  // 1010001110010110 original & ~wMask
  // 1010101110010110 masked | value
  uint16_t wReadWord, wMask;

  wReadWord = ads1115_readReg(RegAddr, debug);
  
  wMask = ((1 << Length) - 1) << (BitPosStart - Length + 1);
  Wvalue <<= (BitPosStart - Length + 1); // shift data into correct position
  Wvalue &= wMask; // zero all non-important bits in data
  wReadWord &= ~(wMask); // zero all important bits in existing word
  wReadWord |= Wvalue; // combine data with existing word
  
  return ads1115_writeReg(RegAddr, wReadWord, debug);
}


int16_t ADS1115_config_inicial(bool debug)
{
    
int16_t rcode = 0;
uint16_t Rword = 0xC383; //860SP/s - 1.2ms/SP (bits 7-5), disable ALERT/READY, OS status = 1 (bit 15)

    
	rcode = ads1115_writeReg(CONFIG_REG, Rword, debug);

	if ( rcode == -1 ) {
		// Hubo error: trato de reparar el bus y reintentar la operacion
		// Espero 1 seg a que se termine la fuente de ruido.
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
		// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
		xprintf_P(PSTR("ERROR: ADS1115_config\r\n") );
	}

	return( rcode );
}
//------------------------------------------------------------------------------
bool ADS1115_is_busy(bool debug)
{
    
int16_t rcode = 0;
uint8_t data[2];
bool busy_flag = true;

	rcode =  I2C_read( fdI2C0, DEVADDRESS_ADS1115, CONFIG_REG, 0x01, (char *)&data, 0x02, debug );

	if ( rcode == -1 ) {
		// Hubo error: trato de reparar el bus y reintentar la operacion
		// Espero 1s que se termine la fuente de ruido.
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
		// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
		xprintf_P(PSTR("ERROR: ADS1115_is_busy\r\n") );
		goto quit;
        }
    
    if ( data[1] >> 7)  {
        busy_flag = false; 
        }
   
quit:
    //xprintf("HB_config_reg: %d",data[1]);
    //xprintf("LB_config_reg: %d",data[0]);
    //xprintf_P(PSTR("is_busy\r\n") );
	return( busy_flag );   
}
//------------------------------------------------------------------------------
void ADS1115_start(bool debug)
{
  
int16_t rcode = 0;
uint8_t data[2];

    data[0] = 0x83;
    data[1] = 0xC3;
	rcode =  I2C_write ( fdI2C0, DEVADDRESS_ADS1115, CONFIG_REG, 0x01, (char *)&data, 2, debug );

	if ( rcode == -1 ) {
		// Hubo error: trato de reparar el bus y reintentar la operacion
		// Espero 1s que se termine la fuente de ruido.
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
		// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
		xprintf_P(PSTR("ERROR: ADS1115_start\r\n") );
	}
   
}
//------------------------------------------------------------------------------
uint16_t ADS1115_readConvReg(bool debug)
{
    
uint16_t rcode = 0; //se cambio de int16_t a uint16_t equivale a readRegister())
uint8_t data[2];
uint16_t ads_value=0; //equivale a readRawValue()

	rcode =  I2C_read( fdI2C0, DEVADDRESS_ADS1115, CONVERSION_REG, 0x01, (char *)&data, 0x02, debug );

	if ( rcode == -1 ) {
		// Hubo error: trato de reparar el bus y reintentar la operacion
		// Espero 1seg a que se termine la fuente de ruido.
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
		// Reconfiguro los dispositivos I2C del bus que pueden haberse afectado
		xprintf_P(PSTR("ERROR: ADS1115_read\r\n") );
		goto quit;
	}
    
    ads_value = (data[0] << 8) | data[1];
   
quit:

	return( ads_value );    
}
//------------------------------------------------------------------------------

uint16_t ADS1115_readRawValue(bool debug){
    
    if( !(ADS1115_is_busy(debug)) ){
        vTaskDelay( ( TickType_t)( 1 / portTICK_PERIOD_MS ) );
        ADS1115_start(debug);
        while( ADS1115_is_busy(debug) );
        return ADS1115_readConvReg(debug);
        }
    xprintf_P(PSTR("ERROR: Read ADS1115\r\n"));
	return -1;
}

float ADS1115_readValue(uint8_t n, float k_z) //devuelve el valor leído en mVolts
{
    
uint8_t tries=10, i=0;
int16_t ads_value=0; 
int32_t measure_total=0;
float measure_total_mv;

        while ( (tries-- > 0) && (i<=n-1) ){
            vTaskDelay( ( TickType_t)( 1 / portTICK_PERIOD_MS ) );
            while( (!(ADS1115_is_busy(ads1115_debug_flag))) && (i<=n-1) ){
                ADS1115_start(ads1115_debug_flag);
                while( ADS1115_is_busy(ads1115_debug_flag) );
                ads_value = ADS1115_readRawValue(ads1115_debug_flag);
                measure_total += ads_value;
                i++;
                }
            }
        if(tries < 0){
            xprintf_P(PSTR("ERROR: Read ADS1115 timeout\r\n"));
            return(0);
            }
        measure_total_mv = ((float)measure_total)/n; //imprimir en formato %ld: xprintf("medida total: %ld\r\n",measure_total);        
        measure_total_mv += k_z; //se corrige error de offset ((debería calibrarse cada vez que se va a usar el equipo))
        measure_total_mv *= 0.1713; //se escala el valor a mvoltios en la escala elegida (0V-4.096V)
        return(measure_total_mv);
}
//------------------------------------------------------------------------------
float ADS1115_calibrar_zero(uint8_t m)
{
    uint8_t i=0, tries=10;
    int16_t ads_value;
    int32_t measure_total=0;
            
    while ( (tries-- > 0) && (i<=m-1) ){
            vTaskDelay( ( TickType_t)( 1 / portTICK_PERIOD_MS ) );
            while( (!(ADS1115_is_busy(ads1115_debug_flag))) && (i<=m-1) ){
                ADS1115_start(ads1115_debug_flag);
                while( ADS1115_is_busy(ads1115_debug_flag) );
                ads_value = ADS1115_readConvReg(ads1115_debug_flag);
                measure_total += ads_value;
                i++;
                }
            }
    if(tries < 0){
      xprintf_P(PSTR("ERROR: Read ADS1115 timeout\r\n"));
      return(0);
      }
    return(((float)measure_total)/m);
    
}

void ADS1115_config_debug_flag(bool flag)
{
    ads1115_debug_flag = flag;
}
//------------------------------------------------------------------------------
