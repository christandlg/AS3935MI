//Yet Another Arduino ams AS3935 'Franklin' lightning sensor library 
// Copyright (c) 2018 Gregor Christandl <christandlg@yahoo.com>
// home: https://bitbucket.org/christandlg/as3935mi
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include "AS3935SPIClass.h"

AS3935SPIClass::AS3935SPIClass(uint8_t cs, uint8_t irq) : 
	AS3935MI(irq),
	spi_(spi),
	cs_(cs)
{
}

AS3935SPIClass::~AS3935SPIClass()
{
	spi_ = nullptr;
}

bool AS3935SPIClass::beginInterface()
{
	if (!spi_)
		return false;

	pinMode(cs_, OUTPUT);
	digitalWrite(cs_, HIGH);		//deselect

	return true;
}

uint8_t AS3935SPIClass::readRegister(uint8_t reg)
{
	if (!spi_)
		return 0;

	uint8_t return_value = 0;

	spi_->beginTransaction(spi_settings_);

	digitalWrite(cs_, LOW);				//select sensor

	spi_->transfer((reg & 0x3F) | 0x40);	//select register and set pin 7 (indicates read)

	return_value = spi_->transfer(0);

	digitalWrite(cs_, HIGH);			//deselect sensor

	return return_value;
}

void AS3935SPIClass::writeRegister(uint8_t reg, uint8_t value)
{
	if (!spi_)
		return;

	spi_->beginTransaction(spi_settings_);

	digitalWrite(cs_, LOW);				//select sensor

	spi_->transfer((reg & 0x3F));			//select regsiter 
	spi_->transfer(value);

	digitalWrite(cs_, HIGH);			//deselect sensor
}
