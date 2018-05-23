#include "AS3935.h"

SPISettings AS3935::spi_settings_ = SPISettings(2000000, MSBFIRST, SPI_MODE1);

AS3935::AS3935(uint8_t interface, uint8_t address, uint8_t irq) :
	interface_(interface),
	address_(address), 
	irq_(irq)
{
}


AS3935::~AS3935()
{
}

bool AS3935::begin()
{
	switch (interface_)
	{
	case AS3935_INTERFACE_I2C:
	{

		switch (address_)
		{
		case AS3935_I2C_A01:
		case AS3935_I2C_A10:
		case AS3935_I2C_A11:
			break;
		default:
			//return false if an invalid I2C address was given.
			return false;
		}
	}
	break;
	case AS3935_INTERFACE_SPI:
	{

		pinMode(address_, OUTPUT);
		digitalWrite(address_, HIGH);		//deselect
	}
	break;
	default:
		return false;
	}

	resetToDefaults();

	if (!calibrateResonanceFrequency())
		return false;

	if (!calibrateRCO())
		return false;

	return true;
}

uint8_t AS3935::getStormDistance()
{
	return readRegister(AS3935_REGISTER_DISTANCE, AS3935_MASK_DISTANCE);
}

uint8_t AS3935::getInterruptSource()
{
	return readRegister(AS3935_REGISTER_INT, AS3935_MASK_INT);
}

bool AS3935::getPowerDown()
{
	return (readRegister(AS3935_REGISTER_PWD, AS3935_MASK_PWD) == 1 ? true : false);
}

void AS3935::setPowerDown(bool enabled)
{
	writeRegister(AS3935_REGISTER_PWD, AS3935_MASK_PWD, enabled ? 1 : 0);
}

bool AS3935::getMaskDisturbers()
{
	return (readRegister(AS3935_REGISTER_MASK_DIST, AS3935_MASK_MASK_DIST) == 1 ? true : false);
}

void AS3935::setMaskDisturbers(bool enabled)
{
	writeRegister(AS3935_REGISTER_MASK_DIST, AS3935_MASK_MASK_DIST, enabled ? 1 : 0);
}

uint8_t AS3935::getAFE()
{
	return readRegister(AS3935_REGISTER_AFE_GB, AS3935_MASK_AFE_GB);
}

void AS3935::setAFE(uint8_t afe_setting)
{
	writeRegister(AS3935_REGISTER_AFE_GB, AS3935_MASK_AFE_GB, afe_setting);
}

uint8_t AS3935::getNoiseFloor()
{
	return readRegister(AS3935_REGISTER_NF_LEV, AS3935_MASK_NF_LEV);
}

uint8_t AS3935::getWatchdogThreshold()
{
	return readRegister(AS3935_REGISTER_WDTH, AS3935_MASK_WDTH);
}

void AS3935::setWatchdogThreshold(uint8_t threshold)
{
	writeRegister(AS3935_REGISTER_WDTH, AS3935_MASK_WDTH, threshold);
}

uint8_t AS3935::getSprikeRejection()
{
	return readRegister(AS3935_REGISTER_SREJ, AS3935_MASK_SREJ);
}

void AS3935::setSpikeRejection(uint8_t srej)
{
	writeRegister(AS3935_REGISTER_SREJ, AS3935_MASK_SREJ, srej);
}

uint32_t AS3935::getEnergy()
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

uint8_t AS3935::getAntennaTuning()
{
	uint8_t return_value = readRegister(AS3935_REGISTER_TUN_CAP, AS3935_MASK_TUN_CAP);

	return return_value;
}

void AS3935::setAntennaTuning(uint8_t tuning)
{
	writeRegister(AS3935_REGISTER_TUN_CAP, AS3935_MASK_TUN_CAP, tuning);
}

uint8_t AS3935::getDivisionRatio()
{
	return readRegister(AS3935_REGISTER_LCO_FDIV, AS3935_MASK_LCO_FDIV);
}

void AS3935::setDivisionRatio(uint8_t ratio)
{
	writeRegister(AS3935_REGISTER_LCO_FDIV, AS3935_MASK_LCO_FDIV, ratio);
}

uint8_t AS3935::getMinLightnings()
{
	return readRegister(AS3935_REGISTER_MIN_NUM_LIGH, AS3935_MASK_MIN_NUM_LIGH);
}

void AS3935::setMinLightnings(uint8_t number)
{
	writeRegister(AS3935_REGISTER_MIN_NUM_LIGH, AS3935_MASK_MIN_NUM_LIGH, number);
}

void AS3935::resetToDefaults()
{
	writeRegister(AS3935_REGISTER_PRESET_DEFAULT, AS3935_MASK_PRESET_DEFAULT, AS3935_DIRECT_CMD);

	delayMicroseconds(AS3935_TIMEOUT);
}

