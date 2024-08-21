
#include "adc1X15.h"

/*
 * https://github.com/Wh1teRabbitHU/ADS1115-Driver/blob/master/src/ADS1115-Driver.cpp
 */

static uint8_t currentPGA = 255;

flag_val_t ADS1115_readFlag(reg_addr_t dataAddress, flag_pos_t flagPos);
void ADS1115_writeFlag(reg_addr_t dataAddress, flag_pos_t flagPos, flag_val_t flagValue);
flag_val_t ADS1115_getFlag(reg_val_t registerValue, flag_pos_t flagPos, flag_val_t customVal);
reg_val_t ADS1115_setFlag(reg_val_t registerValue, flag_pos_t flagPos, flag_val_t flagVal);

bool f_adc1115_debug = false;
bool f_i2c_debug = false;

//------------------------------------------------------------------------------
void ADS1115_set_debug(void) {
    f_adc1115_debug = true;
}
//------------------------------------------------------------------------------
void ADS1115_clear_debug(void) {
    f_adc1115_debug = false;
}
//------------------------------------------------------------------------------
reg_val_t ADS1115_readRegister(reg_addr_t dataAddress) {
    
uint16_t data;

    I2C_read( fdI2C0, DEVADDRESS_ADC1115, dataAddress, 0x01, (char*)&data, 0x02, f_i2c_debug );

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 READ: regAddr=0x%02x, val=0x%02x\r\n"), dataAddress, data);
    }
 
    return (data);

}
//------------------------------------------------------------------------------
void ADS1115_writeRegister(reg_addr_t dataAddress, uint16_t data) {

uint8_t vdata[2];
    
    vdata[1] = data >> 8;
    vdata[0] = data & 255;

    I2C_write ( fdI2C0, DEVADDRESS_ADC1115, dataAddress, 0x01, (char *)&vdata, 0x02, f_i2c_debug );

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 WRITE: regAddr=0x%02x, val=0x%02x\r\n"), dataAddress, data);
    }
    
}
//------------------------------------------------------------------------------
/*
 * FUNCIONE PRIVADAS
 */

flag_val_t ADS1115_readFlag(reg_addr_t dataAddress, flag_pos_t flagPos) {

reg_val_t registerValue;
flag_val_t flag;

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 RDFLAG: regAddr=0x%02x, flag_pos=%d\r\n"), dataAddress, flagPos);
    }
	
    registerValue = ADS1115_readRegister(dataAddress);
	
    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 RDFLAG IN: regValue=0x%02x\r\n"), registerValue);
    }
    
    flag =  ADS1115_getFlag(registerValue, flagPos, 1);

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 RDFLAG OUT: flagValue=0x%02x\r\n"), flag);
    }
    
    return(flag);
    
}

//------------------------------------------------------------------------------
void ADS1115_writeFlag(reg_addr_t dataAddress, flag_pos_t flagPos, flag_val_t flagValue) {
	
    reg_val_t registerValue = ADS1115_readRegister(dataAddress);
    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 WRFLAG: regAddr=0x%02x, flag_pos=%d, flag_val=%d\r\n"), dataAddress, flagPos, flagValue);
        xprintf_P(PSTR("ADS1115 WRFLAG IN: regValue=0x%02x\r\n"), registerValue);
    }
	registerValue = ADS1115_setFlag(registerValue, flagPos, flagValue);
    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 WRFLAG OUT: regValue=0x%02x\r\n"), registerValue);
    }
	ADS1115_writeRegister(dataAddress, registerValue);
}
/*
flag_val_t ADS1115_getFlag(reg_val_t registerValue, flag_pos_t flagPos) {
	return getFlag(registerValue, flagPos, 1);
}
*/
//------------------------------------------------------------------------------
flag_val_t ADS1115_getFlag(reg_val_t registerValue, flag_pos_t flagPos, flag_val_t customVal) {
	
    return ((registerValue >> flagPos) & 1) == 1 ? customVal : 0;
}

