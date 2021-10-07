typedef struct SYS_Reg
{
	SYS_Reg(uint8_t raw)
	{
		SYS_MON = (raw >> 5) & 0x07;
		CAL_SAMP = (raw >> 3) & 0x03;
		TIMEOUT = (raw >> 2) & 0x01;
		_CRC = (raw >> 1) & 0x01;
		SENDSTAT = (raw >> 0) & 0x01;
	}

	uint8_t get()
	{
		uint8_t retval = 0x00;
		retval |= (SENDSTAT << 0);
		retval |= (_CRC << 1);
		retval |= (TIMEOUT << 2);
		retval |= (CAL_SAMP << 3);
		retval |= (SYS_MON << 5);
		return retval;
	}

	uint8_t SYS_MON :3;
	/*
	 * System monitor configuration(1)
	 Enables a set of system monitor measurements using the ADC.
	 000 : Disabled (default)
	 001 : PGA inputs shorted to (AVDD + AVSS) / 2 and disconnected from
	 AINx and the multiplexer; gain set by user
	 010 : Internal temperature sensor measurement; PGA must be enabled
	 (PGA_EN[1:0] = 01); gain set by user(2)
	 011 : (AVDD – AVSS) / 4 measurement; gain set to 1
	 (3)
	 100 : DVDD / 4 measurement; gain set to 1
	 (3)
	 101 : Burn-out current sources enabled, 0.2-µA setting
	 110 : Burn-out current sources enabled, 1-µA setting
	 111 : Burn-out current sources enabled, 10-µA setting
	 */
	uint8_t CAL_SAMP :2;
	/*
	 * Calibration sample size selection
	 Configures the number of samples averaged for self and system offset and
	 system gain calibration.
	 00 : 1 sample
	 01 : 4 samples
	 10 : 8 samples (default)
	 11 : 16 samples
	 */
	uint8_t TIMEOUT :1;
	/*
	 * SPI timeout enable
	 Enables the SPI timeout function.
	 0 : Disabled (default)
	 1 : Enabled
	 */
	uint8_t _CRC :1;
	/*
	 * CRC enable
	 Enables the CRC byte appended to the conversion result. When enabled,
	 CRC is calculated across the 24-bit conversion result (plus the STATUS
	 byte if enabled).
	 0 : Disabled (default)
	 1 : Enabled
	 */
	uint8_t SENDSTAT :1;
	/*
	 * STATUS byte enable
	 Enables the STATUS byte prepended to the conversion result.
	 0 : Disabled (default)
	 1 : Enabled
	 */
} SYS_Reg;

typedef struct ID_Reg
{
	ID_Reg(uint8_t raw)
	{
		RESERVED = (raw >> 3);
		DEV_ID = raw & 0x7;
	}

	uint8_t get()
	{
		uint8_t retval = 0x00;
		retval |= (DEV_ID << 0);
		retval |= (RESERVED << 3);
		return retval;
	}

	uint8_t RESERVED :5; /* Values are subject to change without notice */
	uint8_t DEV_ID :3;
	/*
	 * Device identifier
	 Identifies the model of the device.
	 000 : ADS124S08 (12 channels, 24 bits)
	 001 : ADS124S06 (6 channels, 24 bits)
	 else: Reserved
	 */
} ID_Reg;

