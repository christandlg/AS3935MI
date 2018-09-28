// AS3935_LightningDetector_SPI.ino
//
// shows how to use the AS3935 library interfaces other than the native I2C or SPI interfaces. 
// here, the second I2C port of an Arduino Due is used (Wire1)
//
// Copyright (c) 2018 Gregor Christandl
//
// connect the AS3935 to the Arduino Due like this:
//
// Arduino - AS3935
// 3.3V ---- VCC
// GND ----- GND
// D2 ------ IRQ		must be a pin supporting external interrupts, e.g. D2 or D3 on an Arduino Uno.
// SDA1 ---- MOSI
// SCL1 ---- SCL
// 5V ------ SI		(activates I2C for the AS3935)
// 5V ------ A0		(sets the AS3935' I2C address to 0x01)
// GND ----- A1		(sets the AS3935' I2C address to 0x01)
// other pins can be left unconnected.

#include <Arduino.h>

#include <Wire.h>

#include <AS3935MI.h>

#define PIN_IRQ 2

//class derived from AS3935MI that implements communication via an interface other than native I2C or SPI. 
class AS3935Wire1 : public AS3935MI
{
	public:	
		//constructor of the derived class. in this case, only 2 parameters are needed
		//@param address i2c address of the sensor.
		//@param irq input pin the sensors irq pin is connected to. this parameter is passed to the constructor of the parent class (AS3935MI)
		AS3935Wire1(uint8_t address, uint8_t irq) : 
		AS3935MI(irq),		//AS3935MI does not have a default constructor therefore the constructor must be called explicitly. it takes the irq pin number as an argument.
		address_(address)	//initialize the AS3935Wire1 classes private member address_ to the i2c address provided
		{
			//nothing else to do here...
		}
		
		//this function must be implemented by derived classes. it is used to initialize the interface. 
		//@return true if the interface was initializes successfully, false otherwise. 
		bool beginInterface()
		{
			//check if a valid i2c address for AS3935 lightning sensors has been provided.
			switch (address_)
			{
			case 0x01:
			case 0x02:
			case 0x03:
				break;		//exit the switch statement 
			default:
				//return false if an invalid I2C address was given.
				return false;
			}

			return true;
		}
	
	private:
		//this function must be implemented by derived classes. this function is responsible for reading data from the sensor. 
		//@param reg register to read. 
		//@return read data (1 byte).
		uint8_t readRegister(uint8_t reg)
		{
		#if defined(ARDUINO_SAM_DUE)
			//workaround for Arduino Due. The Due seems not to send a repeated start with the code below, so this 
			//undocumented feature of Wire::requestFrom() is used. can be used on other Arduinos too (tested on Mega2560)
			//see this thread for more info: https://forum.arduino.cc/index.php?topic=385377.0
			Wire1.requestFrom(address_, 1, reg, 1, true);
		#else
			Wire1.beginTransmission(address_);
			Wire1.write(reg);
			Wire1.endTransmission(false);
			Wire1.requestFrom(address_, static_cast<uint8_t>(1));
		#endif
			
			return Wire1.read();
		}

		//this function must be implemented by derived classes. this function is responsible for sending data to the sensor. 
		//@param reg register to write to.
		//@param data data to write to register.
		void writeRegister(uint8_t reg, uint8_t data)
		{
			Wire1.beginTransmission(address_);
			Wire1.write(reg);
			Wire1.write(data);
			Wire1.endTransmission();
		}
		
		uint8_t address_;		//i2c address of sensor
};

//create an AS3935 object using the Wire1 interface, I2C address 0x01 and IRQ pin number 2
AS3935Wire1 as3935(AS3935I2C::AS3935I2C_A01, PIN_IRQ);

