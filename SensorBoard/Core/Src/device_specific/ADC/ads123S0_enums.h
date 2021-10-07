

enum EXCITATION_CURRENT : uint8_t
{
	  OFF	 =	0b0000,
	  _10uA  = 	0b0001,
	  _50uA  = 	0b0010,
	  _100uA = 	0b0011,
	  _250uA =	0b0100,
	  _500uA =	0b0101,
	  _750uA =	0b0110,
	  _1000uA = 0b0111,
	  _1500uA = 0b1000,
	  _2000uA = 0b1001
};


enum REGISTER : uint8_t
{
      ADS124S0_ID = 0x00,
      ADS124S0_STATUS = 0x01,
      ADS124S0_INPMUX = 0x02,
      ADS124S0_PGA = 0x03,
      ADS124S0_DATARATE = 0x04,
      ADS124S0_REF = 0x05,
      ADS124S0_IDACMAG = 0x06,
      ADS124S0_IDACMUX = 0x07,
      ADS124S0_VBIAS = 0x08,
      ADS124S0_SYS = 0x09,
      ADS124S0_OFCAL0 = 0x0A,
      ADS124S0_OFCAL1 = 0x0B,
      ADS124S0_OFCAL2 = 0x0C,
      ADS124S0_FSCAL0 = 0x0D,
      ADS124S0_FSCAL1 = 0x0E,
      ADS124S0_FSCAL2 = 0x0F,
      ADS124S0_GPIODAT = 0x10,
      ADS124S0_GPIOCON = 0x11
};


enum COMMAND : uint8_t
{
  // Control Commands
      NOP = 0x00,
      WAKEUP = 0x02,
      POWERDOWN = 0x04,
      _RESET = 0x06,
      START = 0x08,
      STOP = 0x0A,
  // Calibration Commands
      SYOCAL = 0x16,
      SYGCAL = 0x17,
      SFOCAL = 0x19,
  // Data Read Commands (cant be used with issue command)
      RDATA = 0x12,
  // Read/write Reg Masks (cant be used with issue command)
      RREG,
      WREG
};


enum DEVICE_ID : uint8_t
{
  ADS124S08 = 0x00,
  ADS124S06 = 0x01,
};