//------------------------------------------------------------------------------
reg_val_t ADS1115_setFlag(reg_val_t registerValue, flag_pos_t flagPos, flag_val_t flagVal) {
	if (flagVal == 1) {
		return registerValue | (1 << flagPos);
	}

	return registerValue & (~(1 << flagPos));
}
/*
 * FUNCIONE PUBLICAS
 */
//------------------------------------------------------------------------------
void ADS1115_reset(void) {
    
    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >RESET\r\n"));
    }
	ADS1115_writeRegister(ADS1115_CONFIG_REG_ADDR, ADS1115_CONFIG_REG_DEF);
	ADS1115_writeRegister(ADS1115_LOW_TRESH_REG_ADDR, ADS1115_LOW_TRESH_REG_DEF);
	ADS1115_writeRegister(ADS1115_HIGH_TRESH_REG_ADDR, ADS1115_HIGH_TRESH_REG_DEF);
}
//------------------------------------------------------------------------------
reg_val_t ADS1115_readRawValue(void) {

uint16_t reg_value; 

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >READRAW regAddr=0x%02x\r\n"), ADS1115_CONVERSION_REG_ADDR );
    }

    reg_value = ADS1115_readRegister(ADS1115_CONVERSION_REG_ADDR); 

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 READRAW rawval=0x%02x (%u)\r\n"), reg_value, reg_value);
    }
    
	return (reg_value);
}
//------------------------------------------------------------------------------
float ADS1115_readConvertedValue(void) {
    
float multiplier = 0.0;
float convertedValue = 0.0;
uint16_t rawValue;

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >READCONVVALUE\r\n"));
    }
    
	rawValue = ADS1115_readRawValue();

	if (currentPGA == 255) {
		currentPGA = ADS1115_getPga();
	}
    
	switch (currentPGA) {
		case ADS1115_PGA_0_256:
			multiplier = ADS1115_PGA_0_256_MULT;
			break;
		case ADS1115_PGA_0_512:
			multiplier = ADS1115_PGA_0_512_MULT;
			break;
		case ADS1115_PGA_1_024:
			multiplier = ADS1115_PGA_1_024_MULT;
			break;
		case ADS1115_PGA_2_048:
			multiplier = ADS1115_PGA_2_048_MULT;
			break;
		case ADS1115_PGA_4_096:
			multiplier = ADS1115_PGA_4_096_MULT;
			break;
		case ADS1115_PGA_6_144:
			multiplier = ADS1115_PGA_6_144_MULT;
			break;
		default:
			multiplier = 1.0;
			break;
	}

    convertedValue = multiplier * rawValue;
    
    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 READCONVVALUE: currentPGA=0x%02x\r\n"), currentPGA);
        xprintf_P(PSTR("ADS1115 READCONVVALUE: multiplier=%0.3f\r\n"), multiplier);
        xprintf_P(PSTR("ADS1115 READCONVVALUE: rawvalue=%u\r\n"), rawValue);
        xprintf_P(PSTR("ADS1115 READCONVVALUE: convvalue=%0.3f\r\n"), convertedValue);
    }
    
	return (convertedValue);
    
}
//------------------------------------------------------------------------------
void ADS1115_startSingleConvertion(void) {

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >STARTSC\r\n"));
    }
    
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_OS_FLAG_POS, 1);
}
//------------------------------------------------------------------------------
uint8_t ADS1115_getOperationalStatus(void) {
    
    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >GETOPSTATUS\r\n"));
    }
    
	return ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_OS_FLAG_POS);
}
//------------------------------------------------------------------------------
void ADS1115_setMultiplexer(uint8_t mux) {
    
    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >SETMUX [0x%02d]\r\n"), mux);
    }
    
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_MUX2_DAT_POS, ADS1115_getFlag(mux, 2, 1));
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_MUX1_DAT_POS, ADS1115_getFlag(mux, 1, 1));
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_MUX0_DAT_POS, ADS1115_getFlag(mux, 0, 1));
}
//------------------------------------------------------------------------------
uint8_t ADS1115_getMultiplexer(void) {
    
	uint8_t mux = ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_MUX0_DAT_POS);

    
	mux |= ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_MUX1_DAT_POS) << 1;
	mux |= ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_MUX2_DAT_POS) << 2;

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >GETMUX [0x%02d]\r\n"), mux);
    }
    
	return mux;
}
//------------------------------------------------------------------------------
void ADS1115_setPga(uint8_t pga) {
	
    currentPGA = pga;

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >SETPGA [0x%02d]\r\n"), pga);
    }
    
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_PGA2_DAT_POS, ADS1115_getFlag(pga, 2, 1));
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_PGA1_DAT_POS, ADS1115_getFlag(pga, 1, 1));
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_PGA0_DAT_POS, ADS1115_getFlag(pga, 0, 1));
}
//------------------------------------------------------------------------------
uint8_t ADS1115_getPga(void) {
	
    uint8_t pga = ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_PGA0_DAT_POS);

	pga |= ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_PGA1_DAT_POS) << 1;
	pga |= ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_PGA2_DAT_POS) << 2;

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >GETPGA [0x%02d]\r\n"), pga);
    }
    
	return pga;
}
//------------------------------------------------------------------------------
void ADS1115_setDeviceMode(uint8_t mode) {
    
    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >SETMODE [0x%02d]\r\n"), mode);
    }
    
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_MODE_FLAG_POS, mode);
}
//------------------------------------------------------------------------------
uint8_t ADS1115_getDeviceMode(void) {
    
    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >GETMODE\r\n"));
    }
	return ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_MODE_FLAG_POS);
}
//------------------------------------------------------------------------------
void ADS1115_setDataRate(uint8_t dataRate) {
    
    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >SETDATARATE\r\n"));
    }
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_DR2_DAT_POS, ADS1115_getFlag(dataRate, 2, 1));
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_DR1_DAT_POS, ADS1115_getFlag(dataRate, 1, 1));
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_DR0_DAT_POS, ADS1115_getFlag(dataRate, 0, 1));
}
//------------------------------------------------------------------------------
uint8_t ADS1115_getDataRate(void) {
    
	uint8_t dataRate = ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_DR0_DAT_POS);

	dataRate |= ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_DR1_DAT_POS) << 1;
	dataRate |= ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_DR2_DAT_POS) << 2;

	return dataRate;
}
//------------------------------------------------------------------------------
void ADS1115_setComparatorMode(uint8_t mode) {
    
    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >SETCOMPMODE\r\n"));
    }
    
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_COMP_MODE_FLAG_POS, mode);
}
//------------------------------------------------------------------------------
uint8_t ADS1115_getComparatorMode(void) {
	return ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_COMP_MODE_FLAG_POS);
}
//------------------------------------------------------------------------------
void ADS1115_setComparatorPolarity(uint8_t polarity) {

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >SETPOL\r\n"));
    }
    
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_COMP_POL_FLAG_POS, polarity);
}
//------------------------------------------------------------------------------
uint8_t ADS1115_getComparatorPolarity(void) {
	return ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_COMP_POL_FLAG_POS);
}
//------------------------------------------------------------------------------
void ADS1115_setLatching(bool latching) {

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >SETLATCHING\r\n"));
    }
    
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_COMP_LAT_FLAG_POS, latching);
}
//------------------------------------------------------------------------------
bool ADS1115_isLatching(void) {
	return ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_COMP_LAT_FLAG_POS);
}
//------------------------------------------------------------------------------
void ADS1115_setComparatorQueue(uint8_t queue) {

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >SETCOMPQUEUE\r\n"));
    }
    
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_COMP_QUE1_DAT_POS, ADS1115_getFlag(queue, 1, 1));
	ADS1115_writeFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_COMP_QUE0_DAT_POS, ADS1115_getFlag(queue, 0, 1));
}
//------------------------------------------------------------------------------
uint8_t ADS1115_getComparatorQueue(void) {
	uint8_t queue = ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_COMP_QUE0_DAT_POS);

	queue |= ADS1115_readFlag(ADS1115_CONFIG_REG_ADDR, ADS1115_COMP_QUE1_DAT_POS) << 1;

	return queue;
}
//------------------------------------------------------------------------------
void ADS1115_setLowTreshold(reg_val_t lowTreshold) {
	ADS1115_writeRegister(ADS1115_LOW_TRESH_REG_ADDR, lowTreshold);
}
//------------------------------------------------------------------------------
reg_val_t ADS1115_getLowTreshold(void) {
	return ADS1115_readRegister(ADS1115_LOW_TRESH_REG_ADDR);
}
//------------------------------------------------------------------------------
void ADS1115_setHighTreshold(reg_val_t highTreshold) {
	ADS1115_writeRegister(ADS1115_HIGH_TRESH_REG_ADDR, highTreshold);
}
//------------------------------------------------------------------------------
reg_val_t ADS1115_getHighTreshold(void) {
	return ADS1115_readRegister(ADS1115_HIGH_TRESH_REG_ADDR);
}
//------------------------------------------------------------------------------

