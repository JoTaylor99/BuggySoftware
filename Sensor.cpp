// 
// 
// 

#include "Sensor.h"

#ifdef QTRSINUSE
static QTRSensorsRC rc[2];
#endif

SFE_TSL2561 tsl;
Adafruit_MCP23017 sensGPIO;


const sC::sensorNumber Sensor::DefaultOrder[8] = { sC::oFrontRight, sC::frontRight, sC::frontLeft, sC::oFrontLeft, sC::middleRight, sC::middleLeft, sC::backRight, sC::backLeft };
const sC::sensorNumber Sensor::Front[4] = { sC::oFrontRight, sC::frontRight, sC::frontLeft, sC::oFrontLeft };
const sC::sensorNumber Sensor::Back[2] = { sC::backRight, sC::backLeft };
const sC::sensorNumber Sensor::Middle[2] = { sC::middleRight, sC::middleLeft };
const sC::sensorNumber Sensor::FrontNMiddle[6] = { sC::oFrontRight, sC::frontRight, sC::frontLeft, sC::oFrontLeft, sC::middleRight, sC::middleLeft };


//uint8_t Sensor::values = 0x00;
bool Sensor::values[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

Sensor::Sensor(uint16_t Pin) {
	_pin = Pin;
	Max = MAX_DEFAULT;
	Min = MIN_DEFAULT;
	Normalised = 0;
	tileWhite = false;
	previoustileWhite = false;
	_s = sC::SensorType::TSL;
}; 

Sensor::Sensor(uint16_t index, sC::SensorType S) {
	_pin = index;
	Max = MAX_DEFAULT;
	Min = MIN_DEFAULT;
	Normalised = 0;
	tileWhite = false;
	previoustileWhite = false;
	_s = S;
};

Sensor::~Sensor(){};

bool Sensor::_sensorInitComplete = false;

//Reads the Raw value from whichever sensor is enabled
void Sensor::ReadRaw() {
	if(_s == sC::SensorType::TSL) {
		Raw = 0;
		if (!tsl.getData(Raw)) {
			ERROR_PRINTLN("I2C ERROR");
		}
		INFO_VPRINTLN(Raw);
	}
	else {
		#ifdef QTRSINUSE
		unsigned int *a;
		rc[_pin].readCalibrated(a);
		Raw = (uint16_t)a;
		#endif
		}
}

//Updates the sensor's range of values for correct scaling
void Sensor::UpdateRange() {
	if (Raw >= Max) {
		Max = Raw;
	}
	else if (Raw <= Min) {
		Min = Raw;
	}
}

//Normalise the sensor's reading on a scale between it's minimum and maximum
void Sensor::Normalise() {
	double n = 1, m = 1; //Internal variable for calculating normalised value
	n = Raw - Min;
	m = (n / Max) * NORMALISED_MAX;
	Normalised = m;
}

//Checks the given sensor values and returns and array of bools.
bool Sensor::GetReading() {
	previoustileWhite = tileWhite;
	ReadRaw();
	UpdateRange();
	Normalise();
	toTileColour();
	return tileWhite;
}

//Switches the I2C address of the desired sensor and removes others
void Sensor::SelectSensor(byte sensorNumber) {
	if (sensorNumber == sC::FR) { sensGPIO.writeGPIOA(0x7F); INFO_PRINTLN("FR Selected"); }
	else if (sensorNumber == sC::LTR) { sensGPIO.writeGPIOA(0xBF); INFO_PRINTLN("LTR Selected"); }
	else if (sensorNumber == sC::LTL) { sensGPIO.writeGPIOA(0xDF); INFO_PRINTLN("LTL Selected"); }
	else if (sensorNumber == sC::FL) { sensGPIO.writeGPIOA(0xEF); INFO_PRINTLN("FL Selected"); }
	else if (sensorNumber == sC::MR) { sensGPIO.writeGPIOA(0xF7); INFO_PRINTLN("MR Selected"); }
	else if (sensorNumber == sC::ML) { sensGPIO.writeGPIOA(0xFB); INFO_PRINTLN("ML Selected"); }
	else if (sensorNumber == sC::BR) { sensGPIO.writeGPIOA(0xFD); INFO_PRINTLN("BR Selected"); }
	else if (sensorNumber == sC::BL) { sensGPIO.writeGPIOA(0xFE); INFO_PRINTLN("BL Selected"); }
	else {
		ERROR_PRINTLN("Sensor Select Error");
		ERROR_PRINTLN("All address lines returned to default");
		sensGPIO.writeGPIOA(0xFF);
	}
}


//Checks the Normalised sensor readings against their thresholds
void Sensor::toTileColour() {
	double WhiteThreshold = NORMALISED_MAX * (double)(1 - (double)((double)UPPER_THRESHOLD / 100)); //Default threshold values of 200 and 800, They can be cahnegd by editing the definitions in the header file
	double BlackThreshold = NORMALISED_MAX * (double)((double)LOWER_THRESHOLD / 100);
	if (Normalised >= WhiteThreshold) {
		tileWhite = true;
	}
	else if (Normalised <= BlackThreshold) {
		tileWhite = false;
	}
}


//Checks the Calculated tileWhite value and returns whether it is valid
void Sensor::LogicCheck(Sensor *sens) {
	if (abs(sens[0].Normalised - sens[1].Normalised) >= LOGIC_THRESHOLD) {
		//Should be different values
		if (sens[0].Normalised < sens[1].Normalised) {
			sens[0].tileWhite = false;
			sens[1].tileWhite = true;
		}
		else {
			sens[0].tileWhite = true;
			sens[1].tileWhite = false;
		}
	}
	else if (abs((double)sens[0].Normalised - (double)sens[1].Normalised) < LOGIC_THRESHOLD) {
	}
	if (abs(sens[2].Normalised - sens[3].Normalised) >= LOGIC_THRESHOLD) {
		//Should be different values
		if (sens[2].Normalised < sens[3].Normalised) {
			sens[2].tileWhite = false;
			sens[3].tileWhite = true;
		}
		else {
			sens[2].tileWhite = true;
			sens[3].tileWhite = false;
		}
	}
	else if (abs(sens[2].Normalised - sens[3].Normalised) < LOGIC_THRESHOLD) {
		//Should be the same
	}
	if (abs(sens[4].Normalised - sens[5].Normalised) >= LOGIC_THRESHOLD) {
		//Should be different values
		if (sens[4].Normalised < sens[5].Normalised) {
			sens[4].tileWhite = false;
			sens[5].tileWhite = true;
		}
		else {
			sens[4].tileWhite = true;
			sens[5].tileWhite = false;
		}
	}
	else if (abs(sens[4].Normalised - sens[5].Normalised) < LOGIC_THRESHOLD) {
		//Should be the same
	}
	if (abs(sens[2].Normalised - sens[4].Normalised) >= LOGIC_THRESHOLD) {
		//Should be different values
		if (sens[2].Normalised < sens[4].Normalised) {
			sens[2].tileWhite = false;
			sens[4].tileWhite = true;
		}
		else {
			sens[4].tileWhite = true;
			sens[2].tileWhite = false;
		}
	}
	if (abs(sens[3].Normalised - sens[5].Normalised) >= LOGIC_THRESHOLD) {
		//Should be different values
		if (sens[3].Normalised < sens[5].Normalised) {
			sens[3].tileWhite = false;
			sens[5].tileWhite = true;
		}
		else {
			sens[5].tileWhite = true;
			sens[3].tileWhite = false;
		}
	}
}


//Detects if the buggy is drifting from a line
Sensor::DriftDirection Sensor::Drifting(Sensor *sens, bool lastCorrectLeft, bool lastCorrectRight) {
	DEBUG_PRINTLN("Checking for drift");
	//DriftDirection CurrentDrift = DriftDirection::None;
	//if (sens[0].tileWhite == sens[1].tileWhite) {
	//	//Front Sensors are the same therefor the buggy is drifting
	//	if (sens[2].tileWhite != sens[3].tileWhite) {
	//		//The middle two sensors are not mis-aligned and therefor can be used for determining which direction to turn
	//		if (sens[2].tileWhite == sens[0].tileWhite) {
	//			//Drifting to the Right
	//			return DriftDirection::DRight;
	//		}
	//		else {
	//			//Drifitng to the Left
	//			return DriftDirection::DLeft;
	//		}
	//	}
	//	else if (sens[4].tileWhite != sens[5].tileWhite) {
	//		//The Middle two sensors are the same and therefore fallback to the back two sensors for alignment
	//		if (sens[4].tileWhite == sens[0].tileWhite) {
	//			//Drifitng to the right
	//			return DriftDirection::DRight;
	DriftDirection CurrentDrift = DriftDirection::None;
	if (sens[0].tileWhite == sens[1].tileWhite) { // If they are the same
		if (lastCorrectLeft != sens[1].tileWhite) {

			// Dirfiting right
			return DRight;
		}
		else {
			//No Dirft
		}
		if (lastCorrectRight != sens[0].tileWhite) {
			//Drift Left
			return DLeft;
		}
		else {
			//No Drift
			//CurrentDrift = None;
		}



		return CurrentDrift;


	}

	//		}
	//		else {
	//			//Drifting to the Left
	//			return DriftDirection::DLeft;

	//		}

	//	}
	//	else {
	//		//No Idea where the buggy is
	//	}






	//}






	{
		//DriftDirection CurrentDrift = DriftDirection::None;
		if (sens[0].tileWhite == sens[1].tileWhite) { // If they are the same
			if (lastCorrectLeft != sens[1].tileWhite) {
				// Dirfiting right
				return DRight;
			}
			else {
				//No Dirft
			}
			if (lastCorrectRight != sens[0].tileWhite) {
				//Drift Left
				return DLeft;
			}
			else {
				//No Drift
				//CurrentDrift = None;

			}

			return CurrentDrift;

		}
		else {
			return DriftDirection::None;
		}
	}
}
//Being called by PollSensors & loop1
//This handles printing of the boolean values of the sensor values (x6), to either the XBee or the USB
//Prints boolean values, presently there is no material difference between the conditions,
//so one has been commented out until such time as communication code is finalised.
void Sensor::printbw(bool *values) {
	//Print via USB
	//if (communications::SMode == communications::SerialMode::USB) {
	DEBUG_PRINT("FL:"); DEBUG_VPRINT(values[sC::FL]); DEBUG_PRINT("\t");
	DEBUG_PRINT("LTL:"); DEBUG_VPRINT(values[sC::LTL]); DEBUG_PRINT("\t");
	DEBUG_PRINT("LTR:"); DEBUG_VPRINT(values[sC::LTR]); DEBUG_PRINT("\t");
	DEBUG_PRINT("FR:"); DEBUG_VPRINT(values[sC::FR]); DEBUG_PRINTLN("\t");
	DEBUG_PRINT("ML:"); DEBUG_VPRINT(values[sC::ML]); DEBUG_PRINT("\t");
	DEBUG_PRINT("MR:"); DEBUG_VPRINT(values[sC::MR]); DEBUG_PRINTLN("\t");
	DEBUG_PRINT("BL:"); DEBUG_VPRINT(values[sC::BL]); DEBUG_PRINT("\t");
	DEBUG_PRINT("BR:"); DEBUG_VPRINT(values[sC::BR]); DEBUG_PRINTLN("\t");
		
	//}
	////Print via wireless Xbee
	//else if (communications::SMode == communications::SerialMode::Xbee) {
	//	Serial.print("S2:"); Serial.print(values[1]); Serial.print("\t");
	//	Serial.print("S1:"); Serial.print(values[0]); Serial.print("\t");
	//	Serial.println();
	//	Serial.print("S4:"); Serial.print(values[3]); Serial.print("\t");
	//	Serial.print("S3:"); Serial.print(values[2]); Serial.print("\t");
	//	Serial.println();
	//	Serial.print("S6:"); Serial.print(values[5]); Serial.print("\t");
	//	Serial.print("S5:"); Serial.print(values[4]); Serial.print("\t");
	//	Serial.println();
	//	Serial.println();
	//}
}

//Polls all given sensors in the order specified.
void Sensor::PollSensors(Sensor *sens, const sC::sensorNumber *order, byte OrderLength){
	byte CurrentSensorIndex = 0;

	for (byte n = 0; n < OrderLength; n++) {
		CurrentSensorIndex = order[n] - 1;
		//  Serial.print("Current Index"); Serial.println(CurrentSensorIndex);
		Sensor::SelectSensor(static_cast<sC::sensorNumber>(CurrentSensorIndex));
		sens[CurrentSensorIndex].GetReading();
		values[CurrentSensorIndex] = sens[CurrentSensorIndex].tileWhite;
		// Serial.print(sens[CurrentSensorIndex].tileWhite); Serial.print("\t"); Serial.print(sens[CurrentSensorIndex].Normalised); Serial.print("\t"); Serial.println(sens[CurrentSensorIndex].Raw);
	}
	//Sensor::LogicCheck(sens);


}


void Sensor::initSensors() {
	sensGPIO.begin(1);

	//Address pins
	sensGPIO.pinMode(OUTERFRONTRIGHTPIN, OUTPUT);
	sensGPIO.pinMode(FRONTRIGHTPIN, OUTPUT);
	sensGPIO.pinMode(FRONTLEFTPIN, OUTPUT);
	sensGPIO.pinMode(OUTERFRONTLEFTPIN, OUTPUT);
	sensGPIO.pinMode(MIDDLERIGHTPIN, OUTPUT);
	sensGPIO.pinMode(MIDDLELEFTPIN, OUTPUT);
	sensGPIO.pinMode(BACKRIGHTPIN, OUTPUT);
	sensGPIO.pinMode(BACKLEFTPIN, OUTPUT);

	//-------Set all sensor select pins to high---------------
	sensGPIO.writeGPIOA(0xFF);

	//Ultrasonic pins
	pinMode(A0, OUTPUT);
	pinMode(A1, INPUT);
	
	////-------Sensor interrupt setups-------

	////setup GPIO pinModes
	//sensGPIO.pinMode(SENSOR1INTPIN, INPUT);
	//sensGPIO.pinMode(SENSOR2INTPIN, INPUT);
	//sensGPIO.pinMode(SENSOR3INTPIN, INPUT);
	//sensGPIO.pinMode(SENSOR4INTPIN, INPUT);
	//sensGPIO.pinMode(SENSOR5INTPIN, INPUT);
	//sensGPIO.pinMode(SENSOR6INTPIN, INPUT);
	//sensGPIO.pinMode(SENSOR7INTPIN, INPUT);
	//sensGPIO.pinMode(SENSOR8INTPIN, INPUT);

	 for (sC::sensorNumber i = sC::oFrontRight; i < sC::invalid; i + 1) {
		SelectSensor(i);
		tsl.begin(TSL2561_ADDR_0);
		tsl.setTiming(TSL2561_GAIN_16X, TSL2561_INTEGRATIONTIME_14MS);
		tsl.setPowerUp();

	}
	delay(14);
	_sensorInitComplete = true;
}
		/*const byte blackTileLowLow = BLACKLOWTHRESHOLDLOWBYTE;
		const byte blackTileLowHigh = BLACKLOWTHRESHOLDHIGHBYTE;
		const byte blackTileHighLow = BLACKHIGHTHRESHOLDLOWBYTE;
		const byte blackTileHighHigh = BLACKHIGHTHRESHOLDLOWBYTE;

		const byte whiteTileLowLow = WHITELOWTHRESHOLDLOWBYTE;
		const byte whiteTileLowHigh = WHITELOWTHRESHOLDHIGHBYTE;
		const byte whiteTileHighLow = WHITEHIGHTHRESHOLDLOWBYTE;
		const byte whiteTileHighHigh = WHITEHIGHTHRESHOLDHIGHBYTE;*/


		//Set interrupt thesholds
		//RIGHT starts as black, black, black, white
		//LEFT starts as white, white, white, black
		//Thresholds set such that value will remain inside bound until change of tile.
		//if ((i == sensorNumber::junctRight) || (i == frontRight) || (i == middleRight) || (i == backLeft)) {
		//	//set thresholds for over black tile
		//	//set low threshold
		//	tsl.write16bits(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | TSL2561_REGISTER_THRESHHOLDL_LOW, blackTileLowLow, blackTileLowHigh);
		//	//set high threshold
		//	tsl.write16bits(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | TSL2561_REGISTER_THRESHHOLDH_LOW, blackTileHighLow, blackTileHighHigh);
		//}
		//else if ((i == junctLeft) || (i == frontLeft) || (i == middleLeft) || (i == backRight)) {
		//set thresholds for over white tile

		//		tsl.write16bits(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | TSL2561_REGISTER_THRESHHOLDL_LOW, whiteTileLowLow, whiteTileLowHigh);
		//		//set high threshold
		//		tsl.write16bits(TSL2561_COMMAND_BIT | TSL2561_WORD_BIT | TSL2561_REGISTER_THRESHHOLDH_LOW, whiteTileHighLow, whiteTileHighHigh);

		//	}


		//	uint8_t _interruptPersistence = TSL2561_INTERRUPT_PERSISTENCE_ONE;

		//	//Set interrupt control register with mode and persistence;
		//	tsl.write8bits(TSL2561_COMMAND_BIT | TSL2561_REGISTER_INTERRUPT, _interruptPersistence | TSL2561_INTERRUPT_LEVELMODE);

		//	//

		//}

		//---------Setup Gains and Intergration time for all sensors
		//tsl.setGain(TSL2561_GAIN_16X);
		//tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // Shortest time (Bright light)
		//tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
		//tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS); //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)
//}




/* UNUSED FUNCTIONS
*
*public static function declared in scope Sensors but never called:
*bool Sensor::OnIntersection(Sensor *sens) {
*	//---------------------------------------------re-write
*	//If Sensor 3 = Sensor 6 OR Sensor 4 = Sensor 5
*	if (sens[2].tileWhite == sens[5].tileWhite && sens[3].tileWhite == sens[4].tileWhite) {
*		return true;
*	}
*
*	//Over Intersection Perfectly
*	if (sens[0].tileWhite == sens[2].tileWhite && sens[0].tileWhite != sens[4].tileWhite ||
*		sens[1].tileWhite == sens[3].tileWhite && sens[1].tileWhite != sens[5].tileWhite) {
*		return true;
*	}
*
*	//Transition Change -- Ommited for now as it requied extra variables to be passed in
*
*	//One Bit error
*	if (sens[2].tileWhite != sens[3].tileWhite || sens[2].tileWhite != sens[4].tileWhite ||
*		sens[5].tileWhite != sens[3].tileWhite || sens[5].tileWhite != sens[4].tileWhite ||
*		sens[3].tileWhite != sens[2].tileWhite || sens[3].tileWhite != sens[5].tileWhite ||
*		sens[4].tileWhite != sens[2].tileWhite || sens[4].tileWhite != sens[5].tileWhite
*		) {
*		//return true;
*	}
*	return false;
*}
*
*	
*public static function declared in scope Sensors but never called:
*bool Sensor::OnInitialIntersection(Sensor *sens, bool *start) {
*	if (sens[2].tileWhite == start[2] &&
*		sens[3].tileWhite == start[3] &&
*		sens[4].tileWhite == start[4] &&
*		sens[5].tileWhite == start[5]
*		) {
*		return true;
*	}
*	return false;
*}
*
*
*public static function declared in scope Sensors but never called:
*bool Sensor::OnLine(Sensor *sens) {
*if ((sens[0].tileWhite != sens[1].tileWhite)) {
*return true;
*}
*else {
*return false;
*}
*}
*
*
*public static function declared in scope Sensors but never called:
*bool Sensor::SensorsCrossedIntersectionCheck(Sensor *sens, bool *start) {
*	bool LeftTarget = !start[1];
*	bool RightTarget = !start[0];
*	Serial.println("In SensorCheck");
*	Serial.print(LeftTarget); Serial.print("\t"); Serial.println(RightTarget);
*	Serial.print(sens[1].tileWhite); Serial.print("\t"); Serial.println(sens[0].tileWhite);
*	if (sens[0].tileWhite == RightTarget && sens[1].tileWhite == LeftTarget && sens[0].tileWhite != sens[1].tileWhite) {
*		Serial.println("In if");
*
*		//passed_intersection_line = true;
*		return true;
*	}
*	else {
*		Serial.println("Else of if");
*		// passed_intersection_line = false;
*		return false;
*	}
*}
*
*
*public static function declared in scope Sensors but never called:
*void Sensor::DiffCheck(Sensor *sens) {
*
*}
*
*
*/