typedef struct STATUS_Reg
{
	STATUS_Reg(uint8_t raw)
	{
		FL_POR = (raw >> 7) & 0x01;
		nRDY = (raw >> 6) & 0x01;
		FL_P_RAILP = (raw >> 5) & 0x01;
		FL_P_RAILN = (raw >> 4) & 0x01;
		FL_N_RAILP = (raw >> 3) & 0x01;
		FL_N_RAILN = (raw >> 2) & 0x01;
		FL_REF_L1 = (raw >> 1) & 0x01;
		FL_REF_L0 = (raw >> 0) & 0x01;
	}

	uint8_t get()
	{
		uint8_t retval = 0x00;
		retval |= (FL_REF_L0 << 0);
		retval |= (FL_REF_L1 << 1);
		retval |= (FL_N_RAILN << 2);
		retval |= (FL_N_RAILP << 3);
		retval |= (FL_P_RAILN << 4);
		retval |= (FL_P_RAILP << 5);
		retval |= (nRDY << 6);
		retval |= (FL_POR << 7);
		return retval;
	}

	uint8_t FL_POR :1;
	/*
	 * POR flag
	 Indicates a power-on reset (POR) event has occurred.
	 0 : Register has been cleared and no POR event has occurred.
	 1 : POR event occurred and has not been cleared. Flag must be cleared by
	 user register write (default)
	 */
	uint8_t nRDY :1;
	/*
	 * Device ready flag
	 Indicates the device has started up and is ready for communication.
	 0 : ADC ready for communication (default)
	 1 : ADC not ready
	 */
	uint8_t FL_P_RAILP :1;
	/*
	 * Positive PGA output at positive rail flag(1)
	 Indicates the positive PGA output is within 150 mV of AVDD.
	 0 : No error (default)
	 1 : PGA positive output within 150 mV of AVDD
	 */
	uint8_t FL_P_RAILN :1;
	/*
	 * Positive PGA output at negative rail flag(1)
	 Indicates the positive PGA output is within 150 mV of AVSS.
	 0 : No error (default)
	 1 : PGA positive output within 150 mV of AVSS
	 */
	uint8_t FL_N_RAILP :1;
	/*
	 * Negative PGA output at positive rail flag(1)
	 Indicates the negative PGA output is within 150 mV of AVDD.
	 0 : No error (default)
	 1 : PGA negative output within 150 mV of AVDD
	 */
	uint8_t FL_N_RAILN :1;
	/*
	 * Negative PGA output at negative rail flag(1)
	 Indicates the negative PGA output is within 150 mV of AVSS.
	 0 : No error (default)
	 1 : PGA negative output within 150 mV of AVSS
	 */
	uint8_t FL_REF_L1 :1;
	/*
	 * Reference voltage monitor flag, level 1
	 Indicates the external reference voltage is lower than 1/3 of the analog
	 supply voltage. Can be used to detect an open-excitation lead in a 3-wire
	 RTD application.
	 0 : Differential reference voltage ≥ 1/3 · (AVDD – AVSS) (default)
	 1 : Differential reference voltage < 1/3 · (AVDD – AVSS)
	 */
	uint8_t FL_REF_L0 :1;
	/*
	 * Reference voltage monitor flag, level 0
	 Indicates the external reference voltage is lower than 0.3 V. Can be used to
	 indicate a missing or floating external reference voltage.
	 0 : Differential reference voltage ≥ 0.3 V (default)
	 1 : Differential reference voltage < 0.3 V
	 */
} STATUS_Reg;

typedef struct INPMUX_Reg
{
	INPMUX_Reg(uint8_t raw)
	{
		MUXP = (raw >> 4) & 0x07;
		MUXN = (raw >> 0) & 0x07;
	}

	uint8_t get()
	{
		uint8_t retval = 0x00;
		retval |= (MUXN << 0);
		retval |= (MUXP << 4);
		return retval;
	}

	uint8_t MUXP :4;
	/*
	 * Positive ADC input selection
	 Selects the ADC positive input channel.
	 0000 : AIN0 (default)
	 0001 : AIN1
	 0010 : AIN2
	 0011 : AIN3
	 0100 : AIN4
	 0101 : AIN5
	 0110 : AIN6 (ADS124S08 only)
	 0111 : AIN7 (ADS124S08 only)
	 1000 : AIN8 (ADS124S08 only)
	 1001 : AIN9 (ADS124S08 only)
	 1010 : AIN10 (ADS124S08 only)
	 1011 : AIN11 (ADS124S08 only)
	 1100 : AINCOM
	 1101 : Reserved
	 1110 : Reserved
	 1111 : Reserved
	 */
	uint8_t MUXN :4;
	/*
	 * Negative ADC input selection
	 Selects the ADC negative input channel.
	 0000 : AIN0
	 0001 : AIN1 (default)
	 0010 : AIN2
	 0011 : AIN3
	 0100 : AIN4
	 0101 : AIN5
	 0110 : AIN6 (ADS124S08 only)
	 0111 : AIN7 (ADS124S08 only)
	 1000 : AIN8 (ADS124S08 only)
	 1001 : AIN9 (ADS124S08 only)
	 1010 : AIN10 (ADS124S08 only)
	 1011 : AIN11 (ADS124S08 only)
	 1100 : AINCOM
	 1101 : Reserved
	 1110 : Reserved
	 1111 : Reserved
	 */
} INPMUX_Reg;

