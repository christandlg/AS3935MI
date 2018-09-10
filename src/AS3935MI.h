//Yet Another Arduino ams AS3935 'Franklin' lightning sensor library 
// Copyright (c) 2018 Gregor Christandl <christandlg@yahoo.com>
// home: https://bitbucket.org/christandlg/as3935mi
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef AS3935MI_H_
#define AS3935MI_H_

#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>

class AS3935MI
{
public:
	enum AFESetting_t : uint8_t
	{
		AS3935_INDOORS = 0b10010,
		AS3935_OUTDOORS = 0b01110
	};

	enum InterruptName_t : uint8_t
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

	enum MinNumLightnings_t : uint8_t
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

	static const uint8_t AS3935_DST_OOR = 0b111111;		//detected lightning was out of range

	AS3935MI(uint8_t irq);
	virtual ~AS3935MI();

	virtual bool begin() = 0;

	/*
	@return storm distance in km. */
	uint8_t readStormDistance();

	/*
	@return interrupt source as AS9395::InterruptName_t. */
	uint8_t readInterruptSource();

	/*
	@return true: powered down, false: powered up. */
	bool readPowerDown();

	/*
	@param enabled: true to power down, false to power up. */
	void writePowerDown(bool enabled);

	/*
	@return true if disturbers are masked, false otherwise. */
	bool readMaskDisturbers();

	/*
	@param enabled true to mask disturbers, false otherwise. */
	void writeMaskDisturbers(bool enabled);

	/*
	@return AFE setting as AFESetting_t. */
	uint8_t readAFE();

	/*
	@param afe_setting AFE setting as one if AFESetting_t. */
	void writeAFE(uint8_t afe_setting);

	/*
	@return current noise floor. */
	uint8_t readNoiseFloorThreshold();

	void writeNoiseFloorThreshold(uint8_t threshold);

	/*
	@return current noise floor threshold. */
	uint8_t readWatchdogThreshold();

	/*
	@param noise floor threshold setting. */
	void writeWatchdogThreshold(uint8_t noise_floor);

	/*
	@return current spike rejection setting as SREJSetting_t. */
	uint8_t readSprikeRejection();

	/*
	@param spike rejection setting as SREJSetting_t. */
	void writeSpikeRejection(uint8_t threshold);

	/*
	@return lightning energy. no physical meaning. */
	uint32_t readEnergy();

	/*
	@return antenna tuning*/
	uint8_t readAntennaTuning();

	void writeAntennaTuning(uint8_t tuning);

	uint8_t readDivisionRatio();

	void writeDivisionRatio(uint8_t ratio);

	/*
	get the currently set minimum number of lightnings in the last 15 minues before lightning interrupts are issued, as MinNumLightnings_t. */
	uint8_t readMinLightnings();

	/*
	@param minimum number of lightnings in the last 15 minues before lightning interrupts are issued, as MinNumLightnings_t. */
	void writeMinLightnings(uint8_t number);

	/*
	resets all registers to default values. */
	void resetToDefaults();

	/*
	calibrates the AS3935 TCRO accordingto procedure in AS3935 datasheet p36. must be done *after* calibrating the resonance frequency.
	@return true on success, false otherwise. */
	bool calibrateRCO();

