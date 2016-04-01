#include "box.h"

box::box() {
}

box::~box() {
}

void box::init(){
	initControl();
	setupADC();
};

void box1() {
}

void box2a() { //Find R1, p1-5V p2-ADC gnd-HI Z Rk-ground
	int Rk = 560; // get exact value from bridge
	int Vi = 5.00; //get exact output with lipo to arduino
	int Vo; // assign Vo to ADC Value 
	Vo = (Vo * Vi) / 1023; // turns ADC value to actual voltage level
	int R1;
	R1 = Rk * ((Vi - Vo) / Vo);
	//output R1
}

void box2b() { //Find R2, p1-HI Z p2-ADC GND-GND Rk-5V
	int Rk = 560;
	int Vi = 5.00;
	int Vo; 
	Vo = (Vo * Vi) / 1023;
	int R2;
	R2 = (Vo * Rk) / (Vi - Vo);
	//output R2
}

void box3a(){ //Find R1, p1-GND p2-ADC GND-HI Z Rk-5V
	int Rk = 560;
	int Vi = 5.00;
	int Vo;
	Vo = (Vo * Vi) / 1023;
	int R1;
	R1 = (Vo * Rk) / (Vi - Vo);
	//output R1
}

void box3b() { //Find R2, p1-HI Z p2-ADC GND-GND Rk-5V
	int Rk = 560;
	int Vi = 5.00;
	int Vo;
	Vo = (Vo * Vi) / 1023;
	int R2;
	R2 = (Vo * Rk) / (Vi - Vo);
	//output R2
}

void box4a(){//Find R1, p1-5V p2-ADC gnd-gnd Rk-HI Z
	int Rk = 1200; // get exact value from bridge
	int Vi = 5.00; //get exact output with lipo to arduino
	int Vo; // assign Vo to ADC Value 
	Vo = (Vo * Vi) / 1023; // turns ADC value to actual voltage level
	int R1;
	R1 = Rk * ((Vi - Vo) / Vo);
	//output R1
}

void box4b() {//Find R2, p1-ADC p2-5V gnd-gnd Rk-HI Z
	int Rk = 560; // get exact value from bridge
	int Vi = 5.00; //get exact output with lipo to arduino
	int Vo; // assign Vo to ADC Value 
	Vo = (Vo * Vi) / 1023; // turns ADC value to actual voltage level
	int R2;
	R2 = Rk * ((Vi - Vo) / Vo);
	//output R2
}

void box5a(){
	//capacitor read between p1 and p2
}

void box5b() {//Find R, p1-HI Z p2-ADC GND-GND Rk-5V
	int Rk = 560;
	int Vi = 5.00;
	int Vo;
	Vo = (Vo * Vi) / 1023;
	int R;
	R = (Vo * Rk) / (Vi - Vo);
	//output R
}

void box6a(){ 
	//capacitor read between p2 and gnd
}

void box6b() {//Find R, p1-gnd p2-ADC GND-HI Z Rk-5V
	int Rk = 560;
	int Vi = 5.00;
	int Vo;
	Vo = (Vo * Vi) / 1023;
	int R;
	R = (Vo * Rk) / (Vi - Vo);
	//output R
}

void box7a(){
	// capacitor read betwen p1 and p2
}

void box7b() {//Find R, p1-HI Z p2-ADC GND-GND Rk-5V
	int Rk = 560;
	int Vi = 5.00;
	int Vo;
	Vo = (Vo * Vi) / 1023;
	int R;
	R = (Vo * Rk) / (Vi - Vo);
	//output R
}

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
		ERROR_PRINTLN("Buggy not docked");
		return presentationData.error;
	};

	if (boxNumber = 1) {
		setInput(ON, boxNumber);
		presentationData.v1 = getReading();
		Serial.print(F("V1 = "));
		Serial.println(presentationData.v1);
		return 0;
	}
	else {
		setInput(ON);
		rawValue = getReading();
		calculatedValue = calculateResistorValue(rawValue, boxNumber);
		presentationData.r1 = PrefResistor(calculatedValue, boxNumber);
		if ((boxNumber == 2) || (boxNumber == 3) || (boxNumber == 4)) {
			Serial.print(F("Ra = "));
		} else if ((boxNumber == 5) || (boxNumber == 6) || (boxNumber == 7)) {
			Serial.print(F("R1 = "));
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
		Serial.print(F("Rb = "));
		Serial.println(presentationData.r2);
		setInput(OFF); 
	} else {
		calculatedValue = measureCapacitance();
		presentationData.c1 = PrefCapactitor(calculatedValue, boxNumber);
		Serial.print(F("C = "));
		Serial.println(presentationData.c1);

		presentationData.f = calculateFrequency(boxNumber);
		(boxNumber == 7) ? (Serial.print(F("Resonant Frequency = "))) : (Serial.print(F("Corner Frequency = ")));
		Serial.println(presentationData.f);
	}
	return 0;

};

double box::getOneReading(){
	return analogRead(_adcInPin);
};

void box::setupADC(){
	pinMode(_adcInPin, INPUT);

	const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	ADCSRA &= ~PS_128;  // remove bits set by Arduino library

	const unsigned char PS_16 = (1 << ADPS2);

	ADCSRA |= PS_16; //20us

	double setup = getOneReading();
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
			ERROR_PRINTLN("Error, no second stage in box 1 analysis");
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

double calculateResistorValue(double rawValue, byte boxNumber){
};

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
		for (; (i < IMAX+1); i++) {
			if ((OResistor <= (PResistors[i] * 1.05)) && (OResistor >= (PResistors[i] * 0.95))) {
				FinalResistor = PResistors[i];
				break;
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
		for (; (i < IMAX+1); i++) {
			if ((OCap <= (PCapacitors[i] * 1.10)) && (OCap >= (PCapacitors[i] * 0.90))) {
				FinalCap = PCapacitors[i];
				break;
			}
		}
	}

	return FinalCap;
}