typedef struct PGA_Reg
{
	PGA_Reg(uint8_t raw)
	{
		DELAY = (raw >> 5) & 0x07;
		PGA_EN = (raw >> 3) & 0x03;
		GAIN = (raw >> 0) & 0x07;
	}

	uint8_t get()
	{
		uint8_t retval = 0x00;
		retval |= (GAIN << 0);
		retval |= (PGA_EN << 3);
		retval |= (DELAY << 5);
		return retval;
	}

	uint8_t DELAY :3;
	/*
	 * Programmable conversion delay selection
	 Sets the programmable conversion delay time for the first conversion after a
	 WREG when a configuration change resets of the digital filter and triggers a
	 new conversion(1)
	 .
	 000 : 14 · tMOD (default)
	 001 : 25 · tMOD
	 010 : 64 · tMOD
	 011 : 256 · tMOD
	 100 : 1024 · tMOD
	 101 : 2048 · tMOD
	 110 : 4096 · tMOD
	 111 : 1 · tMOD
	 */
	uint8_t PGA_EN :2;
	/*
	 * PGA enable
	 Enables or bypasses the PGA.
	 00 : PGA is powered down and bypassed. Enables single-ended
	 measurements with unipolar supply (Set gain = 1
	 (2)) (default)
	 01 : PGA enabled (gain = 1 to 128)
	 10 : Reserved
	 11 : Reserved
	 */
	uint8_t GAIN :3;
	/*
	 * PGA gain selection
	 Configures the PGA gain.
	 000 : 1 (default)
	 001 : 2
	 010 : 4
	 011 : 8
	 100 : 16
	 101 : 32
	 110 : 64
	 111 : 128
	 */
} PGA_Reg;

typedef struct DATARATE_Reg
{
	DATARATE_Reg(uint8_t raw)
	{
		G_CHOP = (raw >> 7) & 0x01;
		CLK = (raw >> 6) & 0x01;
		MODE = (raw >> 5) & 0x01;
		FILTER = (raw >> 4) & 0x01;
		DR = raw & 0x07;
	}

	uint8_t get()
	{
		uint8_t retval = 0x00;
		retval |= (DR << 0);
		retval |= (FILTER << 4);
		retval |= (MODE << 5);
		retval |= (CLK << 6);
		retval |= (G_CHOP << 7);
		return retval;
	}

	uint8_t G_CHOP :1;
	/*
	 * Global chop enable
	 Enables the global chop function. When enabled, the device automatically
	 swaps the inputs and takes the average of two consecutive readings to
	 cancel the offset voltage.
	 0 : Disabled (default)
	 1 : Enabled
	 */
	uint8_t CLK :1;
	/*
	 * Clock source selection
	 Configures the clock source to use either the internal oscillator or an
	 external clock.
	 0 : Internal 4.096-MHz oscillator (default)
	 1 : External clock
	 */
	uint8_t MODE :1;
	/*
	 * Conversion mode selection
	 Configures the ADC for either continuous conversion or single-shot
	 conversion mode.
	 0 : Continuous conversion mode (default)
	 1 : Single-shot conversion mode
	 */
	uint8_t FILTER :1;
	/*
	 * Digital filter selection
	 Configures the ADC to use either the sinc3 or the low-latency filter.
	 0 : Sinc3
	 filter
	 1 : Low-latency filter (default)
	 */
	uint8_t DR :4;
	/*
	 * Data rate selection
	 Configures the output data rate(1)
	 .
	 0000 : 2.5 SPS
	 0001 : 5 SPS
	 0010 : 10 SPS
	 0011 : 16.6 SPS
	 0100 : 20 SPS (default)
	 0101 : 50SPS
	 0110 : 60 SPS
	 0111 : 100 SPS
	 1000 : 200 SPS
	 1001 : 400 SPS
	 1010 : 800 SPS
	 1011 : 1000 SPS
	 1100 : 2000 SPS
	 1101 : 4000 SPS
	 1110 : 4000 SPS
	 1111 : Reserved
	 */
} DATARATE_Reg;

