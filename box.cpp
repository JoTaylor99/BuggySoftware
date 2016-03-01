#include "box.h"

box::box() {
}

box::~box() {
}

void box::init(){
	initControl();
	pinMode(_adcInPin, INPUT);

	const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	ADCSRA &= ~PS_128;  // remove bits set by Arduino library

	const unsigned char PS_16 = (1 << ADPS2);

	ADCSRA |= PS_16; //20us

	double setup = getReadingOnce();
};

boxValues::returnData box::interrogateBox(byte boxNumber, bool boxInverted){};

double box::getReadingOnce(){};