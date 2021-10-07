/*
 * ADCads124S0.cpp
 *
 *      Author: reed
 */

#include "../device_specific/ADC/ads124S0.h"

#include "cmsis_os.h"


ADS124S0::ADS124S0(SPI_HandleTypeDef *spi, uint16_t ResetPin,
		GPIO_TypeDef *resetPort, uint16_t CSPin, GPIO_TypeDef *CSPort,
		uint16_t DRDYPin, GPIO_TypeDef *DRDYPort, uint16_t STARTPin,
		GPIO_TypeDef *STARTPort):
			spi(spi),
			ResetPin(ResetPin), resetPort(resetPort),
			CSPin(CSPin), CSPort(CSPort),
			DRDYPin(DRDYPin), DRDYPort(DRDYPort),
			STARTPin(STARTPin), STARTPort(STARTPort)
{
	HAL_GPIO_WritePin(resetPort, ResetPin, GPIO_PIN_SET);   // Make sure its not in reset
	HAL_GPIO_WritePin(STARTPort, STARTPin, GPIO_PIN_RESET); // Don't start sampling yet
	HAL_GPIO_WritePin(CSPort, CSPin, GPIO_PIN_SET);         // Not talking to it yet

}


bool ADS124S0::init()
{
	// Reset
	reset();

	// check ID
	adc_ID = ID_Reg(readReg8(REGISTER::ADS124S0_ID));
	if(adc_ID.DEV_ID != DEVICE_ID::ADS124S06 &&
			adc_ID.DEV_ID != DEVICE_ID::ADS124S08)
	{
		// Not a recognized device
		return false;
	}


	// set REFSEL to 00 Ref0: Pg35
	// configure REFP_BUF/REFN_BUF ????? disable : pg36
	adc_reference = REF_Reg(readReg8(REGISTER::ADS124S0_REF));
	adc_reference.REFSEL = 0b00;
	adc_reference.REFCON = 0b10; // internal reference needs to be on for current excitation
	writeReg8(REGISTER::ADS124S0_REF, adc_reference.get());


	// set CLK bit to ?? to choose internal oscillator
	// set FILTER bit to Sinc^3 or low-latency?? : pg37
		// sinc is slower, but less noise, need to find timing requirements
	// set DATARATE reg to ???? decide on slowest possible
	// set GCHOP
		// On is twice as slow, but again more accurate
	// set Mode bit to 1 in datarate reg for single shot : Pg60
	adc_datarate = DATARATE_Reg(readReg8(REGISTER::ADS124S0_DATARATE));
	adc_datarate.CLK = 0b0; // internal clock
	adc_datarate.MODE = 0b1; // single shot
	adc_datarate.G_CHOP = 0b1; // enable chopping
	adc_datarate.FILTER = 0b0; // sinc3 filter
	adc_datarate.DR = 0b0111; // 100SPS
	writeReg8(REGISTER::ADS124S0_DATARATE, adc_datarate.get());

	/*								Timing analysis
	 * Number of channels to sample total : (4 thermopile + 1 thermister)*2 = 10 channels
	 * Max time per channel: 100 ms
	 * Internal oscillator: 4.096 Mhz, or T = 244ns,
	 *
	 * Modulator runs at 4.096/16= 256Khz
	 *
	 * Max cycles per channel: 409,600
	 *
	 * GCHOP doubles number of samples
	 * sinc^3 filter triples number of samples
	 * 6 samples total per reading total
	 *
	 *
	 *
	 */


	// turn off current sources, IMAG to 0000 : Pg50
	adc_excite_current_mag = IDACMAG_Reg(readReg8(REGISTER::ADS124S0_IDACMAG));
	adc_excite_current_mag.FL_RAIL_EN = 0b0;//disabled
	adc_excite_current_mag.PSW = 0b0;//open
	adc_excite_current_mag.RESERVED = 0b00;
	adc_excite_current_mag.IMAG = EXCITATION_CURRENT::OFF; // disabled
	writeReg8(REGISTER::ADS124S0_IDACMAG, adc_excite_current_mag.get());

	// I1MUX set to 1111 for no connect : Pg50
	// I2MUX set to 1111 for no connect : Pg50
	adc_excite_current_mux = IDACMUX_Reg(readReg8(REGISTER::ADS124S0_IDACMUX));
	adc_excite_current_mux.I1MUX = 0b1111; // disabled
	adc_excite_current_mux.I2MUX = 0b1111; // disabled
	writeReg8(REGISTER::ADS124S0_IDACMUX, adc_excite_current_mux.get());


	// turn on/off voltage bias(VB_AINx) : Pg51
	adc_bias = VBIAS_Reg(0x00); // disable vbias
	writeReg8(REGISTER::ADS124S0_VBIAS, adc_bias.get());


	// turn on CRC & status
	adc_settings = SYS_Reg(readReg8(REGISTER::ADS124S0_SYS));
	adc_settings.SENDSTAT = 0b1; // send status
	adc_settings._CRC = 0b1; // send CRC
	adc_settings.TIMEOUT = 0b1; // spi timeout monitor
	writeReg8(REGISTER::ADS124S0_SYS, adc_settings.get());


	// set DELAY for signals to settle? timing analysis required : Pg61
	adc_prog_gain = PGA_Reg(readReg8(REGISTER::ADS124S0_PGA));
	adc_prog_gain.DELAY = 0b010;
	adc_prog_gain.PGA_EN = 0b00;
	adc_prog_gain.GAIN = 0b000;
	writeReg8(REGISTER::ADS124S0_PGA, adc_prog_gain.get());


	// set GPIO to analog input (should be default but make sure) : Pg85
	adc_gpio_con.RESERVED = 0x0000;
	adc_gpio_con.CON = 0b0000; // all analog inputs
	writeReg8(REGISTER::ADS124S0_GPIOCON, adc_gpio_con.get());

	adc_status = STATUS_Reg(readReg8(REGISTER::ADS124S0_STATUS));
	adc_status.FL_POR = 0; // reset POR flag
	writeReg8(REGISTER::ADS124S0_STATUS, adc_status.get());





	// Wait for the ADC to be ready (should just be good to go)
	// If not ready in 15ms, bail and report error
	for(int idx = 0; idx < 3; idx++)
	{
		if(!adc_status.nRDY)
		{
			return true;
		}
		adc_status = STATUS_Reg(readReg8(REGISTER::ADS124S0_STATUS));
		osDelay(5);
	}

	return false;
}