typedef struct REF_Reg
{
	REF_Reg(uint8_t raw)
	{
		FL_REF_EN = (raw >> 6) & 0x03;
		nREFP_BUF = (raw >> 5) & 0x01;
		nREFN_BUF = (raw >> 4) & 0x01;
		REFSEL = (raw >> 2) & 0x03;
		REFCON = (raw >> 0) & 0x03;
	}

	uint8_t get()
	{
		uint8_t retval = 0x00;
		retval |= (REFCON << 0);
		retval |= (REFSEL << 2);
		retval |= (nREFN_BUF << 4);
		retval |= (nREFP_BUF << 5);
		retval |= (FL_REF_EN << 6);
		return retval;
	}

	uint8_t FL_REF_EN :2;
	/*
	 * Reference monitor configuration
	 Enables and configures the reference monitor.
	 00 : Disabled (default)
	 01 : FL_REF_L0 monitor enabled, threshold 0.3 V
	 10 : FL_REF_L0 and FL_REF_L1 monitors enabled, thresholds 0.3 V and
	 1/3 · (AVDD – AVSS)
	 11 : FL_REF_L0 monitor and 10-MΩ pull-together enabled, threshold 0.3 V
	 */
	uint8_t nREFP_BUF :1;
	/*
	 * Positive reference buffer bypass
	 Disables the positive reference buffer. Recommended when V(REFPx) is
	 close to AVDD.
	 0 : Enabled (default)
	 1 : Disabled
	 */
	uint8_t nREFN_BUF :1;
	/*
	 * Negative reference buffer bypass
	 Disables the negative reference buffer. Recommended when V(REFNx) is
	 close to AVSS.
	 0 : Enabled
	 1 : Disabled (default)
	 */
	uint8_t REFSEL :2;
	/*
	 * Reference input selection
	 Selects the reference input source for the ADC.
	 00 : REFP0, REFN0 (default)
	 01 : REFP1, REFN1
	 10 : Internal 2.5-V reference(1)
	 11 : Reserved
	 */
	uint8_t REFCON :2;
	/*
	 * Internal voltage reference configuration(2)
	 Configures the behavior of the internal voltage reference.
	 00 : Internal reference off (default)
	 01 : Internal reference on, but powers down in power-down mode
	 10 : Internal reference is always on, even in power-down mode
	 11 : Reserved
	 */
} REF_Reg;

typedef struct IDACMAG_Reg
{
	IDACMAG_Reg(uint8_t raw)
	{
		FL_RAIL_EN = (raw >> 7) & 0x01;
		PSW = (raw >> 6) & 0x01;
		RESERVED = (raw >> 4) & 0x03;
		IMAG = (raw >> 0) & 0x0F;
	}

	uint8_t get()
	{
		uint8_t retval = 0x00;
		retval |= (IMAG << 0);
		retval |= (RESERVED << 4);
		retval |= (PSW << 6);
		retval |= (FL_RAIL_EN << 7);
		return retval;
	}

	uint8_t FL_RAIL_EN :1;
	/*
	 * PGA output rail flag enable
	 Enables the PGA output voltage rail monitor circuit.
	 0 : Disabled (default)
	 1 : Enabled
	 */
	uint8_t PSW :1;
	/*
	 * Low-side power switch
	 Controls the low-side power switch. The low-side power switch opens
	 automatically in power-down mode.
	 0 : Open (default)
	 1 : Closed
	 */
	uint8_t RESERVED :2; // Always write 0h
	uint8_t IMAG :4;
	/*
	 * IDAC magnitude selection
	 Selects the value of the excitation current sources. Sets IDAC1 and IDAC2
	 to the same value.
	 0000 : Off (default)
	 0001 : 10 µA
	 0010 : 50 µA
	 0011 : 100 µA
	 0100 : 250 µA
	 0101 : 500 µA
	 0110 : 750 µA
	 0111 : 1000 µA
	 1000 : 1500 µA
	 1001 : 2000 µA
	 1010 - 1111 : Off
	 */
} IDACMAG_Reg;

