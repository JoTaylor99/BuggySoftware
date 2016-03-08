// 
// 
// 

#include "Sensor.h"

static TSL2561 tsl;
static QTRSensorsRC rc[2];

const byte Sensor::DefaultOrder[6] = { 1, 2, 3, 4, 5, 6 };
const byte Sensor::Front[2] = { 1, 2 };
const byte Sensor::Back[2] = { 5, 6 };
const byte Sensor::FrontM[2] = { 3, 4 };
const byte Sensor::FrontNMiddle[4] = { 1, 2, 3, 4 };

bool Sensor::values[6] = { false, false, false, false, false, false };

Sensor::Sensor(uint16_t Pin) {
	_pin = Pin;
	Max = MAX_DEFAULT;
	Min = MIN_DEFAULT;
	Normalised = 0;
	Boolian = false;
	PreviousBool = false;
	_s = sensorConfig::SensorType::TSL;
}; 

Sensor::Sensor(uint16_t index, sensorConfig::SensorType S) {
	_pin = index;
	Max = MAX_DEFAULT;
	Min = MIN_DEFAULT;
	Normalised = 0;
	Boolian = false;
	PreviousBool = false;
	_s = S;
};

Sensor::~Sensor(){};

bool Sensor::_sensorInitComplete = false;

void Sensor::ReadRaw() {
	if (_s == sensorConfig::SensorType::TSL) {
		tsl.begin();
		uint32_t lum = 0;
		uint16_t ir = 0;
		lum = tsl.getFullLuminosity();
		ir = lum >> 16;
		Raw = lum & 0xFFFF;
	}
	else {
		unsigned int *a;
		rc[_pin].readCalibrated(a);
		Raw = (uint16_t)a;
	}
}

//Update's the sensor's maximum value for correct scaling
void Sensor::UpdateMaximum() {
	if (Raw >= Max) {
		Max = Raw;
	}
}