bool AS3935::calibrateRCO()
{
	//cannot calibrate if in power down mode.
	if (getPowerDown())
		return false;

	//disable interrupts
	noInterrupts();

	//issue calibration command
	writeRegister(AS3935_REGISTER_CALIB_RCO, AS3935_MASK_CALIB_RCO, AS3935_DIRECT_CMD);

	//expose clock on IRQ pin (necessary?)
	writeRegister(AS3935_REGISTER_DISP_SRCO, AS3935_REGISTER_DISP_SRCO, static_cast<uint8_t>(1));

	//wait for calibration to finish...
	delayMicroseconds(AS3935_TIMEOUT);

	//stop exposing clock on IRQ pin
	writeRegister(AS3935_REGISTER_DISP_SRCO, AS3935_REGISTER_DISP_SRCO, static_cast<uint8_t>(0));

	//reenable interrupts
	interrupts();

	//check calibration results. bits will be set if calibration failed.
	bool success_TRCO = !static_cast<bool>(readRegister(AS3935_REGISTER_TRCO_CALIB_NOK, AS3935_MASK_TRCO_CALIB_NOK));
	bool success_SRCO = !static_cast<bool>(readRegister(AS3935_REGISTER_SRCO_CALIB_NOK, AS3935_MASK_SRCO_CALIB_NOK));

	return (success_TRCO && success_SRCO);
}

bool AS3935::calibrateResonanceFrequency()
{
	if (getPowerDown())
		return false;

	setDivisionRatio(AS3935_DR_128);

	int16_t target = 781;		//500kHz / 16 * 0.1s * 2 (counting each high-low / low-high transition)
	int16_t best_diff_abs = 32767;
	uint8_t best_i = 0;

	for (uint8_t i = 0; i < 16; i++)
	{
		//set tuning capacitors
		setAntennaTuning(i);

		delayMicroseconds(AS3935_TIMEOUT);

		//display TCRO on IRQ
		writeRegister(AS3935_REGISTER_DISP_TRCO, AS3935_MASK_DISP_TRCO, 1);

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

		//stop displaying TCRO on IRQ
		writeRegister(AS3935_REGISTER_DISP_TRCO, AS3935_MASK_DISP_TRCO, 0);

		delayMicroseconds(AS3935_TIMEOUT);

		//remember if the current setting was better than the previous
		if (abs(target - counts) < best_diff_abs)
		{
			best_diff_abs = abs(target - counts);
			best_i = i;
		}

		delayMicroseconds(AS3935_TIMEOUT);

		Serial.print(getAntennaTuning(), DEC);
		Serial.print(" - ");
		Serial.print(target);
		Serial.print("vs.");
		Serial.println(counts);
		Serial.println(target - counts);
	}

	setAntennaTuning(best_i);

	//return true if the absolute difference between best value and target value is < 3.5% of target value
	return (abs(best_diff_abs) < 218 ? true : false);
}

uint8_t AS3935::getMaskShift(uint8_t mask)
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

uint8_t AS3935::readRegister(uint8_t reg, uint8_t mask)
{
	uint8_t return_value = 0;

	if (interface_ == AS3935_INTERFACE_I2C)
	{
		Wire.beginTransmission(address_);
		Wire.write(reg);
		Wire.endTransmission(false);
		Wire.requestFrom(address_, static_cast<uint8_t>(1));

		return_value = Wire.read();
	}
	else //if (interface_ == AS3935_INTERFACE_SPI)
	{
		SPI.beginTransaction(spi_settings_);

		digitalWrite(address_, LOW);

		SPI.transfer((reg & 0x3F) | 0x40);	//set pin 7 (indicates read)
		return_value = SPI.transfer(0);

		digitalWrite(address_, HIGH);
	}

	return_value &= mask;

	return_value >>= getMaskShift(mask);

	return return_value;
}

void AS3935::writeRegister(uint8_t reg, uint8_t mask, uint8_t value)
{

	uint8_t reg_val = readRegister(reg, 0xFF);

	//clear masked bits.
	reg_val &= (~mask);

	value <<= getMaskShift(mask);

	value &= mask;

	reg_val |= value;

	if (interface_ == AS3935_INTERFACE_I2C)
	{
		Wire.beginTransmission(address_);
		Wire.write(reg);
		Wire.write(reg_val);
		Wire.endTransmission();
	}
	else //if (interface_ == AS3935_INTERFACE_SPI)
	{
		SPI.beginTransaction(spi_settings_);

		digitalWrite(address_, LOW);

		SPI.transfer((reg & 0x3F));
		SPI.transfer(reg_val);

		digitalWrite(address_, HIGH);
	}
}