void ADS1115_setup(void) {

	ADS1115_reset();
	ADS1115_setDeviceMode(ADS1115_MODE_SINGLE);
	ADS1115_setDataRate(ADS1115_DR_250_SPS);
	ADS1115_setPga(ADS1115_PGA_2_048);
}
//------------------------------------------------------------------------------

float ADS1115_readValue(uint8_t input) {
    
float converted_value;
    
    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >READVALUE\r\n"));
    }
    
	ADS1115_setMultiplexer(input);
	ADS1115_startSingleConvertion();

    vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
    
	while (ADS1115_getOperationalStatus() == 0) {
        vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
    }

    converted_value = ADS1115_readConvertedValue();
    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 READVALUE converted_val=[0x%02x](%0.3f)\r\n"), converted_value, converted_value);
    }    
	return (converted_value);
    
}

//------------------------------------------------------------------------------
void ADS1115_test_readRawSingle(void) {
    
    
uint16_t reg_rawvalue;
    
    xprintf_P(PSTR("ADS1115 >READRAWSINGLE\r\n"));
    
	ADS1115_setMultiplexer(ADS1115_MUX_AIN0_GND);
	ADS1115_startSingleConvertion();

    vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
    
	while (ADS1115_getOperationalStatus() == 0) {
        vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
    }

    reg_rawvalue = ADS1115_readRegister(ADS1115_CONVERSION_REG_ADDR); 
    
    xprintf_P(PSTR("ADS1115 READRAWSINGLE rawval=[0x%02x](%0.3f)\r\n"), reg_rawvalue, reg_rawvalue);  

}
//------------------------------------------------------------------------------
void ADS1115_test_readSingle(void) {
    
float value0;

    if (f_adc1115_debug) {
        xprintf_P(PSTR("ADS1115 >READSINGLE\r\n"));
    }

    value0 = ADS1115_readValue(ADS1115_MUX_AIN0_GND);
    xprintf_P(PSTR("ADS1115 READSINGLE res=0x%04x (%0.3f)\r\n"),value0, value0 );
}
//------------------------------------------------------------------------------
void ADS1115_test_readMultiple(uint8_t veces) {
    
uint8_t i;
float buffer[255];
float total = 0.0;
 
    for (i=0; i<veces;i++) {
        buffer[i] = ADS1115_readValue(ADS1115_MUX_AIN0_GND);
        total += buffer[i];
        xprintf_P(PSTR("p%02d=%0.2f, %0.2f\r\n"), i, buffer[i], total );
    }

    total /= veces;

    xprintf_P(PSTR("RES=%0.2f\r\n"), total );
}

//------------------------------------------------------------------------------
void ADS1115_test_readRawMultiple(uint8_t veces) {
    
uint8_t i;
uint16_t buffer[255];
float total = 0.0;

	ADS1115_setMultiplexer(ADS1115_MUX_AIN0_GND);
 
    for (i=0; i<veces;i++) {
        ADS1115_startSingleConvertion();
        while (ADS1115_getOperationalStatus() == 0) {
            vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
        }
        buffer[i] = ADS1115_readRawValue();
        total += buffer[i];
        xprintf_P(PSTR("p%02d=%u [0x%02x], %0.2f\r\n"), i, buffer[i], buffer[i], total );
    }

    total /= veces;

    xprintf_P(PSTR("RES=%0.2f\r\n"), total );
}

//------------------------------------------------------------------------------
