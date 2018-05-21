#ifndef AS3935_H_
#define AS3935_H_

#include <Arduino.h>		//assume arduino version >= 1.0

class AS3935
{
public:
	enum AS3935Registers_t : uint8_t
	{
		AS3935_REGISTER_AFE_GB = 0x00,			//Analog Frontend Gain Boost
		AS3935_REGISTER_PWD = 0x00,				//Power Down
		AS3935_REGISTER_NF_LEV = 0x01,			//Noise Floor Level
		AS3935_REGISTER_WDTH = 0x01,			//Watchdog threshold
		AS3935_REGISTER_CL_STAT = 0x02,			//Clear statistics
		AS3935_REGISTER_MIN_NUM_LIGH = 0x02,	//Minimum number of lightnings
		AS3935_REGISTER_SREJ = 0x02,			//Spike rejection
		AS3935_REGISTER_LCO_FDIV = 0x03,		//Frequency division ratio for antenna tuning
		AS3935_REGISTER_MASK_DIST = 0x03,		//Mask Disturber
		AS3935_REGISTER_INT = 0x03,				//Interrupt
		AS3935_REGISTER_S_LIG_L = 0x04,			//Energy of the Single Lightning LSBYTE
		AS3935_REGISTER_S_LIG_M = 0x05,			//Energy of the Single Lightning MSBYTE
		AS3935_REGISTER_S_LIG_MM = 0x06,		//Energy of the Single Lightning MMSBYTE
		AS3935_REGISTER_DISTANCE = 0x07,		//Distance estimation
		AS3935_REGISTER_DISP_LCO = 0x08,		//Display LCO on IRQ pin
		AS3935_REGISTER_DISP_SRCO = 0x08,		//Display SRCO on IRQ pin
		AS3935_REGISTER_DISP_TRCO = 0x08,		//Display TRCO on IRQ pin
		AS3935_REGISTER_TUN_CAP = 0x08,			//Internal Tuning Capacitors (from 0 to	120pF in steps of 8pF)
		AS3935_REGISTER_TRCO_CALIB_DONE = 0x3A, //Calibration of TRCO done (1=successful)
		AS3935_REGISTER_TRCO_CALIB_NOK = 0x3A,	//Calibration of TRCO unsuccessful (1 = not successful)
		AS3935_REGISTER_SRCO_CALIB_DONE = 0x3B,	//Calibration of SRCO done (1=successful)
		AS3935_REGISTER_SRCO_CALIB_NOK = 0x3B	//Calibration of SRCO unsuccessful (1 = not successful)
	};

	enum AS3935RegisterMask_t : uint8_t
	{
		AS3935_MASK_AFE_GB =			0b00111110,	//Analog Frontend Gain Boost
		AS3935_MASK_PWD =				0b00000001, //Power Down
		AS3935_MASK_NF_LEV =			0b01110000,	//Noise Floor Level
		AS3935_MASK_WDTH =				0b00001111,	//Watchdog threshold
		AS3935_MASK_CL_STAT =			0b01000000,	//Clear statistics
		AS3935_MASK_MIN_NUM_LIGH =		0b00110000,	//Minimum number of lightnings
		AS3935_MASK_SREJ =				0b00001111,	//Spike rejection
		AS3935_MASK_LCO_FDIV =			0b11000000,	//Frequency division ratio for antenna tuning
		AS3935_MASK_MASK_DIST =			0b00100000,	//Mask Disturber
		AS3935_MASK_INT =				0b00001111,	//Interrupt
		AS3935_MASK_S_LIG_L =			0b11111111,	//Energy of the Single Lightning LSBYTE
		AS3935_MASK_S_LIG_M =			0b11111111,	//Energy of the Single Lightning MSBYTE
		AS3935_MASK_S_LIG_MM =			0b00001111,	//Energy of the Single Lightning MMSBYTE
		AS3935_MASK_DISTANCE =			0b00111111,	//Distance estimation
		AS3935_MASK_DISP_LCO =			0b10000000,	//Display LCO on IRQ pin
		AS3935_MASK_DISP_SRCO =			0b01000000,	//Display SRCO on IRQ pin
		AS3935_MASK_DISP_TRCO =			0b00100000,	//Display TRCO on IRQ pin
		AS3935_MASK_TUN_CAP =			0b00001111,	//Internal Tuning Capacitors (from 0 to	120pF in steps of 8pF)
		AS3935_MASK_TRCO_CALIB_DONE =	0b10000000, //Calibration of TRCO done (1=successful)
		AS3935_MASK_TRCO_CALIB_NOK =	0b01000000,	//Calibration of TRCO unsuccessful (1 = not successful)
		AS3935_MASK_SRCO_CALIB_DONE =	0b10000000,	//Calibration of SRCO done (1=successful)
		AS3935_MASK_SRCO_CALIB_NOK =	0b01000000	//Calibration of SRCO unsuccessful (1 = not successful)
	};

	enum AFESetting_t : uint8_t
	{
		AS3935_INDOORS = 0b10010,
		AS3935_OUTDOORS = 0b01110
	};

	enum InterruptName : uint8_t
	{
		AS3935_INT_NH = 0b0001,		//noise level too high
		AS3935_INT_D = 0b0100,		//disturber detected
		AS3935_INT_L = 0b1000		//lightning interrupt
	};

