#ifndef _BOX_H
#define _BOX_H


#include "boxControl.h"
#include "Config.h"
#include "boxValues.h"
//#include "Conversions.h"

class box : protected boxControl {

public:
	/* Box constructor*/
	box();

	/* Box constructor*/
	~box();

	/* Box init function
	 *  Void return type
	 *  Depends on communications class for sending error codes or debug statements
	 *  Calls initControl to setup pin modes on GPIO expander and Arduino 5V out pin
	 *  Calls setupADC to setup ADC
	*/
	void init();

	/* interrogateBox function
	* return type is boxValues::return data
	* @param byte boxNumber, a byte with range 1-7 containing the number of the box to interrogate.  Function will Coms an error message if boxNumber is out of range and set an error flag in the return type.
	* @param bool boxInverted, a bool set to false if box is approached from knob side, true otherwise.
	* Depends on communications class for sending error codes or debug statements 
	* 
	* Box analysis control flow:
	*		Call checkconfigCorrect to ensure all required init functions have been called.
	*		turn all input off via boxControl::setInput
	*		Create return values variable
	*		Call boxControl::switchControl to set switches
	*		Test properly docked
	*			IF !DOCKED 
	*				Coms not correctly docked error
	*				Return attempt re-dock error flag in returnData to calling function
	*			ELSE
	*				continue
	*		Turn input on via boxControl::setInput
	*		Take readings
	*		Turn input off via boxControl::setInput
	*		Calculate required value
	*		Get preferred value
	*		fill in the relevant variable in returnData
	*		Call boxControl::switchControl to set switches for 2nd stage (if reqd)
	*		Turn input on via boxControl::setInput	(box 5, 6 and 7 flow instead calls measureCapacitance())
	*		Take readings
	*		Turn input off via boxControl::setInput
	*		Calculate required value
	*		Get preferred value
	*		fill in the relevant variable in returnData
	*		return box values to calling function
	*/
	boxValues::returnData interrogateBox(byte boxNumber, bool boxInverted);

	using boxControl::inputStatus;

private:
		//ADC in pin
		const byte _adcInPin = ADCINPUTPIN;
		static bool _boxinitComplete;
		
		/* Capacitance measurement function
		 * returns double representing measured capacitance in nanofarads
		 * Depends on communications class for sending error codes or debug statements 
		 * Calls SetInput and getReadingOnce function.
			*/
		double measureCapacitance();

		/* getReading function 
		 * returns double containing the average of N ADC readings, where N is defined as a const variable in config.h
		 * Calls getReadingOnce function for each reading
		 */
		double getReading();

		double getReadingOnce();

		/*setupADC function
		*  Sets ADC pinMode
		*  Sets ADC prescaler
		*  Calls analogRead once to initialise ADC
		*/
		void setupADC();

		/* checkConfigCorrectfunction
		* checks to ensure all initialization functions have been run,
		*	if any have not been called the an object will be created and the relevant init function run again.
		*	return type void
		*/

		void checkconfigCorrect();

		/* retrieveSettings function
		 *  return type boxConfig::boxSettings
		 *	@param byte boxNumber
		 *	retrieves the appropriate relay configuration from boxConfig
		 *	boxNumber will contain the box number (1-7 in its 3 least significant bits) and have its most significant bit set for analysis stage 2
			*/
		boxConfig::boxSettings retrieveSettings(byte boxNumber);

		/* Docked function
		*	Bool return type
		*	Checks ADC to ensure that the buggy has made good electrical contact with the box
		*	Returns true if sucessful
		*/
		bool docked();
};



#endif