// AS3935MI_LightningDetector_I2C.ino
//
// shows how to use the AS3935MI library with the lightning sensor connected using I2C.
//
// Copyright (c) 2018 Gregor Christandl
//
// connect the AS3935 to the Arduino like this:
//
// Arduino - AS3935
// 5V ------ VCC
// GND ----- GND
// D2 ------ IRQ		must be a pin supporting external interrupts, e.g. D2 or D3 on an Arduino Uno.
// SDA ----- MOSI
// SCL ----- SCL
// 5V ------ SI		(activates I2C for the AS3935)
// 5V ------ A0		(sets the AS3935' I2C address to 0x01)
// GND ----- A1		(sets the AS3935' I2C address to 0x01)
// 5V ------ EN_VREG !IMPORTANT when using 5V Arduinos (Uno, Mega2560, ...)
// other pins can be left unconnected.

#include <Arduino.h>
#include <Wire.h>

#include <AS3935MI.h>

#define PIN_IRQ 2

//create an AS3935 object using the I2C interface, I2C address 0x01 and IRQ pin number 2
AS3935I2C as3935(AS3935I2C::AS3935I2C_A01, PIN_IRQ);

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
		Serial.println("begin() failed. Check the I2C address passed to the AS3935I2C constructor. ");
		while (1);
	}

	//check I2C connection.
	if (!as3935.checkConnection())
	{
		Serial.println("checkConnection() failed. check your I2C connection and I2C Address. ");
		while (1);
	}

	//check the IRQ pin connection.
	if (!as3935.checkIRQ())
	{
		Serial.println("checkIRQ() failed. check if the correct IRQ pin was passed to the AS3935I2C constructor. ");
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
		Serial.println("RCO Calibration failed");
		while (1);
	}
	else
		Serial.println("RCP Calibration succeeded");

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
			Serial.println("Noise floor too high. attempting to increase noise floor threshold. ");

			//if the noise floor threshold setting is not yet maxed out, increase the setting.
			//note that noise floor threshold events can also be triggered by an incorrect
			//analog front end setting.
			if (as3935.increaseNoiesFloorThreshold())
				Serial.println("increased noise floor threshold");
			else
				Serial.println("noise floor threshold already at maximum");
		}

		//send a report if a disturber was detected. if disturbers are masked with as3935.writeMaskDisturbers(true);
		//this event will never be reported.
		else if (event == AS3935MI::AS3935_INT_D)
		{

			Serial.println("Disturber detected. attempting to increase noise floor threshold. ");

			//increasing the Watchdog Threshold and / or Spike Rejection setting improves the AS3935s resistance 
			//against disturbers but also decrease the lightning detection efficiency (see AS3935 datasheet)
			uint8_t wdth = as3935.readWatchdogThreshold();
			uint8_t srej = as3935.readSprikeRejection();

			if ((wdth < AS3935MI::AS3935_WDTH_10) || (srej < AS3935MI::AS3935_SREJ_10))
			{
				//alternatively increase spike rejection and watchdog threshold 
				if (srej < wdth)
				{
					if (as3935.increaseSpikeRejection())
						Serial.println("increased spike rejection ratio");
					else
						Serial.println("spike rejection ratio already at maximum");
				}
				else
				{
					if (as3935.increaseWatchdogThreshold())
						Serial.println("increased watchdog threshold");
					else
						Serial.println("watchdog threshold already at maximum");
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