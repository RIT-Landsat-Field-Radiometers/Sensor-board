/*
 * ADCads124S0.h
 *
 *      Author: reed
 */

#ifndef INC_ADS124S0_H_
#define INC_ADS124S0_H_

#include <cstdint>
#include "main.h"

class ADS124S0
{
public:
#include "../device_specific/ADC/ads123S0_enums.h" 	// Contains enums for this class
#include "../device_specific/ADC/ads123S0_structs.h"	// Contains structs for this class

public:
	ADS124S0(SPI_HandleTypeDef *spi, uint16_t ResetPin, GPIO_TypeDef *resetPort,
			uint16_t CSPin, GPIO_TypeDef *CSPort, uint16_t DRDYPin,
			GPIO_TypeDef *DRDYPort, uint16_t STARTPin, GPIO_TypeDef *STARTPort);
	bool init();
	void reset();

	uint32_t readChannel(uint8_t channel);

	virtual ~ADS124S0();
	ADS124S0(const ADS124S0 &other) = default;
	ADS124S0(ADS124S0 &&other) = default;
	ADS124S0& operator=(const ADS124S0 &other) = default;
	ADS124S0& operator=(ADS124S0 &&other) = default;

	void setExcitationMagnitude(EXCITATION_CURRENT current);

	void stopExcitation();

	void routeExcitation1(uint8_t channel);

	void routeExcitation2(uint8_t channel);

	bool checkCRC(uint8_t *data, uint8_t length, uint8_t checksum);

	uint8_t readReg8(REGISTER address);

	uint32_t readData();

	void writeReg8(REGISTER address, uint8_t data);

	void issueCommand(COMMAND cmd);

private:

	SPI_HandleTypeDef *spi;

	uint16_t ResetPin;
	GPIO_TypeDef *resetPort;

	uint16_t CSPin;
	GPIO_TypeDef *CSPort;

	uint16_t DRDYPin;
	GPIO_TypeDef *DRDYPort;

	uint16_t STARTPin;
	GPIO_TypeDef *STARTPort;

	uint8_t txBuff[8];
	uint8_t rxBuff[8];

	SYS_Reg adc_settings = 0x10; // default from reset: PG72

	ID_Reg adc_ID = 0xFF; // this is an invalid value, can use to see if it has been read yet

	STATUS_Reg adc_status = 0x80; // default value: PG72

	INPMUX_Reg adc_input_mux = 0x01; // default PG:72

	PGA_Reg adc_prog_gain = 0x00; // default PG:72

	DATARATE_Reg adc_datarate = 0x14; // default PG:72

	REF_Reg adc_reference = 0x10; // default PG:72

	IDACMAG_Reg adc_excite_current_mag = 0x00; // default PG:72

	IDACMUX_Reg adc_excite_current_mux = 0xFF; // default PG:72

	VBIAS_Reg adc_bias = 0x00; // default PG:72

	GPIOCON_Reg adc_gpio_con = 0x00;

	static const uint8_t crc8x_table[256];
};

#endif /* INC_ADS124S0_H_ */
