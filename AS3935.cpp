#include "AS3935.h"



AS3935::AS3935(uint8_t interface, uint8_t address) : 
	interface_(interface),
	address_(address)
{
}


AS3935::~AS3935()
{
}

bool AS3935::begin()
{
	if (interface_ == AS3935_INTERFACE_I2C)
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
	else //if (interface_ == AS3935_INTERFACE_SPI)
	{
		pinMode(address_, OUTPUT);

	}


}

//void AS3935::run()
//{
//	if (!event_ && interrupt_ != 255 && digitalRead(interrupt_))
//		setEvent();
//
//	if (!event_)
//		return;
//
//	//AS3935 datasheet p34: "After the signal IRQ goes high the external unit should 
//	//wait 2ms before reading the interrupt register."
//	if (millis() - event_time_ < AS3935_IRQ_TIMEOUT)
//		return;
//
//	uint8_t interrupt_source = getInterruptSource();
//
//	switch (interrupt_source)
//	{
//	case AS3935_INT_NH:
//	{
//		uint8_t nf = getNoiseFloor();
//
//		if (nf < AS3935_NFL_7)
//			setNoiseFloor(nf++);
//	}
//	break;
//	case AS3935_INT_D:
//	{
//		//TODO
//	}
//	break;
//	case AS3935_INT_L:
//	{
//		//TODO
//	}
//	break;
//	}
//
//	event_ = false;
//}

//void AS3935::setEvent()
//{
//	event_ = true;
//	event_time_ = millis();
//}

uint8_t AS3935::getStormDistance()
{
	return uint8_t();
}

uint8_t AS3935::getInterruptSource()
{
	return uint8_t();
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
}

uint8_t AS3935::readRegister(uint8_t reg)
{
	uint8_t return_value = 0;

	if (interface_ == AS3935_INTERFACE_I2C)
	{

	}
	else //if (interface_ == AS3935_INTERFACE_SPI)
	{

	}
}

bool AS3935::writeRegister(uint8_t reg, uint8_t value)
{
	if (interface_ == AS3935_INTERFACE_I2C)
	{

	}
	else //if (interface_ == AS3935_INTERFACE_SPI)
	{

	}
}
