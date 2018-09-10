//Yet Another Arduino ams AS3935 'Franklin' lightning sensor library 
// Copyright (c) 2018 Gregor Christandl <christandlg@yahoo.com>
// home: https://bitbucket.org/christandlg/as3935
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

#include "AS3935MI.h"

SPISettings AS3935SPI::spi_settings_ = SPISettings(2000000, MSBFIRST, SPI_MODE1);

AS3935MI::AS3935MI(uint8_t irq) :
	irq_(irq)
{
}

AS3935MI::~AS3935MI()
{
}

uint8_t AS3935MI::readStormDistance()
{
	return readRegister(AS3935_REGISTER_DISTANCE, AS3935_MASK_DISTANCE);
}

uint8_t AS3935MI::readInterruptSource()
{
	return readRegister(AS3935_REGISTER_INT, AS3935_MASK_INT);
}

bool AS3935MI::readPowerDown()
{
	return (readRegister(AS3935_REGISTER_PWD, AS3935_MASK_PWD) == 1 ? true : false);
}

void AS3935MI::writePowerDown(bool enabled)
{
	writeRegister(AS3935_REGISTER_PWD, AS3935_MASK_PWD, enabled ? 1 : 0);
}

bool AS3935MI::readMaskDisturbers()
{
	return (readRegister(AS3935_REGISTER_MASK_DIST, AS3935_MASK_MASK_DIST) == 1 ? true : false);
}

void AS3935MI::writeMaskDisturbers(bool enabled)
{
	writeRegister(AS3935_REGISTER_MASK_DIST, AS3935_MASK_MASK_DIST, enabled ? 1 : 0);
}

uint8_t AS3935MI::readAFE()
{
	return readRegister(AS3935_REGISTER_AFE_GB, AS3935_MASK_AFE_GB);
}

void AS3935MI::writeAFE(uint8_t afe_setting)
{
	writeRegister(AS3935_REGISTER_AFE_GB, AS3935_MASK_AFE_GB, afe_setting);
}

uint8_t AS3935MI::readNoiseFloorThreshold()
{
	return readRegister(AS3935_REGISTER_NF_LEV, AS3935_MASK_NF_LEV);
}

void AS3935MI::writeNoiseFloorThreshold(uint8_t threshold)
{
	if (threshold > 0x07)
		return;

	writeRegister(AS3935_REGISTER_NF_LEV, AS3935_MASK_NF_LEV, threshold);
}

uint8_t AS3935MI::readWatchdogThreshold()
{
	return readRegister(AS3935_REGISTER_WDTH, AS3935_MASK_WDTH);
}

void AS3935MI::writeWatchdogThreshold(uint8_t threshold)
{
	writeRegister(AS3935_REGISTER_WDTH, AS3935_MASK_WDTH, threshold);
}

uint8_t AS3935MI::readSprikeRejection()
{
	return readRegister(AS3935_REGISTER_SREJ, AS3935_MASK_SREJ);
}

void AS3935MI::writeSpikeRejection(uint8_t threshold)
{
	writeRegister(AS3935_REGISTER_SREJ, AS3935_MASK_SREJ, threshold);
}

uint32_t AS3935MI::readEnergy()
{
	uint32_t energy = 0;
	//from https://www.eevblog.com/forum/microcontrollers/define-mmsbyte-for-as3935-lightning-detector/
	//Reg 0x04: Energy word, bits 0 : 7
	//Reg 0x05 : Energy word, bits 8 : 15
	//Reg 0x06 : Energy word, bits 16 : 20
	//energy |= LSB
	//energy |= (MSB << 8)
	//energy |= (MMSB << 16)
	energy |= static_cast<uint32_t>(readRegister(AS3935_REGISTER_S_LIG_L, AS3935_MASK_S_LIG_L));
	energy |= (static_cast<uint32_t>(readRegister(AS3935_REGISTER_S_LIG_M, AS3935_MASK_S_LIG_M)) << 8);
	energy |= (static_cast<uint32_t>(readRegister(AS3935_REGISTER_S_LIG_MM, AS3935_MASK_S_LIG_MM)) << 16);

	return energy;
}