void ADS124S0::reset()
{
	HAL_GPIO_WritePin(resetPort, ResetPin, GPIO_PIN_RESET);   // Make sure its not in reset
	osDelay(10);
	HAL_GPIO_WritePin(resetPort, ResetPin, GPIO_PIN_SET);   // Make sure its not in reset
	osDelay(10);
}

uint32_t ADS124S0::readChannel(uint8_t channel)
{

	adc_input_mux.MUXN = 0b1100; // common
	adc_input_mux.MUXP = channel & 0xF;
	writeReg8(REGISTER::ADS124S0_INPMUX, adc_input_mux.get());
	osDelay(1);
	//HAL_GPIO_WritePin(STARTPort, STARTPin, GPIO_PIN_SET);
	issueCommand(COMMAND::START);
	osDelay(2);
	while(HAL_GPIO_ReadPin(DRDYPort, DRDYPin) == GPIO_PIN_SET)osThreadYield(); // wait for pin to go low

	//HAL_GPIO_WritePin(STARTPort, STARTPin, GPIO_PIN_RESET);
	return readData();
}

ADS124S0::~ADS124S0()
{
	// TODO Auto-generated destructor stub
}

uint8_t ADS124S0::readReg8(REGISTER address)
{
	txBuff[0] = 0b00100000;
	txBuff[0] |= (((uint8_t)address) & 0x1F);
	txBuff[1] = 0x00; // only write one register at a time

	HAL_GPIO_WritePin(CSPort, CSPin, GPIO_PIN_RESET);

	HAL_SPI_Transmit(spi, txBuff, 2, 100);

	HAL_SPI_Receive(spi, rxBuff, 1, 100);

	HAL_GPIO_WritePin(CSPort, CSPin, GPIO_PIN_SET);

	return rxBuff[0];
}

void ADS124S0::setExcitationMagnitude(EXCITATION_CURRENT current)
{
	IDACMAG_Reg mag(readReg8(ADS124S0_IDACMAG));
	mag.IMAG = ((uint8_t) current) & 0xF;
	writeReg8(ADS124S0_IDACMAG, mag.get());
}

void ADS124S0::stopExcitation()
{
	setExcitationMagnitude(EXCITATION_CURRENT::OFF);
	routeExcitation1(0xF); // disabled
	routeExcitation2(0xF); // disabled
}

void ADS124S0::routeExcitation1(uint8_t channel)
{
	IDACMUX_Reg imux(readReg8(ADS124S0_IDACMUX));
	imux.I1MUX = (uint8_t) channel & 0xF;
	writeReg8(ADS124S0_IDACMUX, imux.get());
}