typedef struct IDACMUX_Reg
{
	IDACMUX_Reg(uint8_t raw)
	{
		I2MUX = (raw >> 4) & 0x0F;
		I1MUX = (raw >> 0) & 0x0F;
	}

	uint8_t get()
	{
		uint8_t retval = 0x00;
		retval |= (I1MUX << 0);
		retval |= (I2MUX << 4);
		return retval;
	}

	uint8_t I2MUX :4;
	/**
	 * IDAC2 output channel selection
	 Selects the output channel for IDAC2.
	 0000 : AIN0
	 0001 : AIN1
	 0010 : AIN2
	 0011 : AIN3
	 0100 : AIN4
	 0101 : AIN5
	 0110 : AIN6 (ADS124S08), REFP1 (ADS124S06)
	 0111 : AIN7 (ADS124S08), REFN1 (ADS124S06)
	 1000 : AIN8 (ADS124S08 only)
	 1001 : AIN9 (ADS124S08 only)
	 1010 : AIN10 (ADS124S08 only)
	 1011 : AIN11 (ADS124S08 only)
	 1100 : AINCOM
	 1101 - 1111 : Disconnected (default)
	 */
	uint8_t I1MUX :4;
	/*
	 * IDAC1 output channel selection
	 Selects the output channel for IDAC1.
	 0000 : AIN0
	 0001 : AIN1
	 0010 : AIN2
	 0011 : AIN3
	 0100 : AIN4
	 0101 : AIN5
	 0110 : AIN6 (ADS124S08 only), REFP1 (ADS124S06)
	 0111 : AIN7 (ADS124S08 only), REFN1 (ADS124S06)
	 1000 : AIN8 (ADS124S08 only)
	 1001 : AIN9 (ADS124S08 only)
	 1010 : AIN10 (ADS124S08 only)
	 1011 : AIN11 (ADS124S08 only)
	 1100 : AINCOM
	 1101 - 1111 : Disconnected (default)
	 */
} IDACMUX_Reg;

