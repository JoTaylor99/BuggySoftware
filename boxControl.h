#ifndef _BOXCONTROL_H
#define _BOXCONTROL_H


//#include <bitset.h>

#include "Config.h"
#include <stdint.h>
#include <Adafruit_MCP23017.h>

class boxControl {

public:

	boxControl();

	~boxControl();

protected:
//enum for box driving voltage control
	enum inputStatus {
		OFF = 0,
		ON = 1
	};

	void initControl();

	bool switchControl(boxConfig::boxSettings boxSetting);

	bool setInput(boxControl::inputStatus position);
	bool setInput(boxControl::inputStatus position, short boxNumber);

	bool setDirection(bool inverted);

private:
	class switcher {
	public:
		switcher();
		~switcher();

		void initSwitcher();

		bool actuateRelays(unsigned short command);

	private:

		static Adafruit_MCP23017 switchingUnit;
		static unsigned short _currentRelayState;
		static bool initComplete;

		bool setRelay(short relayPin);

		short retrievePinNumber(short relayName);

		bool compareRelayAssignments(unsigned short validSet);

		bool _inverted = false;
	};

	bool assignmentValid = 0;
	const byte _fiveVoltOutPin = FIVEVOLTOUTPUTPIN;
	bool checkRelayAssignments(unsigned short setting);
	bool switchRelays();

	const byte _pinP1BC = pinP1BC;
	const byte _pinP15V = pinP15V;
	const byte _pinP1ADC = pinP1ADC;
	const byte _pinP2GND = pinP2GND;
	const byte _pinP25V = pinP25V;
	const byte _pinP2Rd = pinP2Rd;
	const byte _pinP2Rk = pinP2Rk;
	const byte _pinP2ADC = pinP2ADC;
	const byte _pinGNDADC = pinGNDADC;
	const byte _pinGNDGND = pinGNDGND;
	const byte _pinGND5V = pinGND5V;
	const byte _pinP1P2switch = pinP1P2switch;
	const byte _pinBC = pinBC;
	const byte _pinADCswitch = pinADCswitch;
};


#endif