uint8_t AS3935MI::readAntennaTuning()
{
	uint8_t return_value = readRegister(AS3935_REGISTER_TUN_CAP, AS3935_MASK_TUN_CAP);

	return return_value;
}

void AS3935MI::writeAntennaTuning(uint8_t tuning)
{
	writeRegister(AS3935_REGISTER_TUN_CAP, AS3935_MASK_TUN_CAP, tuning);
}

uint8_t AS3935MI::readDivisionRatio()
{
	return readRegister(AS3935_REGISTER_LCO_FDIV, AS3935_MASK_LCO_FDIV);
}

void AS3935MI::writeDivisionRatio(uint8_t ratio)
{
	writeRegister(AS3935_REGISTER_LCO_FDIV, AS3935_MASK_LCO_FDIV, ratio);
}

uint8_t AS3935MI::readMinLightnings()
{
	return readRegister(AS3935_REGISTER_MIN_NUM_LIGH, AS3935_MASK_MIN_NUM_LIGH);
}

void AS3935MI::writeMinLightnings(uint8_t number)
{
	writeRegister(AS3935_REGISTER_MIN_NUM_LIGH, AS3935_MASK_MIN_NUM_LIGH, number);
}

void AS3935MI::resetToDefaults()
{
	writeRegister(AS3935_REGISTER_PRESET_DEFAULT, AS3935_MASK_PRESET_DEFAULT, AS3935_DIRECT_CMD);

	delayMicroseconds(AS3935_TIMEOUT);
}

bool AS3935MI::calibrateRCO()
{
	//cannot calibrate if in power down mode.
	if (readPowerDown())
		return false;

	//issue calibration command
	writeRegister(AS3935_REGISTER_CALIB_RCO, AS3935_MASK_CALIB_RCO, AS3935_DIRECT_CMD);

	//expose clock on IRQ pin (necessary?)
	writeRegister(AS3935_REGISTER_DISP_SRCO, AS3935_REGISTER_DISP_SRCO, static_cast<uint8_t>(1));

	//wait for calibration to finish...
	delayMicroseconds(AS3935_TIMEOUT);

	//stop exposing clock on IRQ pin
	writeRegister(AS3935_REGISTER_DISP_SRCO, AS3935_REGISTER_DISP_SRCO, static_cast<uint8_t>(0));

	//check calibration results. bits will be set if calibration failed.
	bool success_TRCO = !static_cast<bool>(readRegister(AS3935_REGISTER_TRCO_CALIB_NOK, AS3935_MASK_TRCO_CALIB_NOK));
	bool success_SRCO = !static_cast<bool>(readRegister(AS3935_REGISTER_SRCO_CALIB_NOK, AS3935_MASK_SRCO_CALIB_NOK));

	return (success_TRCO && success_SRCO);
}

bool AS3935MI::calibrateResonanceFrequency()
{
	if (readPowerDown())
		return false;

	writeDivisionRatio(AS3935_DR_16);

	int16_t target = 6250;		//500kHz / 16 * 0.1s * 2 (counting each high-low / low-high transition)
	int16_t best_diff_abs = 32767;
	uint8_t best_i = 0;

	for (uint8_t i = 0; i < 16; i++)
	{
		//set tuning capacitors
		writeAntennaTuning(i);

		delayMicroseconds(AS3935_TIMEOUT);

		//display LCO on IRQ
		writeRegister(AS3935_REGISTER_DISP_LCO, AS3935_MASK_DISP_LCO, 1);

		delayMicroseconds(AS3935_TIMEOUT);

		bool irq_current = digitalRead(irq_);
		bool irq_last = irq_current;

		int16_t counts = 0;

		uint32_t time_start = millis();

		//count transitions for 100ms
		while ((millis() - time_start) < 100)
		{
			irq_current = digitalRead(irq_);

			if (irq_current != irq_last)
				counts++;

			irq_last = irq_current;
		}

		//stop displaying LCO on IRQ
		writeRegister(AS3935_REGISTER_DISP_LCO, AS3935_MASK_DISP_LCO, 0);

		//remember if the current setting was better than the previous
		if (abs(target - counts) < best_diff_abs)
		{
			best_diff_abs = abs(target - counts);
			best_i = i;
		}
	}

	writeAntennaTuning(best_i);

	//return true if the absolute difference between best value and target value is < 3.5% of target value
	return (abs(best_diff_abs) < 218 ? true : false);
}