typedef struct VBIAS_Reg
{
	VBIAS_Reg(uint8_t raw)
	{
		VB_LEVEL = (raw >> 7) & 0x01;
		VB_AINC = (raw >> 6) & 0x01;
		VB_AIN5 = (raw >> 5) & 0x01;
		VB_AIN4 = (raw >> 4) & 0x01;
		VB_AIN3 = (raw >> 3) & 0x01;
		VB_AIN2 = (raw >> 2) & 0x01;
		VB_AIN1 = (raw >> 1) & 0x01;
		VB_AIN0 = (raw >> 0) & 0x01;
	}

	uint8_t get()
	{
		uint8_t retval = 0x00;
		retval |= (VB_AIN0 << 0);
		retval |= (VB_AIN1 << 1);
		retval |= (VB_AIN2 << 2);
		retval |= (VB_AIN3 << 3);
		retval |= (VB_AIN4 << 4);
		retval |= (VB_AIN5 << 5);
		retval |= (VB_AINC << 6);
		retval |= (VB_LEVEL << 7);
		return retval;
	}

	uint8_t VB_LEVEL :1;
	/*
	 * VBIAS level selection
	 Sets the VBIAS output voltage level. VBIAS is disabled when not connected
	 to any input.
	 0 : (AVDD + AVSS) / 2 (default)
	 1 : (AVDD + AVSS) / 12
	 */
	uint8_t VB_AINC :1;
	/**
	 * AINCOM VBIAS selection(1)
	 Enables VBIAS on the AINCOM pin.
	 0 : VBIAS disconnected from AINCOM (default)
	 1 : VBIAS connected to AINCOM
	 */
	uint8_t VB_AIN5 :1;
	/*
	 * AIN5 VBIAS selection(1)
	 Enables VBIAS on the AIN5 pin.
	 0 : VBIAS disconnected from AIN5 (default)
	 1 : VBIAS connected to AIN5
	 */
	uint8_t VB_AIN4 :1;
	/*
	 * AIN4 VBIAS selection(1)
	 Enables VBIAS on the AIN4 pin.
	 0 : VBIAS disconnected from AIN4 (default)
	 1 : VBIAS connected to AIN4
	 */
	uint8_t VB_AIN3 :1;
	/*
	 * AIN3 VBIAS selection(1)
	 Enables VBIAS on the AIN3 pin.
	 0 : VBIAS disconnected from AIN3 (default)
	 1 : VBIAS connected to AIN3
	 */
	uint8_t VB_AIN2 :1;
	/*
	 * AIN2 VBIAS selection(1)
	 Enables VBIAS on the AIN2 pin.
	 0 : VBIAS disconnected from AIN2 (default)
	 1 : VBIAS connected to AIN2
	 */
	uint8_t VB_AIN1 :1;
	/*
	 * AIN1 VBIAS selection(1)
	 Enables VBIAS on the AIN1 pin.
	 0 : VBIAS disconnected from AIN1 (default)
	 1 : VBIAS connected to AIN1
	 */
	uint8_t VB_AIN0 :1;
	/*
	 * AIN0 VBIAS selection(1)
	 Enables VBIAS on the AIN0 pin.
	 0 : VBIAS disconnected from AIN0 (default)
	 1 : VBIAS connected to AIN5
	 */
} VBIAS_Reg;

typedef struct FSCAL_Reg
{
	union FSCAL
	{
		uint32_t gain;
		uint8_t raw[3];
	};
} FSCAL_Reg;

typedef struct GPIODAT_Reg
{
	GPIODAT_Reg(uint8_t raw)
	{
		DIR = (raw >> 4) & 0x0F;
		DAT = (raw >> 0) & 0x0F;
	}

	uint8_t get()
	{
		uint8_t retval = 0x00;
		retval |= (DAT << 0);
		retval |= (DIR << 4);
		return retval;
	}

	uint8_t DIR :4;
	/*
	 * GPIO direction
	 Configures the selected GPIO as an input or output.
	 0 : GPIO[x] configured as output (default)
	 1 : GPIO[x] configured as input
	 */
	uint8_t DAT :4;
	/*
	 * GPIO data
	 Contains the data of the GPIO inputs or outputs.
	 0 : GPIO[x] is low (default)
	 1 : GPIO[x] is high
	 */
} GPIODAT_Reg;

typedef struct GPIOCON_Reg
{
	GPIOCON_Reg(uint8_t raw)
	{
		RESERVED = (raw >> 4) & 0x0F;
		CON = (raw >> 0) & 0x0F;
	}

	uint8_t get()
	{
		uint8_t retval = 0x00;
		retval |= (CON << 0);
		retval |= (RESERVED << 4);
		return retval;
	}

	uint8_t RESERVED :4; // Always write 0h
	uint8_t CON :4;
	/*
	 * GPIO pin configuration
	 Configures the GPIO[x] pin as an analog input or GPIO. CON[x]
	 corresponds to the GPIO[x] pin.
	 0 : GPIO[x] configured as analog input (default)(1)
	 1 : GPIO[x] configured as GPIO
	 */
} GPIOCON_Reg;

typedef struct OFCAL_Reg
{
	union OFCAL
	{
		uint32_t offset;
		uint8_t raw[3];
	};
} OFCAL_Reg;
