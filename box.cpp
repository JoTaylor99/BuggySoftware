#include "box.h"

box::box() {
}

box::~box() {
}

void box::init(){
	initControl();
	setupADC();
};

boxValues::returnData box::interrogateBox(byte boxNumber, bool boxInverted){
	int error = 0;
	checkconfigCorrect();		//ensure init functions sucessfully called
	setInput(OFF);				//ensure all circuit stimulis are off

	boxValues::returnData presentationData;

	//select correct switch configuration
	switchControl((retrieveSettings(boxNumber)));
	//sets approach direction relay
	setDirection(boxInverted);

	//check if correctly docked
	if (!docked()) {
		presentationData.error = 1;
	};

	if (boxNumber = 1) {
		setInput(ON, boxNumber);
	}
	else { setInput(ON); }

	setInput(OFF);


	if (boxNumber > 4) { setInput(ON); }

	setInput(OFF);
};

double box::getReadingOnce(){};

void box::setupADC(){
	pinMode(_adcInPin, INPUT);

	const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	ADCSRA &= ~PS_128;  // remove bits set by Arduino library

	const unsigned char PS_16 = (1 << ADPS2);

	ADCSRA |= PS_16; //20us

	double setup = getReadingOnce();
};

void box::checkconfigCorrect() {
	if (!_boxinitComplete) { init(); }

	if (!checkboxcontrolInit()) { initControl(); }
}
bool box::_boxinitComplete = false;

boxConfig::boxSettings box::retrieveSettings(byte boxNumber) {

};

bool box::docked(){};