//this value will be set to true by the AS3935 interrupt service routine.
volatile bool interrupt_ = false;

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);

	//wait for serial connection to open (only necessary on some boards)
	while (!Serial);

	//set the IRQ pin as an input pin. do not use INPUT_PULLUP - the AS3935 will pull the pin 
	//high if an event is registered.
	pinMode(PIN_IRQ, INPUT);

	Wire1.begin();

	//begin() checks the Interface passed to the constructor and resets the AS3935 to 
	//default values.
	if (!as3935.begin())
	{
		Serial.println("begin() failed. check your AS3935 Interface setting.");
		while (1);
	}

	//calibrate the resonance frequency. if this fails, check if the AS3935s IRQ pin is 
	//connected to the correct pin on the Arduino. resonance frequency calibration will 
	//take about 1.7 seconds to complete.
	if (!as3935.calibrateResonanceFrequency())
	{
		Serial.println("Resonance Frequency Calibration failed");
		while (1);
	}
	else
		Serial.println("Resonance Frequency Calibration succeeded");


	//calibrate the RCO.
	if (!as3935.calibrateRCO())
	{
		Serial.println("RCP Calibration failed");
		while (1);
	}
	else
		Serial.println("RCO Calibration succeeded");

	//set the analog front end to 'indoors'
	as3935.writeAFE(AS3935MI::AS3935_INDOORS);

	//set default value for noise floor threshold
	as3935.writeNoiseFloorThreshold(AS3935MI::AS3935_NFL_2);

	//set the default Watchdog Threshold
	as3935.writeWatchdogThreshold(AS3935MI::AS3935_WDTH_2);

	//set the default Spike Rejection 
	as3935.writeSpikeRejection(AS3935MI::AS3935_SREJ_2);

	//write default value for minimum lightnings (1)
	as3935.writeMinLightnings(AS3935MI::AS3935_MNL_1);

	//do not mask disturbers
	as3935.writeMaskDisturbers(false);

	//the AS3935 will pull the interrupt pin HIGH when an event is registered and will keep it 
	//pulled high until the event register is read.
	attachInterrupt(digitalPinToInterrupt(PIN_IRQ), AS3935ISR, RISING);

	Serial.println("Initialization complete, waiting for events...");
}

void loop() {
	// put your main code here, to run repeatedly:

	if (interrupt_)
	{
		//the Arduino should wait at least 2ms after the IRQ pin has been pulled high
		delay(2);

		//reset the interrupt variable
		interrupt_ = false;

		//query the interrupt source from the AS3935
		uint8_t event = as3935.readInterruptSource();

		//send a report if the noise floor is too high. 
		if (event == AS3935MI::AS3935_INT_NH)
		{
			Serial.println("Noise floor too high");

			//read the currently set noise floor threshold
			uint8_t noise_floor_threshold = as3935.readNoiseFloorThreshold();

			//if the noise floor threshold setting is not yet maxed out, increase the setting.
			//note that noise floor threshold events can also be triggered by an incorrect
			//analog front end setting.
			if (noise_floor_threshold < AS3935MI::AS3935_NFL_7)
			{
				noise_floor_threshold += 1;
				Serial.print("increasing noise floor threshold to ");
				Serial.println(noise_floor_threshold);

				as3935.writeNoiseFloorThreshold(noise_floor_threshold);
			}
			else
			{
				Serial.println("error: already at highest noise floor threshold setting! check the analog front end setting.");
			}
		}

		//send a report if a disturber was detected. if disturbers are masked with as3935.writeMaskDisturbers(true);
		//this event will never be reported.
		else if (event == AS3935MI::AS3935_INT_D)
		{
			Serial.println("Disturber detected");

			//increasing the Watchdog Threshold and / or Spike Rejection setting improves the AS3935s resistance 
			//against disturbers but also decrease the lightning detection efficiency (see AS3935 datasheet)
			uint8_t wdth = as3935.readWatchdogThreshold();
			uint8_t srej = as3935.readSprikeRejection();

			if ((wdth < AS3935MI::AS3935_WDTH_10) || (srej < AS3935MI::AS3935_SREJ_10))
			{
				//alternatively increase spike rejection and watchdog threshold 
				if (srej < wdth)
				{
					srej += 1;
					Serial.print("increasing spike rejection ratio setting to: ");
					Serial.println(srej);

					as3935.writeSpikeRejection(srej);
				}
				else
				{
					wdth += 1;
					Serial.print("increasing watchdog threshold setting to: ");
					Serial.println(wdth);

					as3935.writeWatchdogThreshold(wdth);
				}
			}
			else
			{
				Serial.println("error: Watchdog Threshold and Spike Rejection settings are already maxed out.");
			}
		}

		else if (event == AS3935MI::AS3935_INT_L)
		{
			Serial.print("Lightning detected! Storm Front is ");
			Serial.print(as3935.readStormDistance());
			Serial.println("km away.");
		}
	}
}

//interrupt service routine. this function is called each time the AS3935 reports an event by pulling 
//the IRQ pin high.
void AS3935ISR()
{
	interrupt_ = true;
}