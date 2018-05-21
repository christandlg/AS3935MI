#ifndef AS3935_H_
#define AS3935_H_

#include <Arduino.h>		//assume arduino version >= 1.0

class AS3935
{
public:
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
	uint8_t readRegister(uint8_t reg);

	bool writeRegister(uint8_t reg, uint8_t value);

	uint8_t interface_;			//

	uint8_t address_;			//I2C address or SPI CS pin

	//uint32_t event_time_;	//system time (in ms) of last interrupt

	//bool event_;
};

#endif /* AS3935_H_ */