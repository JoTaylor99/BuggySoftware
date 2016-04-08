#include "box.h"

box::box() {

}

box::~box() {
	boxGPIO.resetToDefault();
	pinMode(P1PIN, INPUT);
	pinMode(P2PIN, INPUT);
	pinMode(GNDPIN, INPUT);
	pinMode(RKPIN, INPUT);
}

void box::begin(uint8_t boxNumber, bool boxInverted) {
	_boxNumber = boxNumber;
	boxGPIO.begin();
	boxGPIO.pinMode(P1P2RELAYPIN, OUTPUT);
	boxGPIO.pinMode(BCPIN, OUTPUT);
	boxGPIO.pinMode(BCRELAYPIN, OUTPUT);
	boxGPIO.digitalWrite(BCRELAYPIN, LOW);

	if (boxInverted) {
		boxGPIO.digitalWrite(P1P2RELAYPIN, HIGH);
	}

	if (_boxNumber == 1) {
		P1pin[0] = bC::input;
		P1pin[1] = bC::nop;
		P2pin[0] = bC::input_adc;
		P2pin[1] = bC::nop;
		GNDpin[0] = bC::low;
		GNDpin[1] = bC::nop;
		Rkpin[0] = bC::input;
		Rkpin[1] = bC::nop;
	}
	else if (_boxNumber == 2) {
		P1pin[0] = bC::high;
		P1pin[1] = bC::input;
		P2pin[0] = bC::input_adc;
		P2pin[1] = bC::input_adc;
		GNDpin[0] = bC::input;
		GNDpin[1] = bC::high;
		Rkpin[0] = bC::low;
		Rkpin[1] = bC::low;
	}
	else if (_boxNumber == 3) {
		P1pin[0] = bC::input_adc;
		P1pin[1] = bC::high;
		P2pin[0] = bC::high;
		P2pin[1] = bC::input_adc;
		GNDpin[0] = bC::low;
		GNDpin[1] = bC::low;
		Rkpin[0] = bC::input;
		Rkpin[1] = bC::input;
	}
	else if (_boxNumber == 4) {
		P1pin[0] = bC::high;
		P1pin[1] = bC::input_adc;
		P2pin[0] = bC::input_adc;
		P2pin[1] = bC::high;
		GNDpin[0] = bC::low;
		GNDpin[1] = bC::low;
		Rkpin[0] = bC::input;
		Rkpin[1] = bC::input;
	}
	else if (_boxNumber == 5) {
		P1pin[0] = bC::input;
		P1pin[1] = bC::nop;
		P2pin[0] = bC::input_adc;
		P2pin[1] = bC::nop;
		GNDpin[0] = bC::low;
		GNDpin[1] = bC::input;
		Rkpin[0] = bC::high;
		Rkpin[1] = bC::input;
	}
	else if (_boxNumber == 6) {
		P1pin[0] = bC::high;
		P1pin[1] = bC::input;
		P2pin[0] = bC::input_adc;
		P2pin[1] = bC::nop;
		GNDpin[0] = bC::input;
		GNDpin[1] = bC::nop;
		Rkpin[0] = bC::low;
		Rkpin[1] = bC::input;
	}
	else if (_boxNumber == 7) {
		P1pin[0] = bC::input;
		P1pin[1] = bC::nop;
		P2pin[0] = bC::input_adc;
		P2pin[1] = bC::nop;
		GNDpin[0] = bC::low;
		GNDpin[1] = bC::input;
		Rkpin[0] = bC::high;
		Rkpin[1] = bC::input;
	}
}

bool box::interrogateBox() {
	if (_previouslyCalled == true) {
		ERROR_PRINTLN("Only 1 call to interrogateBox is allowed per object creation, please create another object and try again.");
			return true;
	}
	else {
		_previouslyCalled = true;
	}

	double rawValue = 0;
	double calculatedValue = 0;

	switch (_boxNumber)
	{
	case 1:
		rawValue = getReading(0);
		//calculate actual voltage
		calculatedValue = BOXONERATIO * rawValue;
		//store voltage in return data variable
		presentationData.v1 = calculatedValue;
		//In future will Coms result for now just serial print
		BOX_PRINT("V1 = ");
		BOX_VPRINTLN(presentationData.v1);
		return true;
	case 2:
	case 3:
	case 4:
		rawValue = getReading(0);

		//calculate resistor value
		calculatedValue = calculateResistorValue(rawValue, 0);

		//get preferred
		presentationData.r1 = toPreferredResistor(calculatedValue);

		//In future will Coms result for now just serial print
		BOX_PRINT("R1 = ");
		BOX_VPRINTLN(presentationData.r1);

		rawValue = getReading(1);
		//calculate resistor value
		calculatedValue = calculateResistorValue(rawValue, 1);
		//get preferred
		presentationData.r2 = toPreferredResistor(calculatedValue);

		//In future will Coms result for now just serial print
		BOX_PRINT("R2 = ");
		BOX_VPRINTLN(presentationData.r2);
		return true;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	default:
		ERROR_PRINTLN("Passed invalid box number, returning to navigation");
		return true;
	}
}

