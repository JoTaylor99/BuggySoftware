
//Function to pick the prefered resistor from a given resistor value range.
short PrefResistor(double OResistor, short BoxNo) {
	const short PResistors[] = { 20, 22, 24, 27, 30, 33, 36, 39, 43, 47, 51, //10
		56, 62, 68, 75, 82, 91, //16
		100, 110, 120, 130, 150, 160, 180, //23
		200, 220, 240, 270, 300, 330, 360, 390, 430, 470, //33
		510, 560, 620, 680, 750, 820, 910, //40
		1000, 1100, 1200, 1300, 1400, 1500, 1600, 1800, //48
		2000, 2200, 2400, 2700, 3000 }; //53
	short FinalResistor = 0;
	short IMAX = 53;
	short i = 0; //To define index of array.
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
}


//Function to pick the prefered capacitor from a given capacitor value range.
double PrefCapactitor(double OCap, short BoxNo) {
	const double PCapacitors[] = { 4.7, 5.6, 6.8, 8.2, 10, 12, 15, 18, //7
		22, 27, 33, 39, 47, 56, 68, 82, 100}; //16
	double FinalCap = 0;
	short IMAX = 16;
	short i = 0; //To define index of array.
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