	/*
	calibrates the AS3935 antenna's resonance frequency. 
	@return true on success, false on failure or if the resonance frequency could not be tuned
	to within +-3.5% of 500kHz. */
	bool calibrateResonanceFrequency();

private:
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
		AS3935_REGISTER_SRCO_CALIB_NOK = 0x3B,	//Calibration of SRCO unsuccessful (1 = not successful)
		AS3935_REGISTER_PRESET_DEFAULT = 0x3C,	//Sets all registers in default mode
		AS3935_REGISTER_CALIB_RCO = 0x3D		//Sets all registers in default mode
	};

	enum AS3935RegisterMask_t : uint8_t
	{
		AS3935_MASK_AFE_GB =				0b00111110,	//Analog Frontend Gain Boost
		AS3935_MASK_PWD =					0b00000001, //Power Down
		AS3935_MASK_NF_LEV =				0b01110000,	//Noise Floor Level
		AS3935_MASK_WDTH =					0b00001111,	//Watchdog threshold
		AS3935_MASK_CL_STAT =				0b01000000,	//Clear statistics
		AS3935_MASK_MIN_NUM_LIGH =			0b00110000,	//Minimum number of lightnings
		AS3935_MASK_SREJ =					0b00001111,	//Spike rejection
		AS3935_MASK_LCO_FDIV =				0b11000000,	//Frequency division ratio for antenna tuning
		AS3935_MASK_MASK_DIST =				0b00100000,	//Mask Disturber
		AS3935_MASK_INT =					0b00001111,	//Interrupt
		AS3935_MASK_S_LIG_L =				0b11111111,	//Energy of the Single Lightning LSBYTE
		AS3935_MASK_S_LIG_M =				0b11111111,	//Energy of the Single Lightning MSBYTE
		AS3935_MASK_S_LIG_MM =				0b00001111,	//Energy of the Single Lightning MMSBYTE
		AS3935_MASK_DISTANCE =				0b00111111,	//Distance estimation
		AS3935_MASK_DISP_LCO =				0b10000000,	//Display LCO on IRQ pin
		AS3935_MASK_DISP_SRCO =				0b01000000,	//Display SRCO on IRQ pin
		AS3935_MASK_DISP_TRCO =				0b00100000,	//Display TRCO on IRQ pin
		AS3935_MASK_TUN_CAP =				0b00001111,	//Internal Tuning Capacitors (from 0 to	120pF in steps of 8pF)
		AS3935_MASK_TRCO_CALIB_DONE =		0b10000000, //Calibration of TRCO done (1=successful)
		AS3935_MASK_TRCO_CALIB_NOK =		0b01000000,	//Calibration of TRCO unsuccessful (1 = not successful)
		AS3935_MASK_SRCO_CALIB_DONE =		0b10000000,	//Calibration of SRCO done (1=successful)
		AS3935_MASK_SRCO_CALIB_NOK =		0b01000000,	//Calibration of SRCO unsuccessful (1 = not successful)
		AS3935_MASK_PRESET_DEFAULT =	0b11111111,	//Sets all registers in default mode
		AS3935_MASK_CALIB_RCO =			0b11111111	//Sets all registers in default mode
	};

	/*
	@param mask
	@return number of bits to shift value so it fits into mask. */
	uint8_t getMaskShift(uint8_t mask);
	
	/*
	@param register value of register.
	@param mask mask of value in register
	@return value of masked bits. */
	uint8_t getMaskedBits(uint8_t reg, uint8_t mask);
	
	/*
	@param register value of register
	@param mask mask of value in register
	@param value value to write into masked area
	@param register value with masked bits set to value. */
	uint8_t setMaskedBits(uint8_t reg, uint8_t mask, uint8_t value);
	
	/*
	reads the masked value from the register. 
	@param reg register to read.
	@param mask mask of value.
	@return masked value in register. */
	uint8_t readRegister(uint8_t reg, uint8_t mask);
	
	/*
	sets values in a register. 
	@param reg register to set values in
	@param mask bits of register to set value in
	@param value value to set */
	void writeRegister(uint8_t reg, uint8_t mask, uint8_t value);

	/*
	reads a register from the sensor. must be overwritten by derived classes.
	@param reg register to read. 
	@return register content*/
	virtual uint8_t readData(uint8_t reg) = 0;

	/*
	writes a register to the sensor. must be overwritten by derived classes. 
	@param reg register to write to. 
	@param value value to write to register. */
	virtual void writeData(uint8_t reg, uint8_t value) = 0;	

	static const uint8_t AS3935_DIRECT_CMD = 0x96;

	static const uint32_t AS3935_TIMEOUT = 2000;

	uint8_t irq_;				//interrupt pin
};

class AS3935I2C : public AS3935MI
{
	public:
		enum I2CAddress_t : uint8_t
		{
			AS3935I2C_A01 = 0b01,
			AS3935I2C_A10 = 0b10,
			AS3935I2C_A11 = 0b11
		};
	
		AS3935I2C(uint8_t address, uint8_t irq);
		virtual ~AS3935I2C();
		
		virtual bool begin();
		
	private:
		uint8_t readData(uint8_t reg);

		void writeData(uint8_t reg, uint8_t value);
		
		uint8_t address_;
};

class AS3935SPI : public AS3935MI
{
	public:
		AS3935SPI(uint8_t cs, uint8_t irq);
		virtual ~AS3935SPI();
		
		virtual bool begin();
		
	private:
		uint8_t readData(uint8_t reg);

		void writeData(uint8_t reg, uint8_t value);
		
		uint8_t cs_;

		static SPISettings spi_settings_;     //spi settings object. is the same for all AS3935 sensors
		
};

#endif /* AS3935_H_ */