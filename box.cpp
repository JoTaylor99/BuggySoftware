#include "box.h"


box::box() {

}

box::~box() {
	//boxGPIO.resetToDefault();
	boxGPIO.begin();
	pinMode(P1PIN, INPUT);
	pinMode(P2PIN, INPUT);
	pinMode(GNDPIN, INPUT);
	pinMode(RKPIN, INPUT);
}

bool box::begin(uint8_t boxNumber, bool boxInverted) {
	_boxNumber = boxNumber;
	boxGPIO.begin();
	boxGPIO.pinMode(P1P2RELAYPIN, OUTPUT);
	boxGPIO.pinMode(BCPIN, OUTPUT);
	boxGPIO.pinMode(BCRELAYPIN, OUTPUT);
	boxGPIO.pinMode(BC_DPDT, OUTPUT);
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
		return true;
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
		return true;
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
		return true;
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
		return true;
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
		return true;
		
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
		return true;
	}
	else if (_boxNumber == 7) {
		P1pin[0] = bC::input;
		P1pin[1] = bC::nop;
		P2pin[0] = bC::input_adc;
		P2pin[1] = bC::nop;
		GNDpin[0] = bC::low;
		GNDpin[1] = bC::input;
		Rkpin[0] = bC::input;
		Rkpin[1] = bC::input;
		return true;
	}
	else
	{
		ERROR_PRINTLN("Incorect box number given ");
		return false;
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
		calculatedValue = BOXONERATIO * rawValue; // DC CHANGE THIS to correct value 
		//store voltage in return data variable
		presentationData.v1 = calculatedValue;
		//In future will Coms result for now just serial print
		Serial.print(F("V1 = "));
		Serial.println(presentationData.v1);
		buggyTop::sendBoxValue(Comms::FunctionCodes::BoxValue1, (float)presentationData.v1);
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
		Serial.print (F("Ra = "));
		Serial.println(presentationData.r1);
		buggyTop::sendBoxValue(Comms::FunctionCodes::BoxValue1, (float)presentationData.r1);

		rawValue = getReading(1);
		//calculate resistor value
		calculatedValue = calculateResistorValue(rawValue, 1);
		//get preferred
		presentationData.r2 = toPreferredResistor(calculatedValue);
		Serial.print(F("Rb = "));
		Serial.println(presentationData.r2);
		buggyTop::sendBoxValue(Comms::FunctionCodes::BoxValue1, (float)presentationData.r2);
		//In future will Coms result for now just serial print
		return true;
	case 5:
	case 6:
		rawValue = getReading(0);

		//calculate resistor value
		calculatedValue = calculateResistorValue(rawValue, 0);

		//get preferred
		presentationData.r1 = toPreferredResistor(calculatedValue);

		//In future will Coms result for now just serial print
		Serial.print(F("R = "));
		Serial.println(presentationData.r1);
		buggyTop::sendBoxValue(Comms::FunctionCodes::BoxValue1, (float)presentationData.r1);


		calculatedValue = measureCapacitance();


		presentationData.c1 = toPreferredCapacitor(calculatedValue);
		//In future will Coms result for now just serial print
		Serial.print(F("C = "));
		Serial.println(presentationData.c1);
		buggyTop::sendBoxValue(Comms::FunctionCodes::BoxValue2, (float)presentationData.c1);


		presentationData.f = calculateFrequency();
		//In future will Coms result for now just serial print
		Serial.print(F("Frequency = "));
		Serial.println(presentationData.f);
		buggyTop::sendBoxValue(Comms::FunctionCodes::BoxValue3, (float)presentationData.f);

		return true;
	case 7:
		//_adcPin = RK7pin; set in configureforanalysis
		rawValue = getReading(0);

		//calculate resistor value
		calculatedValue = calculateResistorValue(rawValue, 0);

		//get preferred
		presentationData.r1 = toPreferredResistor(calculatedValue);

		//In future will Coms result for now just serial print
		Serial.print(F("R = "));
		Serial.println(presentationData.r1);
		buggyTop::sendBoxValue(Comms::FunctionCodes::BoxValue1, (float)presentationData.r1);

		calculatedValue = measureCapacitance();
		presentationData.c1 = toPreferredCapacitor(calculatedValue);
		//In future will Coms result for now just serial print
		Serial.print(F("C = "));
		Serial.println(presentationData.c1);
		buggyTop::sendBoxValue(Comms::FunctionCodes::BoxValue2, (float)presentationData.c1);


		presentationData.f = calculateFrequency();
		//In future will Coms result for now just serial print
		Serial.print(F("Frequency = "));
		Serial.println(presentationData.f);

		buggyTop::sendBoxValue(Comms::FunctionCodes::BoxValue3, (float)presentationData.f);

		return true;
	default:
		ERROR_PRINTLN("Passed invalid box number, returning to navigation");
		return true;
	}
}

