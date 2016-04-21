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

		//Array of possible capacitor values
		const double PCapacitors[17] = { 4.7, 5.6, 6.8, 8.2, 10, 12, 15, 18, //7
			22, 27, 33, 39, 47, 56, 68, 82, 100 }; //16

		//Array of possible resistor values
		const short PResistors[54] = { 20, 22, 24, 27, 30, 33, 36, 39, 43, 47, 51, //10
			56, 62, 68, 75, 82, 91, //16
			100, 110, 120, 130, 150, 160, 180, //23
			200, 220, 240, 270, 300, 330, 360, 390, 430, 470, //33
			510, 560, 620, 680, 750, 820, 910, //40
			1000, 1100, 1200, 1300, 1400, 1500, 1600, 1800, //48
			2000, 2200, 2400, 2700, 3000 }; //53

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