	enum WDTHSetting_t : uint8_t
	{
		AS3935_WDTH_0 = 0b0000,
		AS3935_WDTH_1 = 0b0001,
		AS3935_WDTH_2 = 0b0010,
		AS3935_WDTH_3 = 0b0011,
		AS3935_WDTH_4 = 0b0100,
		AS3935_WDTH_5 = 0b0101,
		AS3935_WDTH_6 = 0b0110,
		AS3935_WDTH_7 = 0b0111,
		AS3935_WDTH_8 = 0b1000,
		AS3935_WDTH_9 = 0b1001,
		AS3935_WDTH_10 = 0b1010
	};

	enum SREJSetting_t : uint8_t
	{
		AS3935_SREJ_0 = 0b0000,
		AS3935_SREJ_1 = 0b0001,
		AS3935_SREJ_2 = 0b0010,
		AS3935_SREJ_3 = 0b0011,
		AS3935_SREJ_4 = 0b0100,
		AS3935_SREJ_5 = 0b0101,
		AS3935_SREJ_6 = 0b0110,
		AS3935_SREJ_7 = 0b0111,
		AS3935_SREJ_8 = 0b1000,
		AS3935_SREJ_9 = 0b1001,
		AS3935_SREJ_10 = 0b1010
	};

	enum NoiseFloorLevel_t : uint8_t
	{
		AS3935_NFL_0 = 0b000,
		AS3935_NFL_1 = 0b001,
		AS3935_NFL_2 = 0b010,		//default
		AS3935_NFL_3 = 0b011,
		AS3935_NFL_4 = 0b100,
		AS3935_NFL_5 = 0b101,
		AS3935_NFL_6 = 0b110,
		AS3935_NFL_7 = 0b111,
	};

	enum MinNumLightnins_t : uint8_t
	{
		AS3935_MNL_1 = 0b00,		//minimum number of lightnings: 1
		AS3935_MNL_5 = 0b01,		//minimum number of lightnings: 5
		AS3935_MNL_9 = 0b10,		//minimum number of lightnings: 9
		AS3935_MNL_16 = 0b11,		//minimum number of lightnings: 16
	};

	enum DivisionRatio_t : uint8_t
	{
		AS3935_DR_16 = 0b00,
		AS3935_DR_32 = 0b01,
		AS3935_DR_64 = 0b10,
		AS3935_DR_128 = 0b11
	};

	enum Interface_t : uint8_t
	{
		AS3935_INTERFACE_I2C = 0,
		AS3935_INTERFACE_SPI = 1
	};

	enum I2CAddress_t : uint8_t
	{
		AS3935_I2C_A01 = 0b01,
		AS3935_I2C_A10 = 0b10,
		AS3935_I2C_A11 = 0b11
	};

	static const uint8_t AS3935_DST_OOR = 0b111111;		//detected lightning was out of range

	static const uint8_t AS3935_DIRECT_CMD = 0x96;

	static const uint32_t AS3935_IRQ_TIMEOUT = 2000;

	AS3935(uint8_t interace, uint8_t address);
	~AS3935();

	bool begin();

	//void run();

	//void setEvent();

	/*
	@return storm distance in km. */
	uint8_t getStormDistance();

	uint8_t getInterruptSource();

	/*
	@return true: powered down, false: powered up. */
	bool getPowerDown();

	/*
	@param enabled: true to power down, false to power up. 
	@return true on success, false otherwise. */
	bool setPowerDown(bool enabled);

	/*
	@return true if disturbers are masked, false otherwise. */
	bool getMaskDisturbers();

	/*
	@param enabled true to mask disturbers, false otherwise. 
	@return true on success, false otherwise. */
	bool setMaskDisturbers(bool enabled);

	/*
	@return AFE setting as AFESetting_t. */
	uint8_t getAFE();

	/*
	@param afe_setting AFE setting as one if AFESetting_t.
	@return true on success, false otherwise. */
	bool setAFE(uint8_t afe_setting);

	/*
	@return current noise floor. */
	uint8_t getNoiseFloor();

	/*
	@return current noise floor threshold. */
	uint8_t getNoiseFloorThreshold();

	/*
	@param noise floor threshold setting. 
	@return true on success, false otherwise. */
	bool setNoiseFloorTrheshold(uint8_t noise_floor);

	/*
	@return current spike rejection setting as SREJSetting_t. */
	uint8_t getSprikeRejection();

	/*
	@param spike rejection setting as SREJSetting_t. 
	@return true on success, false otherwise. */
	bool setSpikeRejection(uint8_t srej);

	/*
	@return lightning energy. no physical meaning. */
	uint32_t getEnergy();		

	/*
	@return antenna tuning*/
	uint8_t getAntennaTuning();

	bool setAntennaTuning(uint8_t tuning);

	uint8_t getDivisionRatio();

	bool setDivisionRatio(uint8_t ratio);

	uint8_t getMinLightnings();

	bool setMinLightnings(uint8_t number);

	/*
	calibrates the AS3935 TCRO.
	@return true on success, false otherwise. */
	bool calibrateTRCO();

	/*
	calibrates the AS3935 antenna's resonance frequency. 
	@return true on success, false on failure or if the resonance frequency could not be tuned
	to within +-3.5% of 500kHz. */
	bool calibrateResonanceFrequency();


private:
	/*
	@param mask
	@return number of bits to shift value so it fits into mask. */
	uint8_t getMaskShift(uint8_t mask);

	uint8_t readRegister(uint8_t reg, uint8_t mask);

	bool writeRegister(uint8_t reg, uint8_t mask, uint8_t value);

	uint8_t interface_;			//

	uint8_t address_;			//I2C address or SPI CS pin
};

#endif /* AS3935_H_ */