//Function to pick the prefered resistor from a given resistor value range.
short box::toPreferredResistor(double OResistor) {
	short FinalResistor = PResistors[0];
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

double box::toPreferredCapacitor(double OCap) {
	double FinalCap = PCapacitors[0];
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

double box::ResistorValue(uint8_t BoxNumber) {
	//uint8_t BoxResistors[33];
	byte IMAX = 53;
	byte i = 0; //To define index of array.
	if (BoxNumber < 5)
	{ //Boxes 2-4
		i = 17; //Index of 100.
		IMAX = 49; // Index of 2000.
	}
	else if (BoxNumber < 7)
	{ // Boxes 5 & 6
		i = 44; //Index of 1300.
		IMAX = 53; //Index of 3000.
	}
	else { //Box 7.
		i = 0; //Index of 20
		IMAX = 13; // Index of 68 (closest value to 70)
	}
	randomSeed(micros());
	uint8_t TempRand = random(i,IMAX+1);
	double Resistor = PResistors[TempRand];
	presentationData.r1 = Resistor;
	/*
	if (BoxNumber == 2 | BoxNumber == 3 | BoxNumber == 4) {
		TempRand = random(i, IMAX + 1);
		Resistor = PResistors[TempRand];
		presentationData.r2 = Resistor;
	}
	*/
	return Resistor;
}

double box::CapacitorValue(uint8_t BoxNumber)
{
	byte IMAX = 16;
	byte i = 0; //To define index of array.
	if (BoxNumber < 7)
	{ //Boxes 5-6
		i = 0; //Index of 4.7 (closest to 4).
		IMAX = 6; // Index of 15 (closest under 17).
	}
	else { //Box 7.
		i = 12; //Index of 47 (closest to 40)
		IMAX = 16; // Index of 100 (closest value to 110)
	}
	//Random number generation:
	randomSeed(micros());
	uint8_t TempRand = random(i, IMAX+1);
	double Capacitor = PCapacitors[TempRand];
	presentationData.c1 = Capacitor;
	return Capacitor;
}


void box::setBoostConverter(bC::inputStatus state) {
	if (state = bC::ON) {
		_boostConverterOn = true;
		boxGPIO.digitalWrite(BCRELAYPIN, HIGH);
		boxGPIO.digitalWrite(BC_DPDT, HIGH);
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
	//_adcPin = P2PIN;
	if (_boxNumber == 1 || _boxNumber==2 ) {
		_adcPin = P2PIN;
	}
	else if (_boxNumber == 3) {
		if (state == 0) {
			_adcPin = P1PIN;
		}
		else {
			_adcPin = P2PIN;
		}
	}
	else if (_boxNumber == 4) {
		if (state == 0) {
			_adcPin = P2PIN;
		}
		else {
			_adcPin = P1PIN;
		}
	}
	else{ //5, 6 & 7
		if (state == 0) {
			_adcPin = P2PIN;
			if (_boxNumber == 7) {
				pinMode(RK7pin, OUTPUT);
				digitalWrite(RK7pin, HIGH);
			}
			else if (_boxNumber == 5) {
				pinMode(RKPIN, OUTPUT);
				digitalWrite(RKPIN, HIGH);
				pinMode(P1PIN, INPUT);
				pinMode(GNDPIN, OUTPUT);
				digitalWrite(GNDPIN, LOW);
				_adcPin = P2PIN;
			}
			else if (_boxNumber == 6) {
				pinMode(RKPIN, OUTPUT);
				digitalWrite(RKPIN, HIGH);
				pinMode(P1PIN, OUTPUT);
				digitalWrite(P1PIN, LOW);
				pinMode(GNDPIN, INPUT);
				_adcPin = P2PIN;
			}
		}
		else {
			if (_boxNumber == 7) {
				digitalWrite(RK7pin, LOW);
				pinMode(GNDPIN, INPUT);
			}else{
				_adcPin = P2PIN; //Not sure..
			}
			//_adcPin = P2PIN; //Not sure..
		}
		
	}


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
		if (Rkpin[stage] == bC::low) {
			pinMode(RKPIN, OUTPUT);
			digitalWrite(RKPIN, LOW);
		}
		else if (Rkpin[stage] == bC::high) {
			pinMode(RKPIN, OUTPUT);
			digitalWrite(RKPIN, HIGH);
		}
		if (GNDpin[stage] == bC::low) {
			pinMode(GNDPIN, OUTPUT);
			digitalWrite(GNDPIN, LOW);
		}
		else if (GNDpin[stage] == bC::high) {
			pinMode(GNDPIN, OUTPUT);
			digitalWrite(GNDPIN, HIGH);
		}
		//pinMode(P1PIN, OUTPUT);
		if (P1pin[stage] == bC::low) {
			pinMode(P1PIN, OUTPUT);
			digitalWrite(P1PIN, LOW);
		}
		else if (P1pin[stage] == bC::high) {
			pinMode(P1PIN, OUTPUT);
			digitalWrite(P1PIN, HIGH);
		}

		if (P2pin[stage] == bC::low) {
			pinMode(P2PIN, OUTPUT);
			digitalWrite(P2PIN, LOW);
		}
		else if (P2pin[stage] == bC::high) {
			pinMode(P2PIN, OUTPUT);
			digitalWrite(P2PIN, HIGH);
		}
	}
	else if (state == bC::OFF) {
		if ((Rkpin[stage] == bC::high) || (Rkpin[stage] == bC::low)) {
			digitalWrite(RKPIN, OUTPUT);
			digitalWrite(RKPIN, LOW);
		}
		if ((GNDpin[stage] == bC::high) || (GNDpin[stage] == bC::low)) {
			digitalWrite(GNDPIN, OUTPUT);
			digitalWrite(GNDPIN, LOW);
		}
		if ((P1pin[stage] == bC::high) || (P1pin[stage] == bC::low)) {
			digitalWrite(P1PIN, OUTPUT);
			digitalWrite(P1PIN, LOW);
		}
		if ((P2pin[stage] == bC::high) || (P2pin[stage] == bC::low)) {
			digitalWrite(P2PIN, OUTPUT);
			digitalWrite(P2PIN, LOW);
		}
	}
}

double box::getReading(bool stage) {
	double retrievedVoltage = 0;
	configureForAnalysis(stage);
	toggleOutputs(bC::ON, stage);
	if (_boxNumber == 7 && stage == 1) {} else {
		//_adcPin = P2PIN; //This needs to be taken out.
		pinMode(_adcPin, INPUT);
		if (_boxNumber == 1) {
			pinMode(P1PIN, OUTPUT);
			digitalWrite(P1PIN, HIGH);
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
	}
	return retrievedVoltage;
}

double box::convertToVoltage(short raw) {
	double tempvolt = 5.0*raw/1023.0;
	//double tempvolt = double(VREF * (raw / ADCMAX));


	return tempvolt;
}

short box::getRawReading(uint8_t pin) {
	short temp = analogRead(pin);


	return temp;
}

double box::calculateResistorValue(double rawValue, bool stage) {
	double calculatedResistance = 0;
	if (_boxNumber == 2) { //stage 1 done with 5v P1 and stage 2 with 5V at p2 both with RK at ground so same formula can be used
		//calculatedResistance = (RK * ((VREF / rawValue) - 1));
		//r2 is known find R1
		calculatedResistance = VREF / rawValue;
		calculatedResistance = calculatedResistance - 1;
		calculatedResistance = calculatedResistance * RK;
		return(calculatedResistance);
		//DC EDIT
		/* Both cases R2 is known
		calculatedResistance = (RK*(VREF-rawValue))/VREF); same formula
		*/
	}
	else if (_boxNumber == 4) { //same formula for both as R2 is known
		//calculatedResistance = (((VREF * 1200) / rawValue) - 1200);
		calculatedResistance = VREF * 1200;
		calculatedResistance = calculatedResistance / rawValue;
		calculatedResistance = calculatedResistance - 1200;
		//DC EDIT
		/*
		calculatedResistance = 
		*/
	}
	
	if (stage == 0) {
		if (_boxNumber == 3) {
			//calculatedResistance = (((VREF * 1000) / rawValue) - 1000);
			calculatedResistance = VREF * 1000;
			calculatedResistance = calculatedResistance / rawValue;
			calculatedResistance = calculatedResistance - 1000;
		}
		else if (_boxNumber == 5) {
			//R1 is known find R2
			//calculatedResistance = (rawValue*RK)/(VREF-rawValue);
			calculatedResistance = rawValue*RK;
			double tempresistance = VREF - rawValue;
			calculatedResistance = calculatedResistance / tempresistance;
		}
		else if (_boxNumber == 6) {
			//R2 is known find R1
			//calculatedResistance = (RK*(VREF - rawValue)) / VREF;
			calculatedResistance = VREF / rawValue;
			calculatedResistance = calculatedResistance - 1;
			calculatedResistance = calculatedResistance * RK;
		}
		else if (_boxNumber == 7) {
			//R1 is known find R2
			//calculatedResistance = (rawValue*Rk7)/(VREF-rawValue);
			//calculatedResistance = (rawValue*RK) / (VREF - rawValue);
			calculatedResistance = rawValue*Rk7;
			double tempresistance = VREF - rawValue;
			calculatedResistance = calculatedResistance / tempresistance;
		}
	}
	//DC edit
	/*else if (stage == 1) {
		if (_boxNumber == 3) {
			calculatedResistance = (((rawValue * box3stage 1 answer) / (VREF - rawValue)));
		}
	}
	*/
	else if (_boxNumber ==3) //only stage left for resistance value is box 3 stage 2
	{
			//calculatedResistance = ((rawValue * presentationData.r1)/(VREF - rawValue)); //check presentationdata.r1 maps to box 3 RA
		calculatedResistance = rawValue*presentationData.r1;
		double tempres = VREF - rawValue;
		calculatedResistance = calculatedResistance / tempres;
	}
	return(calculatedResistance);
}

double box::measureCapacitance() {
	uint8_t outPin = P2PIN;
	uint8_t inPin = P1PIN;
	unsigned long u1;
	unsigned long t;
	int digVal;
	unsigned long u2;
	if ((_boxNumber == 5) || (_boxNumber == 7)) {
		inPin = P1PIN;
		outPin = P2PIN;
		pinMode(GNDPIN, INPUT);
		pinMode(_adcPin, INPUT);
		if (_boxNumber == 7){
			pinMode(RK7pin, INPUT);
		}
		else {
			pinMode(RKPIN, INPUT);
		}
	}
	else if (_boxNumber == 6) {
		inPin = GNDPIN;
		outPin = P2PIN;
		pinMode(RKPIN, INPUT);
		pinMode(P1PIN, INPUT);
		pinMode(_adcPin, INPUT);
		
	}

		pinMode2(outPin, OUTPUT);
		//digitalWrite(OUT_PIN, LOW);  //This is the default state for outputs
		pinMode2(inPin, OUTPUT);
		//digitalWrite(IN_PIN, LOW);

		//Capacitor under test between OUT_PIN and IN_PIN
		//Rising high edge on OUT_PIN
		pinMode2(inPin, INPUT);
		digitalWrite2(outPin, HIGH);
		int val = analogRead(inPin);
		digitalWrite2(outPin, LOW);

			//discharge the capacitor (from low capacitance test)
			pinMode(inPin, OUTPUT);
			delay(1);

			//Start charging the capacitor with the internal pullup
			pinMode2(outPin, INPUT_PULLUP);
			u1 = micros();

			//Charge to a fairly arbitrary level mid way between 0 and 5V
			//Best not to use analogRead() here because it's not really quick enough
			do
			{
				digVal = digitalRead2(outPin);
				u2 = micros();
				t = u2 > u1 ? u2 - u1 : u1 - u2;
			} while ((digVal < 1) && (t < 400000L));

			pinMode2(outPin, INPUT);  //Stop charging
			//Now we can read the level the capacitor has charged up to
			val = analogRead(outPin);

			//Discharge capacitor for next measurement
			digitalWrite2(inPin, HIGH);
			t = 400000L;
			int dischargeTime = (int)(t / 1000L) * 5;
			delay(dischargeTime);    //discharge slowly to start with
			pinMode(outPin, OUTPUT);  //discharge remainder quickly
			digitalWrite2(outPin, LOW);
			digitalWrite2(inPin, LOW);


			pinMode2(outPin, INPUT_PULLUP);
			u1 = micros();

			//Charge to a fairly arbitrary level mid way between 0 and 5V
			//Best not to use analogRead() here because it's not really quick enough
			do
			{
				digVal = digitalRead2(outPin);
				u2 = micros();
				t = u2 > u1 ? u2 - u1 : u1 - u2;
			} while ((digVal < 1) && (t < 400000L));

			pinMode2(outPin, INPUT);  //Stop charging
									  //Now we can read the level the capacitor has charged up to
			val = analogRead(outPin);

			//Discharge capacitor for next measurement
			digitalWrite2(inPin, HIGH);
			dischargeTime = (int)(t / 1000L) * 5;
			delay(dischargeTime);    //discharge slowly to start with
			pinMode(outPin, OUTPUT);  //discharge remainder quickly
			digitalWrite2(outPin, LOW);
			digitalWrite2(inPin, LOW);
			//Calculate and print result
			double capacitance = (-(double)t / RPULLUP)
				/ (log(1.0 - (double)val / (double)ADCMAX));

			return capacitance;

}

double box::calculateFrequency() {
	double frequency = 0;

	if ((_boxNumber == 5) || (_boxNumber == 6)) {
		//frequency = (1 / (2 * PI * presentationData.r1 * presentationData.c1));
		frequency = 2 * PI * presentationData.r1 * presentationData.c1 * 1e-9; //* 1e-6;
		frequency = 1 / frequency;
	}
	else{
		frequency = (1 / (2 * PI * sqrt(1e-9 * presentationData.c1)));
	}

	return frequency;
}

double box::calculateFrequency(uint8_t BoxNumber) {
	double frequency = 0;
	if ((BoxNumber == 5) || (BoxNumber == 6)) {
		frequency = 2 * PI * presentationData.r1 * presentationData.c1 * 1e+6;
		frequency = 1 / frequency;
		//frequency = (1 / (2 * PI * presentationData.r1 * presentationData.c1));
	}
	else{
		frequency = (1 / (2 * PI * sqrt(1e-6 * presentationData.c1)));
	}
	return frequency;
}