uint8_t AS3935MI::getMaskShift(uint8_t mask)
{
	uint8_t return_value = 0;

	//count how many times the mask must be shifted right until the lowest bit is set
	if (mask != 0)
	{
		while (!(mask & 1))
		{
			return_value++;
			mask >>= 1;
		}
	}

	return return_value;
}
	
uint8_t AS3935MI::getMaskedBits(uint8_t reg, uint8_t mask)
{
	//extract masked bits
	return ((reg & mask) >> getMaskShift(mask));
}

uint8_t AS3935MI::setMaskedBits(uint8_t reg, uint8_t mask, uint8_t value)
{
	//clear mask bits in register
	reg &= (~mask);
	
	//set masked bits in register according to value
	return ((value << getMaskShift(mask)) & mask) | reg;
}
	
uint8_t AS3935MI::readRegister(uint8_t reg, uint8_t mask)
{
	return getMaskedBits(readData(reg), mask);
}

void AS3935MI::writeRegister(uint8_t reg, uint8_t mask, uint8_t value)
{
	uint8_t reg_val = readData(reg);
	writeData(reg, setMaskedBits(reg_val, mask, value));
}

AS3935I2C::AS3935I2C(uint8_t address, uint8_t irq) :
	AS3935MI(irq),
	address_(address)
{
}

AS3935I2C::~AS3935I2C()
{
}

bool AS3935I2C::begin()
{
	switch (address_)
	{
	case AS3935I2C_A01:
	case AS3935I2C_A10:
	case AS3935I2C_A11:
		break;
	default:
		//return false if an invalid I2C address was given.
		return false;
	}

	resetToDefaults();

	return true;
}

uint8_t AS3935I2C::readData(uint8_t reg)
{
	Wire.beginTransmission(address_);
	Wire.write(reg);
	Wire.endTransmission(false);
	Wire.requestFrom(address_, static_cast<uint8_t>(1));
	
	return Wire.read();
}

void AS3935I2C::writeData(uint8_t reg, uint8_t value)
{
	Wire.beginTransmission(address_);
	Wire.write(reg);
	Wire.write(value);
	Wire.endTransmission();
}

AS3935SPI::AS3935SPI(uint8_t cs, uint8_t irq) :
	AS3935MI(irq),
	cs_(cs)
{
}

AS3935SPI::~AS3935SPI()
{
}

bool AS3935SPI::begin()
{
	pinMode(cs_, OUTPUT);
	digitalWrite(cs_, HIGH);		//deselect

	resetToDefaults();

	return true;
}

uint8_t AS3935SPI::readData(uint8_t reg)
{	
	SPI.beginTransaction(spi_settings_);

	digitalWrite(cs_, LOW);

	SPI.transfer((reg & 0x3F) | 0x40);	//set pin 7 (indicates read)
	
	return SPI.transfer(0);
}

void AS3935SPI::writeData(uint8_t reg, uint8_t value)
{	
	SPI.beginTransaction(spi_settings_);

	digitalWrite(cs_, LOW);

	SPI.transfer((reg & 0x3F));
	SPI.transfer(value);

	digitalWrite(cs_, HIGH);
}