//Update's the sensor's minimum value for correct scaling
void Sensor::UpdateMinimum() {
	if (Raw <= Min) {
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


bool Sensor::GetReading() {
	PreviousBool = Boolian;
	ReadRaw();
	UpdateMaximum();
	UpdateMinimum();
	Normalise();
	NormalToBool();
	return Boolian;
}

//Switches the I2C address of the desired sensor and removes others
void Sensor::SelectSensor(uint8_t sensor_number) {
	switch (sensor_number) {
	case 0:
		digitalWrite(SENSOR1, LOW);
		digitalWrite(SENSOR2, HIGH);
		digitalWrite(SENSOR3, HIGH);
		digitalWrite(SENSOR4, HIGH);
		digitalWrite(SENSOR5, HIGH);
		digitalWrite(SENSOR6, HIGH);
		break;
	case 1:
		digitalWrite(SENSOR1, HIGH);
		digitalWrite(SENSOR2, LOW);
		digitalWrite(SENSOR3, HIGH);
		digitalWrite(SENSOR4, HIGH);
		digitalWrite(SENSOR5, HIGH);
		digitalWrite(SENSOR6, HIGH);
		break;
	case 2:
		digitalWrite(SENSOR1, HIGH);
		digitalWrite(SENSOR2, HIGH);
		digitalWrite(SENSOR3, LOW);
		digitalWrite(SENSOR4, HIGH);
		digitalWrite(SENSOR5, HIGH);
		digitalWrite(SENSOR6, HIGH);
		break;
	case 3:
		digitalWrite(SENSOR1, HIGH);
		digitalWrite(SENSOR2, HIGH);
		digitalWrite(SENSOR3, HIGH);
		digitalWrite(SENSOR4, LOW);
		digitalWrite(SENSOR5, HIGH);
		digitalWrite(SENSOR6, HIGH);
		break;
	case 4:
		digitalWrite(SENSOR1, HIGH);
		digitalWrite(SENSOR2, HIGH);
		digitalWrite(SENSOR3, HIGH);
		digitalWrite(SENSOR4, HIGH);
		digitalWrite(SENSOR5, LOW);
		digitalWrite(SENSOR6, HIGH);
		break;
	case 5:
		digitalWrite(SENSOR1, HIGH);
		digitalWrite(SENSOR2, HIGH);
		digitalWrite(SENSOR3, HIGH);
		digitalWrite(SENSOR4, HIGH);
		digitalWrite(SENSOR5, HIGH);
		digitalWrite(SENSOR6, LOW);
		break;
	default:
		digitalWrite(SENSOR1, HIGH);
		digitalWrite(SENSOR2, HIGH);
		digitalWrite(SENSOR3, HIGH);
		digitalWrite(SENSOR4, HIGH);
		digitalWrite(SENSOR5, HIGH);
		digitalWrite(SENSOR6, HIGH);
		break;
	}
}

//Determins weither a detacted sensor reading is Black or White
void Sensor::NormalToBool() {
	ThresholdCheck();
}

//Checks the Normalised sensor readings against their thresholds
void Sensor::ThresholdCheck() {
	double WhiteThreshold = NORMALISED_MAX * (double)(1 - (double)((double)UPPER_THRESHOLD / 100)); //Default threshold values of 200 and 800, They can be cahnegd by editing the definitions in the header file
	double BlackThreshold = NORMALISED_MAX * (double)((double)LOWER_THRESHOLD / 100);
	if (Normalised >= WhiteThreshold) {
		Boolian = true;
	}
	else if (Normalised <= BlackThreshold) {
		Boolian = false;
	}
	else {
		Boolian = Boolian;
	}
}


//Checks the Calculated boolian value and returns weither it is valid
void Sensor::LogicCheck(Sensor *sens) {
	if (abs(sens[0].Normalised - sens[1].Normalised) >= LOGIC_THRESHOLD) {
		//Should be different values
		if (sens[0].Normalised < sens[1].Normalised) {
			sens[0].Boolian = false;
			sens[1].Boolian = true;
		}
		else {
			sens[0].Boolian = true;
			sens[1].Boolian = false;
		}
	}
	else if (abs((double)sens[0].Normalised - (double)sens[1].Normalised) < LOGIC_THRESHOLD) {
	}
	if (abs(sens[2].Normalised - sens[3].Normalised) >= LOGIC_THRESHOLD) {
		//Should be different values
		if (sens[2].Normalised < sens[3].Normalised) {
			sens[2].Boolian = false;
			sens[3].Boolian = true;
		}
		else {
			sens[2].Boolian = true;
			sens[3].Boolian = false;
		}
	}
	else if (abs(sens[2].Normalised - sens[3].Normalised) < LOGIC_THRESHOLD) {
		//Should be the same
	}
	if (abs(sens[4].Normalised - sens[5].Normalised) >= LOGIC_THRESHOLD) {
		//Should be different values
		if (sens[4].Normalised < sens[5].Normalised) {
			sens[4].Boolian = false;
			sens[5].Boolian = true;
		}
		else {
			sens[4].Boolian = true;
			sens[5].Boolian = false;
		}
	}
	else if (abs(sens[4].Normalised - sens[5].Normalised) < LOGIC_THRESHOLD) {
		//Should be the same
	}
	if (abs(sens[2].Normalised - sens[4].Normalised) >= LOGIC_THRESHOLD) {
		//Should be different values
		if (sens[2].Normalised < sens[4].Normalised) {
			sens[2].Boolian = false;
			sens[4].Boolian = true;
		}
		else {
			sens[4].Boolian = true;
			sens[2].Boolian = false;
		}
	}
	if (abs(sens[3].Normalised - sens[5].Normalised) >= LOGIC_THRESHOLD) {
		//Should be different values
		if (sens[3].Normalised < sens[5].Normalised) {
			sens[3].Boolian = false;
			sens[5].Boolian = true;
		}
		else {
			sens[5].Boolian = true;
			sens[3].Boolian = false;
		}
	}
}


//Detects if the buggy is drifting from a line
Sensor::DriftDirection Sensor::Drifting(Sensor *sens, bool lastCorrectLeft, bool lastCorrectRight) {
	Serial.println("Checking for drift");
	//DriftDirection CurrentDrift = DriftDirection::None;
	//if (sens[0].Boolian == sens[1].Boolian) {
	//	//Front Sensors are the same therefor the buggy is drifting
	//	if (sens[2].Boolian != sens[3].Boolian) {
	//		//The middle two sensors are not mis-aligned and therefor can be used for determining which direction to turn
	//		if (sens[2].Boolian == sens[0].Boolian) {
	//			//Drifting to the Right
	//			return DriftDirection::DRight;
	//		}
	//		else {
	//			//Drifitng to the Left
	//			return DriftDirection::DLeft;
	//		}
	//	}
	//	else if (sens[4].Boolian != sens[5].Boolian) {
	//		//The Middle two sensors are the same and therefore fallback to the back two sensors for alignment
	//		if (sens[4].Boolian == sens[0].Boolian) {
	//			//Drifitng to the right
	//			return DriftDirection::DRight;
	DriftDirection CurrentDrift = DriftDirection::None;
	if (sens[0].Boolian == sens[1].Boolian) { // If they are the same
		if (lastCorrectLeft != sens[1].Boolian) {

			// Dirfiting right
			return DRight;
		}
		else {
			//No Dirft
		}
		if (lastCorrectRight != sens[0].Boolian) {
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
		if (sens[0].Boolian == sens[1].Boolian) { // If they are the same
			if (lastCorrectLeft != sens[1].Boolian) {
				// Dirfiting right
				return DRight;
			}
			else {
				//No Dirft
			}
			if (lastCorrectRight != sens[0].Boolian) {
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

//Prints boolean values, presently there is no material difference between the conditions, so one has been commented out until such time as communication code is finalised.
void Sensor::printbw(bool *values) {
	//Print via USB
	//if (communications::SMode == communications::SerialMode::USB) {
		Serial.print("S2:"); Serial.print(values[1]); Serial.print("\t");
		Serial.print("S1:"); Serial.print(values[0]); Serial.print("\t");
		Serial.println();
		Serial.print("S4:"); Serial.print(values[3]); Serial.print("\t");
		Serial.print("S3:"); Serial.print(values[2]); Serial.print("\t");
		Serial.println();
		Serial.print("S6:"); Serial.print(values[5]); Serial.print("\t");
		Serial.print("S5:"); Serial.print(values[4]); Serial.print("\t");
		Serial.println();
		Serial.println();
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
void Sensor::PollSensors(Sensor *sens, const byte *order, byte OrderLength){
	int CurrentSensorIndex = 0;

	for (int n = 0; n < OrderLength; n++) {
		CurrentSensorIndex = order[n] - 1;
		//  Serial.print("Current Index"); Serial.println(CurrentSensorIndex);
		Sensor::SelectSensor(CurrentSensorIndex);
		sens[CurrentSensorIndex].GetReading();
		values[CurrentSensorIndex] = sens[CurrentSensorIndex].Boolian;
		// Serial.print(sens[CurrentSensorIndex].Boolian); Serial.print("\t"); Serial.print(sens[CurrentSensorIndex].Normalised); Serial.print("\t"); Serial.println(sens[CurrentSensorIndex].Raw);
	}
	Sensor::LogicCheck(sens);

	printbw(values);
	// Serial.println();
}


void Sensor::initSensors() {
	pinMode(SENSOR1, OUTPUT);
	pinMode(SENSOR2, OUTPUT);
	pinMode(SENSOR3, OUTPUT);
	pinMode(SENSOR4, OUTPUT);
	pinMode(SENSOR5, OUTPUT);
	pinMode(SENSOR6, OUTPUT);

	pinMode(A0, OUTPUT);
	pinMode(A1, INPUT);

	//-------Set all sensor select pins to high---------------
	digitalWrite(SENSOR1, HIGH);
	digitalWrite(SENSOR2, HIGH);
	digitalWrite(SENSOR3, HIGH);
	digitalWrite(SENSOR4, HIGH);
	digitalWrite(SENSOR5, HIGH);
	digitalWrite(SENSOR6, HIGH);
	
	//---------Setip Gains and Intergration time for all sensors
	tsl.setGain(TSL2561_GAIN_16X);
	tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // Shortest time (Bright light)
	//tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
	//tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS); //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)

	_sensorInitComplete = true;
}




/* UNUSED FUNCTIONS
*
*public static function declared in scope Sensors but never called:
*bool Sensor::OnIntersection(Sensor *sens) {
*	//---------------------------------------------re-write
*	//If Sensor 3 = Sensor 6 OR Sensor 4 = Sensor 5
*	if (sens[2].Boolian == sens[5].Boolian && sens[3].Boolian == sens[4].Boolian) {
*		return true;
*	}
*
*	//Over Intersection Perfectly
*	if (sens[0].Boolian == sens[2].Boolian && sens[0].Boolian != sens[4].Boolian ||
*		sens[1].Boolian == sens[3].Boolian && sens[1].Boolian != sens[5].Boolian) {
*		return true;
*	}
*
*	//Transition Change -- Ommited for now as it requied extra variables to be passed in
*
*	//One Bit error
*	if (sens[2].Boolian != sens[3].Boolian || sens[2].Boolian != sens[4].Boolian ||
*		sens[5].Boolian != sens[3].Boolian || sens[5].Boolian != sens[4].Boolian ||
*		sens[3].Boolian != sens[2].Boolian || sens[3].Boolian != sens[5].Boolian ||
*		sens[4].Boolian != sens[2].Boolian || sens[4].Boolian != sens[5].Boolian
*		) {
*		//return true;
*	}
*	return false;
*}
*
*	
*public static function declared in scope Sensors but never called:
*bool Sensor::OnInitialIntersection(Sensor *sens, bool *start) {
*	if (sens[2].Boolian == start[2] &&
*		sens[3].Boolian == start[3] &&
*		sens[4].Boolian == start[4] &&
*		sens[5].Boolian == start[5]
*		) {
*		return true;
*	}
*	return false;
*}
*
*
*public static function declared in scope Sensors but never called:
*bool Sensor::OnLine(Sensor *sens) {
*if ((sens[0].Boolian != sens[1].Boolian)) {
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
*	Serial.print(sens[1].Boolian); Serial.print("\t"); Serial.println(sens[0].Boolian);
*	if (sens[0].Boolian == RightTarget && sens[1].Boolian == LeftTarget && sens[0].Boolian != sens[1].Boolian) {
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
