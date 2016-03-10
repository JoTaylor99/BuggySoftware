#include "box.h"

box::box() {
}

box::~box() {
}

void box::init(){
	initControl();
	setupADC();
};

byte box::interrogateBox(byte boxNumber, bool boxInverted){
	presentationData.error = 0;
	presentationData.c1 = 0;
	presentationData.f = 0;
	presentationData.r1 = 0;
	presentationData.r2 = 0;
	presentationData.v1 = 0;

	double rawValue = 0;
	double calculatedValue = 0;
	short preferred = 0;
	checkconfigCorrect();		//ensure init functions sucessfully called
	setInput(OFF);				//ensure all circuit stimulis are off

	//select correct switch configuration
	switchControl((retrieveSettings(boxNumber)));
	//sets approach direction relay
	setDirection(boxInverted);

	//check if correctly docked
	if (!docked()) {
		presentationData.error = 1;
		Serial.println("Buggy not docked");
		return presentationData.error;
	};

	if (boxNumber = 1) {
		setInput(ON, boxNumber);
		presentationData.v1 = getReading();
		Serial.print("V1 = ");
		Serial.println(presentationData.v1);
		return 0;
	}
	else {
		setInput(ON);
		rawValue = getReading();
		calculatedValue = calculateResistorValue(rawValue, boxNumber);
		presentationData.r1 = PrefResistor(calculatedValue, boxNumber);
		if (boxNumber == 2) {
			Serial.print("Ra = ");
		} else if (boxNumber == 3) {
			Serial.print("Ra = ");
		} else if (boxNumber == 4) {
			Serial.print("Ra = ");
		} else if (boxNumber == 5) {
			Serial.print("R1 = ");
		} else if (boxNumber == 6) {
			Serial.print("R1 = ");
		} else if (boxNumber == 7){
			Serial.print("R1 = ");
		}
		Serial.println(presentationData.r1);
	}
	setInput(OFF);

	switchControl((retrieveSettings(0x80|boxNumber)));

	if (boxNumber < 4) {
		setInput(ON);
		rawValue = getReading();
		calculatedValue = calculateResistorValue(rawValue, (0x80|boxNumber));
		presentationData.r2 = PrefResistor(calculatedValue, boxNumber);
		Serial.print("Rb = ");
		Serial.println(presentationData.r2);
		setInput(OFF); 
	} else {
		calculatedValue = measureCapacitance();
		presentationData.c1 = PrefCapactitor(calculatedValue, boxNumber);
		Serial.print("C = ");
		Serial.println(presentationData.c1);

		presentationData.f = calculateFrequency(boxNumber);
		(boxNumber == 7) ? (Serial.print("Resonant Frequency = ")) : (Serial.print("Corner Frequency = "));
		Serial.println(presentationData.f);
	}
	return 0;

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
	if (bitRead(boxNumber, 7)){
		bitClear(boxNumber, 7);
		switch (boxNumber) {
		case 1:
			Serial.print("Error, no second stage in box 1 analysis");
			return boxConfig::box1;
		case 2:	return boxConfig::box2b;
		case 3: return boxConfig::box3b;
		case 4: return boxConfig::box4b;
		case 5: return boxConfig::box5b;
		case 6: return boxConfig::box6b;
		case 7: return boxConfig::box7b;
		}
	} else {
		switch (boxNumber) {
		case 1: return boxConfig::box1;
		case 2: return boxConfig::box2a;
		case 3: return boxConfig::box3a;
		case 4: return boxConfig::box4a;
		case 5: return boxConfig::box5a;
		case 6: return boxConfig::box6a;
		case 7: return boxConfig::box7a;
		}
	}
}

bool box::docked() {
	bool connected = false;
	if (getReading() == 0) {
		connected = true;
	}
	return connected;
};

double box::getReading() {
	

};

double calculateResistorValue(double rawValue, byte boxNumber){};

//Function to pick the prefered resistor from a given resistor value range.
short PrefResistor(double OResistor, byte BoxNo) {
	const short PResistors[] = { 20, 22, 24, 27, 30, 33, 36, 39, 43, 47, 51, //10
		56, 62, 68, 75, 82, 91, //16
		100, 110, 120, 130, 150, 160, 180, //23
		200, 220, 240, 270, 300, 330, 360, 390, 430, 470, //33
		510, 560, 620, 680, 750, 820, 910, //40
		1000, 1100, 1200, 1300, 1400, 1500, 1600, 1800, //48
		2000, 2200, 2400, 2700, 3000 }; //53
	short FinalResistor = 0;
	byte IMAX = 53;
	byte i = 0; //To define index of array.
	if (BoxNo < 5) { //Boxes 2-4
		i = 17; //Index of 100.
		IMAX = 49; // Index of 2000.
	}
	else if (BoxNo < 7) { // Boxes 5 & 6
		i = 44; //Index of 1300.
		IMAX = 53; //Index of 3000.
	}
	else { //Box 7.
		i = 0; //Index of 20
		IMAX = 13; // Index of 68 (closest value to 70)
	}
	if (OResistor < PResistors[i]) {
		FinalResistor = PResistors[i];
	}
	else if (OResistor > PResistors[IMAX]){
		FinalResistor = PResistors[IMAX];
	}
	else {
		for (; (i < 55) && (i < IMAX); i++) {
			if ((OResistor <= (PResistors[i] * 1.05)) && (OResistor >= (PResistors[i] * 0.95))) {
				FinalResistor = PResistors[i];
				i = 55;
			}
		}
	}

	return FinalResistor;
};

byte PrefCapactitor(double OCap, byte BoxNo) {
	const double PCapacitors[] = { 4.7, 5.6, 6.8, 8.2, 10, 12, 15, 18, //7
		22, 27, 33, 39, 47, 56, 68, 82, 100 }; //16
	byte FinalCap = 0;
	byte IMAX = 16;
	byte i = 0; //To define index of array.
	if (BoxNo < 7) { //Boxes 5-6
		i = 0; //Index of 4.7 (closest to 4).
		IMAX = 6; // Index of 15 (closest under 17).
	}
	else { //Box 7.
		i = 12; //Index of 47 (closest to 40)
		IMAX = 16; // Index of 100 (closest value to 110)
	}

	if (OCap < PCapacitors[i]) {
		FinalCap = PCapacitors[i];
	}
	else if (OCap > PCapacitors[IMAX]) {
		FinalCap = PCapacitors[IMAX];
	}
	else {
		for (; (i < 17) && (i < IMAX); i++) {
			if ((OCap <= (PCapacitors[i] * 1.10)) && (OCap >= (PCapacitors[i] * 0.90))) {
				FinalCap = PCapacitors[i];
				i = 17;
			}
		}
	}

	return FinalCap;
}