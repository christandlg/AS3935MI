// AS3935_LightningDetector_I2C.ino
//
// shows how to use the AS3935 library with the lightning sensor connected using I2C.
//
// Copyright (c) 2018 Gregor Christandl
//
// connect the AS3935 to the Arduino like this:
//
// Arduino | AS3935
// 5V - VCC
// GND - GND
// D2 - IRQ		must be a pin supporting external interrupts, e.g. D2 or D3 on an Arduino Uno.
// SDA - MOSI
// SCL - SCL
// 5V - SI		(activates I2C for the AS3935)
// 5V - A0		(sets the AS3935' I2C address to 0x01
// other pins can be left unconnected.

#include <Arduino.h>
#include <Wire.h>

#include <AS3935.h>

#define PIN_IRQ 2

//create an AS3935 object using the I2C interface, I2C address 0x01 and IRQ pin number 2
AS3935 as3935(AS3935::AS3935_INTERFACE_I2C, AS3935::AS3935_I2C_A01, PIN_IRQ);

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

	Wire.begin();

	//begin() checks the Interface and I2C Address passed to the constructor and resets the AS3935 to 
	//default values.
	if (!as3935.begin())
	{
		Serial.println("begin() failed. check your AS3935 Interface and I2C Address.");
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

	//calibrate the RCO.
	if (!as3935.calibrateRCO())
	{
		Serial.println("RCP Calibration failed");
		while (1);
	}

	//set the analog front end to 'indoors'
	as3935.setAFE(AS3935::AS3935_INDOORS);

	//set default value for noise floor threshold
	as3935.setNoiseFloorThreshold(AS3935::AS3935_NFL_2);

	//set the default Watchdog Threshold
	as3935.setWatchdogThreshold(AS3935::AS3935_WDTH_2);

	//set the default Spike Rejection 
	as3935.setSpikeRejection(AS3935::AS3935_SREJ_2);

	//set default value for minimum lightnings (1)
	as3935.setMinLightnings(AS3935::AS3935_MNL_1);

	//do not mask disturbers
	as3935.setMaskDisturbers(false);

	//the AS3935 will pull the interrupt pin HIGH when an event is registered and will keep it 
	//pulled high until the event register is read.
	attachInterrupt(digitalPinToInterrupt(PIN_IRQ), AS3935ISR, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:

	if (interrupt_)
	{
		//query the interrupt source from the AS3935
		uint8_t event = as3935.getInterruptSource();

		//send a report if the noise floor is too high. 
		if (event == AS3935::AS3935_INT_NH)
		{
			Serial.print("Noise floor too high");

			//read the currently set noise floor threshold
			uint8_t noise_floor_threshold = as3935.getNoiseFloorThreshold();

			//if the noise floor threshold setting is not yet maxed out, increase the setting.
			//note that noise floor threshold events can also be triggered by an incorrect
			//analog front end setting.
			if (noise_floor_threshold < AS3935::AS3935_NFL_7)
			{
				noise_floor_threshold += 1;
				Serial.print("increasing noise floor threshold to ");
				Serial.println(noise_floor_threshold);

				as3935.setNoiseFloorThreshold(noise_floor_threshold);
			}
			else
			{
				Serial.println("error: already at highest noise floor threshold setting! check the analog front end setting.");
			}
		}

		//send a report if a disturber was detected. if disturbers are masked with as3935.setMaskDisturbers(true);
		//this event will never be reported.
		else if (event == AS3935::AS3935_INT_D)
		{
			Serial.println("Disturber detected");

			//increasing the Watchdog Threshold and / or Spike Rejection setting improves the AS3935s resistance 
			//against disturbers but also decrease the lightning detection efficiency (see AS3935 datasheet)
			uint8_t wdth = as3935.getWatchdogThreshold();
			uint8_t srej = as3935.getSprikeRejection();

			if ((wdth < AS3935::AS3935_WDTH_10) || (srej < AS3935::AS3935_SREJ_10))
			{
				//alternatively increase spike rejection and watchdog threshold 
				if (srej < wdth)
				{
					srej += 1;
					Serial.print("increasing spike rejection ratio setting to: ");
					Serial.println(srej);

					as3935.setSpikeRejection(srej);
				}
				else
				{
					wdth += 1;
					Serial.print("increasing watchdog threshold setting to: ");
					Serial.println(wdth);

					as3935.setWatchdogThreshold(wdth);
				}
			}
			else
			{
				Serial.println("error: Watchdog Threshold and Spike Rejection settings are already maxed out.");
			}
		}

		else if (event == AS3935::AS3935_INT_L)
		{
			Serial.print("Lightning detected! Storm Front is "); 
			Serial.print(as3935.getStormDistance());
			Serial.println("km away.");
		}

		interrupt_ = false;
	}
}

//interrupt service routine. this function is called each time the AS3935 reports an event by pulling 
//the IRQ pin high.
void AS3935ISR()
{
	interrupt_ = true;
}