void ADS124S0::routeExcitation2(uint8_t channel)
{
	IDACMUX_Reg imux(readReg8(ADS124S0_IDACMUX));
	imux.I2MUX = (uint8_t) channel & 0xF;
	writeReg8(ADS124S0_IDACMUX, imux.get());
}

uint32_t ADS124S0::readData()
{
	txBuff[0] = (uint8_t) COMMAND::RDATA;

	HAL_GPIO_WritePin(CSPort, CSPin, GPIO_PIN_RESET);

	HAL_SPI_Transmit(spi, txBuff, 1, 100);

	bool sendStat = false;

	// Need to determine how many bytes to expect
	if(adc_settings.SENDSTAT)
	{
		sendStat = true;
		HAL_SPI_Receive(spi, rxBuff, 1, 100); // Read the status of the ADC
	}

	HAL_SPI_Receive(spi, rxBuff+1, 3, 100); // Read the adc sample

	bool okay = true;

	if(adc_settings._CRC)
	{
		HAL_SPI_Receive(spi, rxBuff+4, 1, 100); // Read the CRC of the transaction
		if(!checkCRC(rxBuff, 4, rxBuff[4]))
		{
			// data is bad
			okay = false;
		}
	}

	HAL_GPIO_WritePin(CSPort, CSPin, GPIO_PIN_SET);

	if(okay)
	{
		if(sendStat)
		{
			adc_status = STATUS_Reg(rxBuff[0]);
		}
		return rxBuff[1] << 16 | rxBuff[2] << 8 | rxBuff[3] << 0;
	}
	else
	{
		return 0xFFFFFFFF; // not a valid reading from a 24-bit adc, treat as an error;
	}
}

bool ADS124S0::checkCRC(uint8_t * data, uint8_t length, uint8_t checksum)
{
    uint16_t i;
    uint16_t crc = 0x0;

    for(int idx = 0; idx < length; idx++)
    {
        i = (crc ^ data[idx]) & 0xFF;
        crc = (crc8x_table[i] ^ (crc << 8)) & 0xFF;
    }
    crc = crc & 0xFF;

    return checksum == crc;
}


void ADS124S0::writeReg8(REGISTER address, uint8_t data)
{
	// Write nnnnn registers starting at address rrrrr
	// r rrrr = starting register address.
	// n nnnn = number of registers to read or write – 1.
	// 010r rrrr, 000n nnnn
	txBuff[0] = 0b01000000;
	txBuff[0] |= (((uint8_t)address) & 0x1F);
	txBuff[1] = 0x00; // only write one register at a time
	txBuff[2] = data;

	HAL_GPIO_WritePin(CSPort, CSPin, GPIO_PIN_RESET);

	HAL_SPI_Transmit(spi, txBuff, 3, 100);

	HAL_GPIO_WritePin(CSPort, CSPin, GPIO_PIN_SET);
}

void ADS124S0::issueCommand(COMMAND cmd)
{
	switch(cmd)
	{
		case RREG:
		case WREG:
		case RDATA:
			// These should not be used with this method as they require
			// 		more steps than just sending the command
			Error_Handler();
			break;
		default:
			break;
	}
	txBuff[0] = (uint8_t) cmd;

	HAL_GPIO_WritePin(CSPort, CSPin, GPIO_PIN_RESET);

	HAL_SPI_Transmit(spi, txBuff, 1, 100);

	HAL_GPIO_WritePin(CSPort, CSPin, GPIO_PIN_SET);
}


const uint8_t ADS124S0::crc8x_table[256] = {
        0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31,
        0x24, 0x23, 0x2a, 0x2d, 0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
        0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d, 0xe0, 0xe7, 0xee, 0xe9,
        0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
        0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1,
        0xb4, 0xb3, 0xba, 0xbd, 0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
        0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea, 0xb7, 0xb0, 0xb9, 0xbe,
        0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
        0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16,
        0x03, 0x04, 0x0d, 0x0a, 0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
        0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a, 0x89, 0x8e, 0x87, 0x80,
        0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
        0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8,
        0xdd, 0xda, 0xd3, 0xd4, 0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
        0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44, 0x19, 0x1e, 0x17, 0x10,
        0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
        0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f,
        0x6a, 0x6d, 0x64, 0x63, 0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
        0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13, 0xae, 0xa9, 0xa0, 0xa7,
        0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
        0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef,
        0xfa, 0xfd, 0xf4, 0xf3
};

