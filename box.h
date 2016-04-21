#ifndef _BOX_H
#define _BOX_H

#include <Adafruit_MCP23017.h>
#include "arduino2.h"
#include "Config.h"
#include "boxValues.h"

class box {

public:
	/* Box constructor*/
	box();

	/* Box destructor*/
	~box();

	/*Flow:
		//getReading start
	*	Set pins
	*	Set input
	*	Get reading
	*	Unset input
	*	Unset pins
		//getReading end
	*	Calculate
	*	Prefer
	*	Send
	*	Set pins
	*	Set input
	*	Get reading
	*	Unset input
	*	Calculate
	*	Prefer
	*	Send
	*	Return
	*
	*
	* OR
	*	Set pins
	*	Set input
	*	Get reading
	*	Unset input
	*	Calculate
	*	Prefer
	*	Send
	*	measureCapacitance
	*	Prefer
	*	Send
	*	Return
	*/
	bool interrogateBox();

	bool docked();

	bool begin(uint8_t boxNumber, bool boxInverted);

private:
		Adafruit_MCP23017 boxGPIO;

		uint8_t _boxNumber = 0;
		
		bool _boostConverterOn = false;
		bool _previouslyCalled = false;

		uint8_t _adcPin = P2PIN;

		bC::pinSettings P1pin[2] = { bC::input };
		bC::pinSettings P2pin[2] = { bC::input };
		bC::pinSettings GNDpin[2] = { bC::input };
		bC::pinSettings Rkpin[2] = { bC::input };

		boxValues::returnData presentationData = { 0 };

		/// <summary>
		/// Function to pick the prefered resistor from a given resistor value range based on the box number.
		/// </summary>
		/// <param name="OResistor"></param>
		/// <returns></returns>
		short toPreferredResistor(double OResistor);

		/// <summary>
		/// Function to pick the prefered Capacitor from a given capacitor value range based on the box number.
		/// </summary>
		/// <param name="OCap"></param>
		/// <returns></returns>
		double toPreferredCapacitor(double OCap);

		double calculateFrequency();

		/// <summary>
		/// returns one ADC reading of the passed pin
		/// </summary>
		/// <param name="pin">pin from which to read</param>
		/// <returns>raw value read</returns>
		short getRawReading(uint8_t pin);

		/// <summary>
		/// Converts ADC value into a voltage
		/// </summary>
		/// <param name="raw">Raw ADC value</param>
		/// <returns>Voltage resulting from raw ADC value</returns>
		double convertToVoltage(short raw);

		/// <summary>
		/// Sets up all pins, measures voltage, unsets pins, returns voltage
		/// returns an average voltage from a number of ADC readings of the passed pin
		/// number of adc readings is determined by a define in config
		/// </summary>
		/// <param name="stage">analysis stage</param>
		/// <returns>average voltage observed</returns>
		double getReading(bool stage);
		
		/// <summary>
		/// Set boost converter relay, delay and turn boost converter on or off
		/// </summary>
		/// <param name="state">Boost converter state</param>
		void setBoostConverter(bC::inputStatus state);

		/// <summary>
		/// Sets up the analog pin directions of the arduino for analysis
		/// </summary>
		/// <param name="stage">0 for stage 1, 1 for stage 2</param>
		void configureForAnalysis(bool stage);

		/// <summary>
		/// Turns analog outputs on or off
		/// </summary>
		/// <param name="state"></param>
		/// <param name="stage"></param>
		void toggleOutputs(bC::inputStatus state, bool stage);


		double calculateResistorValue(double rawValue, bool stage);

		
		double measureCapacitance();
};




#endif