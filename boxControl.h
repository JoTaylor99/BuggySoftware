#ifndef _BOXCONTROL_H
#define _BOXCONTROL_H


//#include <bitset.h>

#include "Config.h"
#include <stdint.h>
#include <Adafruit_MCP23017.h>

class boxControl {

//All functions of boxControl are protected as they should only be accessible to a box object
protected:

	/*boxControl constructor*/
	boxControl();

	/*boxControl destructor*/
	~boxControl();


//enum for box driving voltage control
	enum inputStatus {
		OFF = 0,
		ON = 1
	};
	/* initControl function
	*  Void return type
	*  Depends on communications class for sending error codes or debug statements
	*  Create switcher object and calls initSwitcher to setup GPIO expander
	*  Sets pinMode for 5V stimulus pin
	*  Sets static bool initComplete to true.
	*/
	void initControl();

	bool switchControl(boxConfig::boxSettings boxSetting);

	/* setInput function
	*	Return type void
	*	@param boxControl::inputStatus position is an enumeration with options ON or OFF
	*	OVERLOADED function: @param short boxNumber, this function should only ever be called for the case of boxNumber = 1;  Error should be coms if called with boxNumber != 1
	*	The off option will always turn off both the Boost converter and 5V supplies in both the regular and overloaded function
	*	The on option will turn on the 5V supply in the normal function or the boost converter in the overloaded function.
	*	*/
	void setInput(boxControl::inputStatus position);
	void setInput(boxControl::inputStatus position, byte boxNumber);

	/* setDirection function
	*	void return type
	*	@param bool inverted
	*	configures the P1P2switch relay according to the direction the buggy has docked with the box
	*	DOES NOT directly interface with the GPIO expander, simply sets a boolean member variable to be checked and if necessary actioned as part of the switchControl function
	*/
	void setDirection(bool inverted);

	bool checkboxcontrolInit();

private:
	class switcher {
	public:
		switcher();
		~switcher();

		void initSwitcher();

		bool actuateRelays(unsigned short command);

		bool checkswitchercontrolInit();

	private:

		static bool _switcherinitComplete;

		static Adafruit_MCP23017 switchingUnit;


		bool setRelay(short relayPin);

		short retrievePinNumber(short relayName);

		bool compareRelayAssignments(unsigned short validSet);

		bool _inverted = false;
	};

	static bool _boxcontrolinitComplete;
	bool _boxInverted = 0;
	bool assignmentValid = 0;
	const byte _fiveVoltOutPin = FIVEVOLTOUTPUTPIN;
	bool checkRelayAssignments(unsigned short setting);
	bool switchRelays();

	const byte _pinP1BC = P1BCPIN;
	const byte _pinP15V = P15VPIN;
	const byte _pinP1ADC = P1ADCPIN;
	const byte _pinP2GND = P2GNDPIN;
	const byte _pinP25V = P25VPIN;
	const byte _pinP2Rd = P2RdPIN;
	const byte _pinP2Rk = P2RkPIN;
	const byte _pinP2ADC = P2ADCPIN;
	const byte _pinGNDADC = GNDADCPIN;
	const byte _pinGNDGND = GNDGNDPIN;
	const byte _pinGND5V = GND5VPIN;
	const byte _pinP1P2switch = P1P2SWITCHPIN;
	const byte _pinBC = BCPIN;
	const byte _pinADCswitch = ADCSWITCHPIN;
};



#endif


/*
* DEFINES:
*    Known cap resistor
*    Known resistor
*    ADC prescalers
*
* Includes:
*
* Adafruit library
*
*
*
* setup function
*    Adafruit_MCP23017 switcher
*    switcher.begin()
*    Assign pinModes
*    Begin Serial
*    Assign pinModes on GPIO expander
*    Setup ADC (Set prescaller bits and call 1 analog read)
*
*  enum:
*  Rk
*  Rd
*  stimulusState (on, off)
*  //boostConv (on, off)
*  components (P15V, P1ADC, P2GND, P25V, P2Rd, P2Rk, P2ADC, GNDADC, GNDGND, GND5V, BC, P1P2switch, BCswitch)
*
*  variables:
*  float v1
*  short r1, r2, r3, c1, f
*
*  const short inputPin
*  const short outputPin
*
*  static int numRelaysActuated
*
*  boxCircuits::components gpioStatus[14];
*
* Functions:
*
* preferred E24 values recovery function for resistor values
*
* preferred E12 valules recovery function for capacitor values
*
* configBox (boxNumber, boxStage, boxDirection) (depends on relay switching function)
*
* bool relay switching function (depends on check relay assignment function, depends on compare expander status function, depends on set expander status function)
*
* get value once function
*
* get value function (dependencies get value once function)
*
* measure capacitor function (dependencies get value once function)
*
* toggle boost converter function (depends on check relay assignments function)
*
* toggle box direction function (depends on check relay assignments function)
*
* byte check relay assignments function (dependency get expander status function)
*
* boxCircuits::components compare expander status function (depends on get expander status function)
*
* get expander status function
*
* set expander status function
*
*
* check relay assignment function
*      looks at relays commanded to switch and checks the following:
*          -not more than 5 pins driven
*          -not P15V and P1GND
*          -not P25V and P2GND
*          -not GND5V and GNDGND
*          -not P1BC and P1ADC
*          -if BC must also be ADCswitch
*          -if BC must also be P1BC
*/