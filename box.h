#ifndef _BOX_H
#define _BOX_H


#include "boxControl.h"
#include "Config.h"
#include "boxValues.h"
//#include "Conversions.h"

class box {

public:

	box();

	~box();

	void init();

	boxValues::returnData interrogateBox(byte boxNumber, bool boxInverted);

private:
		const byte _adcInPin = ADCINPUTPIN;

		enum inputStatus {
			OFF = 0,
			ON = 1

		};

		double measureCapacitance();

		double getReading();

		double getReadingOnce();

		bool setInput(box::inputStatus position);
		bool setInput(box::inputStatus position, short boxNumber);
};
#endif