//Function to pick the prefered resistor from a given resistor value range.
short box::toPreferredResistor(double OResistor) {
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
	if (_boxNumber < 5) { //Boxes 2-4
		i = 17; //Index of 100.
		IMAX = 49; // Index of 2000.
	}
	else if (_boxNumber < 7) { // Boxes 5 & 6
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

byte box::toPreferredCapactitor(double OCap) {
	const double PCapacitors[] = { 4.7, 5.6, 6.8, 8.2, 10, 12, 15, 18, //7
		22, 27, 33, 39, 47, 56, 68, 82, 100 }; //16
	byte FinalCap = 0;
	byte IMAX = 16;
	byte i = 0; //To define index of array.
	if (_boxNumber < 7) { //Boxes 5-6
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

void box::setBoostConverter(bC::inputStatus state) {
	if (state = bC::ON) {
		_boostConverterOn = true;
		boxGPIO.digitalWrite(BCRELAYPIN, HIGH);
		delay(10);
		boxGPIO.digitalWrite(BCPIN, HIGH);
		delay(10);
	}
	else {
		boxGPIO.digitalWrite(BCPIN, LOW);
		delay(10);
		boxGPIO.digitalWrite(BCRELAYPIN, LOW);
		delay(10);
		_boostConverterOn = false;
	}
}

bool box::docked() {
	boxGPIO.digitalWrite(BCRELAYPIN, LOW);
	pinMode(P1PIN, OUTPUT);
	pinMode(GNDPIN, OUTPUT);
	pinMode(P2PIN, INPUT);
	digitalWrite(P1PIN, LOW);
	digitalWrite(GNDPIN, LOW);
	if (getRawReading(P2PIN) != 0) {
		ERROR_PRINTLN("Buggy improperly docked");
		return false;
	}
	else { return true; }
}

void box::configureForAnalysis(bool state) {
	_adcPin = P2PIN;

	if (Rkpin[state] == bC::input_adc) {
		ERROR_PRINTLN("Rkpin not used for readings, setup as high impedance, ADC pin left as default (P2)");
		pinMode(RKPIN, INPUT);
	}
	else if (Rkpin[state] == bC::input) {
		pinMode(RKPIN, INPUT);
	}
	else if ((Rkpin[state] == bC::high) || (Rkpin[state] == bC::low)) {
		pinMode(RKPIN, OUTPUT);
	}
	else {}

	if (GNDpin[state] == bC::input_adc) {
		ERROR_PRINTLN("GNDpin not used for readings, setup as high impedance, ADC pin left as default (P2)");
		pinMode(GNDPIN, INPUT);
	}
	else if (GNDpin[state] == bC::input) {
		pinMode(GNDPIN, INPUT);
	}
	else if ((GNDpin[state] == bC::high) || (GNDpin[state] == bC::low)) {
		pinMode(GNDPIN, OUTPUT);
		}
	else {}

	if (P1pin[state] == bC::input_adc) {
		_adcPin = P1PIN;
		pinMode(P1PIN, INPUT);
	}
	else if (P1pin[state] == bC::input) {
		pinMode(P1PIN, INPUT);
	}
	else if ((P1pin[state] == bC::high) || (P1pin[state] == bC::low)) {
		pinMode(P1PIN, OUTPUT);
	}
	else {}

	if (P2pin[state] == bC::input_adc) {
		_adcPin = P2PIN;
		pinMode(P2PIN, INPUT);
	}
	else if (P2pin[state] == bC::input) {
		pinMode(P2PIN, INPUT);
	}
	else if ((P2pin[state] == bC::high) || (P2pin[state] == bC::low)) {
		pinMode(P2PIN, OUTPUT);
	}
	else {}

}

void box::toggleOutputs(bC::inputStatus state, bool stage) {
	if (state == bC::ON) {
		if (Rkpin[state] == bC::low) { digitalWrite(RKPIN, LOW); }
		else if (Rkpin[state] == bC::high) { digitalWrite(RKPIN, HIGH); }
		if (GNDpin[state] == bC::low) { digitalWrite(GNDPIN, LOW); }
		else if (GNDpin[state] == bC::high) { digitalWrite(GNDPIN, HIGH); }
		if (P1pin[state] == bC::low) { digitalWrite(P1PIN, LOW); }
		else if (P1pin[state] == bC::high) { digitalWrite(P1PIN, HIGH); }
		if (P2pin[state] == bC::low) { digitalWrite(P2PIN, LOW); }
		else if (P2pin[state] == bC::high) { digitalWrite(P2PIN, HIGH); }
	}
	else if (state == bC::OFF) {
		digitalWrite(RKPIN, LOW);
		digitalWrite(GNDPIN, LOW);
		digitalWrite(P1PIN, LOW);
		digitalWrite(P2PIN, LOW);
	}
}

double box::getReading(bool stage) {
	double retrievedVoltage = 0;
	configureForAnalysis(stage);
	toggleOutputs(bC::ON, stage);
	if (_boxNumber == 1) {
	setBoostConverter(bC::ON);
	}
	for (int i = 0; i < NUMADCREADINGS; i++) {
		retrievedVoltage += convertToVoltage(getRawReading(_adcPin));
	}
	retrievedVoltage /= NUMADCREADINGS;
	if (_boxNumber == 1) {
		setBoostConverter(bC::OFF);
	}
	toggleOutputs(bC::OFF, stage);
	return retrievedVoltage;
}

double box::convertToVoltage(short raw) {
	return (VREF * (raw / ADCMAX));
}

short box::getRawReading(uint8_t pin) {
	return analogRead(pin);
}

double box::calculateResistorValue(double rawValue, bool stage) {
	double calculatedResistance = 0;
	if (_boxNumber == 2) {
		calculatedResistance = (RK * ((VREF / rawValue) - 1));
	}
	else if (_boxNumber == 4) {
		calculatedResistance = (((VREF * 1200) / rawValue) - 1200);
	}
	
	if (stage == 0) {
		if (_boxNumber == 3) {
			calculatedResistance = (((VREF * 1000) / rawValue) - 1000);
		}
		else if (_boxNumber == 5) {

		}
		else if (_boxNumber == 6) {

		}
		else if (_boxNumber == 7) {

		}
	}
	else {
			calculatedResistance = ((rawValue * presentationData.r1)/(VREF - rawValue));
	}
}