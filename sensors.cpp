
#include "sensorsConfig.h"
#include "sensorDefs.h"
#include <NewPing.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "TSL2561.h"

//Initial variable declaration:
int Counter = 0;
//Function declarations:
void Kick(KickDirection dir, int Magnitude);
void Kick(KickDirection dir, int Magnitude, uint16_t time);
void Kick(KickDirection dir, int LeftMagnitude, int RightMagnitude);
void Kick(KickDirection LeftDir, KickDirection RightDir, int LeftMag, int RightMag);
void MotorRamp(Direction Dir, int LMag, int RMag);
void DriftCorrect(DriftDirection Dir);
void start();
void moveForward();
void TurnRight();
void TurnLeft();
void smartAlignmentRotation();
void smartAlignmentForward();


//CLASS----------------------------------------------------------
class Sensor {
public:
	Sensor(uint16_t Pin);

	uint16_t Max;
	uint16_t Min;
	uint16_t Raw;
	double Normalised;
	bool Boolian;
	bool PreviousBool;


	bool GetReading();
	static void SelectSensor(uint8_t sensor_number);
	//Checks if buggy is on an intersection
	static bool OnIntersection(Sensor *sens);
	static bool OnInitialIntersection(Sensor *sens, bool *start);
	static bool OnLine(Sensor *sens);
	static DriftDirection Drifting(Sensor *sens);
	static void LogicCheck(Sensor *sens);
	static void PollSensors(Sensor *sens, int *order, int OrderLength);
	static bool CompareBools(Sensor * sens, bool *bools);
	static bool StartComparison(bool *start, Sensor *sens);
	static bool AnyDifferent(bool *start, Sensor *sens);
	static bool SensorsCrossedIntersectionCheck(Sensor *sens, bool *start);
private:
	uint16_t _pin;
	void ReadRaw();
	void UpdateMaximum();
	void UpdateMinimum();
	void Normalise();
	void NormalToBool();
	void ThresholdCheck();

};
//---------------------------------------------------------------
//Variable Declarations:
TSL2561 tsl(TSL2561_ADDR_LOW);
OutputMode OMode = BW;
SerialMode SMode = Xbee; //XBee or USB
Sensor Sensors[6] = { SENSOR1 , SENSOR2, SENSOR3, SENSOR4, SENSOR5, SENSOR6 };
NewPing Ultrasonic(TRIGGER, ECHO, MAXDISTANCE);
bool MoveComplete = false;
State Current = Initial;
int DefaultOrder[6] = { 1, 2, 3, 4, 5, 6 };
int TransistionOrder[10] = { 1, 2, 1, 2, 1, 2, 3, 4, 5, 6 };
int Front[2] = { 1, 2 };
int Back[2] = { 5, 6 };
int FrontM[2] = { 3, 4 };
int FrontNMiddle[4] = { 1, 2, 3, 4 };
int FrontNBack[4] = { 1, 2, 5, 6 };
int Middle[4] = { 3, 4, 5, 6 };
bool logic[6] = { false, false, false, false, false, false };
uint32_t lum = 0;
uint16_t ir = 0, full = 0;
bool LastCorrectLeft = false;
bool LastCorrectRight = false;
bool values[6] = { false, false, false, false, false, false };
bool starting_intersection[6] = { false, false, false, false, false, false }; //array to capture the sensor values at the starting position
																			  //Flag Variables to help identify if the first two sensors or the last two sensors have passed the intersection
bool passed_intersection_line = false;    //forward movement
bool passed_intersection_lineb = false;   // backward movement
bool sensor0_event = false;
bool sensor1_event = false;
bool sensor4_event = false;
bool sensor5_event = false;
bool flag0 = false;
bool flag1 = false;
bool flag4 = false;
bool flag5 = false;

int IntersectionCount = 0;
uint16_t quadrant = 0;  //
//----------------------------------------------------

Sensor::Sensor(uint16_t Pin) {
	_pin = Pin;
	Max = MAX_DEFAULT;
	Min = MIN_DEFAULT;
	Normalised = 0;
	Boolian = false;
	PreviousBool = false;
}
//Checks the given sensor values and returns and array of bools.
bool Sensor::GetReading() {
	PreviousBool = Boolian;
	ReadRaw();
	UpdateMaximum();
	UpdateMinimum();
	Normalise();
	NormalToBool();
	return Boolian;
}

//Reads the Raw value from whichever sensor is enabled
void Sensor::ReadRaw() {
	tsl.begin();
	lum = tsl.getFullLuminosity();
	ir, full;
	ir = lum >> 16;
	Raw = lum & 0xFFFF;
	//delay(5);
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

//Switchjes the I2C address of the desired sensor and removes others
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
bool Sensor::OnIntersection(Sensor *sens) {
	//---------------------------------------------re-write
	//If Sensor 3 = Sensor 6 OR Sensor 4 = Sensor 5
	if (sens[2].Boolian == sens[5].Boolian && sens[3].Boolian == sens[4].Boolian) {
		return true;
	}

	//Over Intersection Perfectly
	if (sens[0].Boolian == sens[2].Boolian && sens[0].Boolian != sens[4].Boolian ||
		sens[1].Boolian == sens[3].Boolian && sens[1].Boolian != sens[5].Boolian) {
		return true;
	}

	//Transition Change -- Ommited for now as it requied extra variables to be passed in

	//One Bit error
	if (sens[2].Boolian != sens[3].Boolian || sens[2].Boolian != sens[4].Boolian ||
		sens[5].Boolian != sens[3].Boolian || sens[5].Boolian != sens[4].Boolian ||
		sens[3].Boolian != sens[2].Boolian || sens[3].Boolian != sens[5].Boolian ||
		sens[4].Boolian != sens[2].Boolian || sens[4].Boolian != sens[5].Boolian
		) {
		//return true;
	}
	return false;
}
bool Sensor::OnInitialIntersection(Sensor *sens, bool *start) {
	if (sens[2].Boolian == start[2] &&
		sens[3].Boolian == start[3] &&
		sens[4].Boolian == start[4] &&
		sens[5].Boolian == start[5]
		) {
		return true;
	}
	return false;
}
//Determines if the buggy is on a line. True == Yes / False == No
bool Sensor::OnLine(Sensor *sens) {
	if ((sens[0].Boolian != sens[1].Boolian)) {
		return true;
	}
	else {
		return false;
	}

}
bool Sensor::SensorsCrossedIntersectionCheck(Sensor *sens, bool *start) {
	bool LeftTarget = !start[1];
	bool RightTarget = !start[0];
	Serial1.println("In SensorCheck");
	Serial1.print(LeftTarget); Serial1.print("\t"); Serial1.println(RightTarget);
	Serial1.print(sens[1].Boolian); Serial1.print("\t"); Serial1.println(sens[0].Boolian);
	if (sens[0].Boolian == RightTarget && sens[1].Boolian == LeftTarget && sens[0].Boolian != sens[1].Boolian) {
		Serial1.println("In if");

		//passed_intersection_line = true;
		return true;
	}
	else {
		Serial1.println("Else of if");
		// passed_intersection_line = false;
		return false;
	}


}

//Detects if the buggy is drifting from a line
DriftDirection Sensor::Drifting(Sensor *sens) {
	//if (true != true) { // Only works while in transition state
	//  return NotInTransition;
	//}
	{
		DriftDirection CurrentDrift = DriftDirection::None;
		if (sens[0].Boolian == sens[1].Boolian) { // If they are the same

			if (LastCorrectLeft != sens[1].Boolian) {
				// Dirfiting right
				return DRight;
			}
			else {
				//No Dirft
			}
			if (LastCorrectRight != sens[0].Boolian) {
				//Drift Left
				return DLeft;
			}
			else {
				//No Drift
				//CurrentDrift = None;
				// CurrentDrift = None;
			}



			return CurrentDrift;


		}
	}
}
//Being called by PollSensors & loop1
//This handles printing of the boolean values of the sensor values (x6), to either the XBee or the USB
void printbw(bool *values) {
	//Print via USB
	if (SMode == USB) {
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


	}
	//Print via wireless Xbee
	else if (SMode == Xbee) {
		Serial1.print("S2:"); Serial1.print(values[1]); Serial1.print("\t");
		Serial1.print("S1:"); Serial1.print(values[0]); Serial1.print("\t");
		Serial1.println();
		Serial1.print("S4:"); Serial1.print(values[3]); Serial1.print("\t");
		Serial1.print("S3:"); Serial1.print(values[2]); Serial1.print("\t");
		Serial1.println();
		Serial1.print("S6:"); Serial1.print(values[5]); Serial1.print("\t");
		Serial1.print("S5:"); Serial1.print(values[4]); Serial1.print("\t");
		Serial1.println();
		Serial1.println();
	}
}
void Kick(KickDirection dir, int Magnitude) {
	if (dir == KickDirection::Forward) {
		analogWrite(E1, Magnitude);
		digitalWrite(M1, HIGH);
		analogWrite(E2, Magnitude);
		digitalWrite(M2, HIGH);
		delay(DEFAULT_KICK_TIME);
		analogWrite(E1, 0);
		digitalWrite(M1, LOW);
		analogWrite(E2, 0);
		digitalWrite(M2, LOW);
	}
	else if (dir == KickDirection::Backward) {
		analogWrite(E1, Magnitude);
		digitalWrite(M1, LOW);
		analogWrite(E2, Magnitude);
		digitalWrite(M2, LOW);
		delay(DEFAULT_KICK_TIME);
		analogWrite(E1, 0);
		digitalWrite(M1, HIGH);
		analogWrite(E2, 0);
		digitalWrite(M2, HIGH);
	}
	else if (dir == KickDirection::Left) {
		analogWrite(E1, Magnitude);
		digitalWrite(M1, HIGH);
		analogWrite(E2, Magnitude);
		digitalWrite(M2, LOW);
		delay(DEFAULT_KICK_TIME);
		analogWrite(E1, 0);
		digitalWrite(M1, HIGH);
		analogWrite(E2, 0);
		digitalWrite(M2, HIGH);
	}
	else if (dir == KickDirection::Right) {
		analogWrite(E1, Magnitude);
		digitalWrite(M1, LOW);
		analogWrite(E2, Magnitude);
		digitalWrite(M2, HIGH);
		delay(DEFAULT_KICK_TIME);
		analogWrite(E1, 0);
		digitalWrite(M1, HIGH);
		analogWrite(E2, 0);
		digitalWrite(M2, HIGH);
	}
	return;
}
void Kick(KickDirection dir, int Magnitude, uint16_t time) {
	if (dir == KickDirection::Forward) {
		analogWrite(E1, Magnitude);
		digitalWrite(M1, HIGH);
		analogWrite(E2, Magnitude);
		digitalWrite(M2, HIGH);
		delay(time);
		analogWrite(E1, 0);
		digitalWrite(M1, LOW);
		analogWrite(E2, 0);
		digitalWrite(M2, LOW);
	}
	else if (dir == KickDirection::Backward) {
		analogWrite(E1, Magnitude);
		digitalWrite(M1, LOW);
		analogWrite(E2, Magnitude);
		digitalWrite(M2, LOW);
		delay(time);
		analogWrite(E1, 0);
		digitalWrite(M1, HIGH);
		analogWrite(E2, 0);
		digitalWrite(M2, HIGH);
	}
	else if (dir == KickDirection::Left) {
		analogWrite(E1, Magnitude);
		digitalWrite(M1, HIGH);
		analogWrite(E2, Magnitude);
		digitalWrite(M2, LOW);
		delay(time);
		analogWrite(E1, 0);
		digitalWrite(M1, HIGH);
		analogWrite(E2, 0);
		digitalWrite(M2, HIGH);
	}
	else if (dir == KickDirection::Right) {
		analogWrite(E1, Magnitude);
		digitalWrite(M1, LOW);
		analogWrite(E2, Magnitude);
		digitalWrite(M2, HIGH);
		delay(time);
		analogWrite(E1, 0);
		digitalWrite(M1, HIGH);
		analogWrite(E2, 0);
		digitalWrite(M2, HIGH);
	}
	return;
}
void Kick(KickDirection dir, int LeftMagnitude, int RightMagnitude) {
	if (dir == KickDirection::Forward) {
		analogWrite(E1, RightMagnitude);
		digitalWrite(M1, HIGH);
		analogWrite(E2, LeftMagnitude);
		digitalWrite(M2, HIGH);
		delay(DEFAULT_KICK_TIME);
		analogWrite(E1, 0);
		digitalWrite(M1, LOW);
		analogWrite(E2, 0);
		digitalWrite(M2, LOW);
	}
	else if (dir == KickDirection::Backward) {
		analogWrite(E1, RightMagnitude);
		digitalWrite(M1, LOW);
		analogWrite(E2, LeftMagnitude);
		digitalWrite(M2, LOW);
		delay(DEFAULT_KICK_TIME);
		analogWrite(E1, 0);
		digitalWrite(M1, HIGH);
		analogWrite(E2, 0);
		digitalWrite(M2, HIGH);
	}
	else if (dir == KickDirection::Left) {
		analogWrite(E1, RightMagnitude);
		digitalWrite(M1, HIGH);
		analogWrite(E2, LeftMagnitude);
		digitalWrite(M2, LOW);
		delay(DEFAULT_KICK_TIME);
		analogWrite(E1, 0);
		digitalWrite(M1, HIGH);
		analogWrite(E2, 0);
		digitalWrite(M2, HIGH);
	}
	else if (dir == KickDirection::Right) {
		analogWrite(E1, RightMagnitude);
		digitalWrite(M1, LOW);
		analogWrite(E2, LeftMagnitude);
		digitalWrite(M2, HIGH);
		delay(DEFAULT_KICK_TIME);
		analogWrite(E1, 0);
		digitalWrite(M1, HIGH);
		analogWrite(E2, 0);
		digitalWrite(M2, HIGH);
	}
	return;
}
void Kick(KickDirection LeftDir, KickDirection RightDir, int LeftMag, int RightMag) {
	if (LeftDir == RightDir) {
		Kick(LeftDir, LeftMag, RightMag);
	}
	else {
		Kick(LeftDir, LeftMag, 0);
		Kick(RightDir, 0, RightMag);
	}

}

//Polls all given sensors in the order specified.
void Sensor::PollSensors(Sensor *sens, int *order, int OrderLength) {
	int CurrentSensorIndex = 0;

	for (int n = 0; n < OrderLength; n++) {

		CurrentSensorIndex = order[n] - 1;
		//  Serial1.print("Current Index"); Serial1.println(CurrentSensorIndex);
		Sensor::SelectSensor(CurrentSensorIndex);
		sens[CurrentSensorIndex].GetReading();
		values[CurrentSensorIndex] = sens[CurrentSensorIndex].Boolian;
		//Serial1.print(sens[CurrentSensorIndex].Boolian); Serial1.print("\t"); Serial1.print(sens[CurrentSensorIndex].Normalised); Serial1.print("\t"); Serial1.println(sens[CurrentSensorIndex].Raw);
	}
	Sensor::LogicCheck(sens);
	printbw(values);
	Serial1.println();
}

//Sensors setup function.
void setup_sensors() {
	Serial.begin(115200); //Setup serial conenction over USB
	Serial1.begin(112500); // Setup serial connection over Xbee
						   //-------Setup Pins used for sensor select as outputs---------------
	pinMode(SENSOR1, OUTPUT);
	pinMode(SENSOR2, OUTPUT);
	pinMode(SENSOR3, OUTPUT);
	pinMode(SENSOR4, OUTPUT);
	pinMode(SENSOR5, OUTPUT);
	pinMode(SENSOR6, OUTPUT);
	pinMode(M1, OUTPUT);
	pinMode(M2, OUTPUT);
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
	tsl.begin();
	tsl.setGain(TSL2561_GAIN_16X);
	tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // Shortest time (Bright light)
												  //tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
												  //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS); //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)
	if (SMode == USB) {
		Serial.print("Setup Complete!");
	}
	else {
		Serial1.print("Setup Complete!");
	}
}

//void BoxApproach() {
//  int Distance = 0;
//  while (true) {
//    Distance = Ultrasonic.ping_cm();
//    if (Distance > 3) {
//      Serial1.println("Approaching Box");
//      if (LineCorrect() == false) {
//        Kick(KickDirection::Forward, KICK_MAGNITUDE, (uint16_t)10);
//      }
//    }
//    else {
//      Serial1.println("I Have Reached the box");
//      if (Distance == 0) {
//        Serial1.println("Either too far or too close");
//      }
//      return;
//    }
//    delay(50);
//  }
//}
void loop1() {
	/*
	for (int i = 0; i < 6; i++) {
	Sensor::SelectSensor(i);
	values[i] = Sensors[i].GetReading();
	//Serial1.println(Sensors[i].Normalised);
	}*/
	//int DefaultOrder[6] = { 1,2,3,4,5,6 };
	digitalWrite(M1, LOW);
	digitalWrite(M2, LOW);
	analogWrite(E1, 70);
	analogWrite(E2, 70);
	Sensor::PollSensors(Sensors, DefaultOrder, 6);

	printbw(values);
	//  sensor_events();
	//Serial.println();

	//Serial1.println("---------------------");
	//delay(50);

	if (Serial1.available() > 0) { // If a command has been sent
		String str = Serial1.readString();
		if (str == "F") {
			sensor0_event = false;
			sensor1_event = false;
			flag0 = false;
			flag1 = false;
			start();
			moveForward();
			/*whereAmI();
			findLineS45() ; //This needs to be changed with the new base
			findLineS01();*/


		}
		if (str == "R") {
			start();
			TurnRight();
			smartAlignmentRotation();
		}
		if (str == "L") {
			start();
			TurnLeft();
			smartAlignmentRotation();
		}
		if (str == "A") {
			smartAlignmentForward();
		}
		if (str == "B") {

		}
	}
}

bool LineCorrect() {
	if (Sensor::Drifting(Sensors) == DriftDirection::DLeft) {
		if (Counter < COUNTER_LEN) {
			Kick(KickDirection::Forward, KickDirection::Backward, KICK_MAGNITUDE, KICK_MAGNITUDE);
			Counter++;
			return true;
		}
		else {
			Counter = 0;
			return false;
		}

	}
	else if (Sensor::Drifting(Sensors) == DriftDirection::DRight) {
		if (Counter < COUNTER_LEN) {
			Kick(KickDirection::Backward, KickDirection::Forward, KICK_MAGNITUDE, KICK_MAGNITUDE);
			Counter++;
			return true;
		}
		else {
			Counter = 0;
			return false;
		}

	}
	else if (Sensor::Drifting(Sensors) == DriftDirection::None) {
		Counter = 0;
		LastCorrectLeft = Sensors[1].Boolian;
		LastCorrectRight = Sensors[0].Boolian;
		return false;
	}
	else {
		Counter = 0;
		return false;
	}
}

//Function to define whether or not the destination node has been reached.
//If so it returns true and prints "Destination Reached" to the terminal, if not returns false.
bool reachedDestination() {
	if ((Sensors[0].Boolian != starting_intersection[0]) && (Sensors[1].Boolian != starting_intersection[1]) && (Sensors[2].Boolian != starting_intersection[2]) &&
		(Sensors[3].Boolian != starting_intersection[3]) && (Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5])) {
		Serial1.println("Destination Reached");
		return true;
	}
	//  else if ((Sensors[2].Boolian != starting_intersection[2]) && (Sensors[3].Boolian != starting_intersection[3])
	//           && (Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5])) {
	//    Serial1.println("Destination Reached2");
	//    return true;
	//  }
	else {
		return false;
	}
}

//captures and stores in an array all the sensor values at the initial node position
void start() {
	Sensor::PollSensors(Sensors, DefaultOrder, 6);
	for (int n = 0; n < 6; n++) {
		starting_intersection[n] = values[n];
	}
	Serial1.println("I have the starting POSITION");
}
//----------------FORWARD ALIGNMENT----------------------------------------------------------------
//Checks if there was an event in the values of the first two sensors during the forward motion
void sensorEvents() {


	if (Sensors[0].Boolian != starting_intersection[0]) {
		sensor0_event = true;
		flag0 = true;
	}
	if (Sensors[1].Boolian != starting_intersection[1]) {
		sensor1_event = true;
		flag1 = true;
	}
}
//Checks if the buggy has passed the line of the destination intersection
void didIPassIntersectionLine() {
	//ideal-case Both sensors have passed the intersection line normally
	if ((Sensors[0].Boolian != starting_intersection[0]) && (Sensors[1].Boolian != starting_intersection[1]) || ((sensor0_event == true) && (sensor1_event == true)))
	{
		passed_intersection_line = true;
		Serial1.println("Yes I passed the intersection line");
	}
	else {
		passed_intersection_line = false;
		Serial1.println("No I havent passed intersection line");
	}
}
//Quadrants Description
//Forward  S0.S2 are in Q4, S1 and S3 are inQ1              //Backwards
//S4 Q3 and S5 Q2                            S4 at Q4, S5 at Q1, S3S1 at Q2, S2S0 at Q3
/* Destination Note                            Destination Note
Q2|Q1                                            Q1|Q2
----|-----     Q0 is the on line case              --|--
Q3|Q4                                            Q4 Q3
*/
// Function to identify the quadrant where the buggy is at the moment
void whereAmI() {
	if (Sensors[0].Boolian == Sensors[1].Boolian) {
		if (passed_intersection_line == true) {
			if (Sensors[0].Boolian != starting_intersection[0]) {
				Serial1.println("I am at Q4");
				quadrant = 4;
			}
			else {
				Serial1.println(" I am at Q1");
				quadrant = 1;
			}
		}
		else {
			if (Sensors[0].Boolian != starting_intersection[0]) {
				Serial1.println("I am at Q2");
				quadrant = 2;
			}
			else {
				Serial1.println("I am at Q3");
				quadrant = 3;
			}
		}
	}
	else {
		quadrant = 0; //S0 and S1 on line
	}
}

//Come back to this one!
void findLineS23() {
	bool smart_line_s45 = false;
	switch (quadrant) {
	case 1: while (smart_line_s45 == false) {
		//Sensor::PollSensors(Sensors, Back, 2);
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (Sensors[2].Boolian != (Sensors[3].Boolian)) {
			smart_line_s45 = true;
		}
		else {
			Serial1.println("Q1 NEED TO KICK BACKWARDS RIGHT");
			Kick(KickDirection::Backward, KickDirection::Forward, 255, 0);
		}
	}
			break;
	case 2: while (smart_line_s45 == false) {
		// Sensor::PollSensors(Sensors, Back, 2);
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (Sensors[2].Boolian != (Sensors[3].Boolian)) {
			smart_line_s45 = true;
		}
		else {
			Serial1.println("Q2 NEED TO KICK BACKWARDS RIGHT");
			Kick(KickDirection::Backward, KickDirection::Forward, KICK_MAGNITUDE, 0);

		}
	}
			break;
	case 3:  while (smart_line_s45 == false) {
		// Sensor::PollSensors(Sensors, Back, 2);
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (Sensors[2].Boolian != (Sensors[3].Boolian)) {
			smart_line_s45 = true;
		}
		else {
			Serial1.println("Q3 NEED TO KICK BACKWARDS LEFT");
			Kick(KickDirection::Forward, KickDirection::Backward, 0, KICK_MAGNITUDE);

		}
	}
			 break;
	case 4:  while (smart_line_s45 == false) {
		// Sensor::PollSensors(Sensors, Back, 2);
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (Sensors[2].Boolian != (Sensors[3].Boolian)) {
			smart_line_s45 = true;
		}
		else {
			Serial1.println("Q4 NEED TO KICK BACKWARDS LEFT");
			Kick(KickDirection::Forward, KickDirection::Backward, 0, KICK_MAGNITUDE);

		}
	}
			 break;
	default:
		break;
	}
}
// Based on the quadrant the buggy rotates left or right to get back to the motion line with sensors 1 and 2
void findLineS01() {
	bool smart_line_s01 = false;
	switch (quadrant) {
	case 1: while (smart_line_s01 == false) {
		//   Sensor::PollSensors(Sensors, Front, 2);
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (Sensors[0].Boolian != (Sensors[1].Boolian)) {
			smart_line_s01 = true;
		}
		else {
			Serial1.println("Q1 NEED TO ROTATE RIGHT");
			Kick(KickDirection::Right, KICK_MAGNITUDE);

		}
	}
			break;
	case 2: while (smart_line_s01 == false) {
		//Sensor::PollSensors(Sensors, Front, 2);
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (Sensors[0].Boolian != (Sensors[1].Boolian)) {
			smart_line_s01 = true;
		}
		else {
			Serial1.println("Q2 NEED TO ROTATE RIGHT");
			Kick(KickDirection::Right, KICK_MAGNITUDE);

		}
	}
			break;
	case 3: while (smart_line_s01 == false) {
		//Sensor::PollSensors(Sensors, Front, 2);
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (Sensors[0].Boolian != (Sensors[1].Boolian)) {
			smart_line_s01 = true;
		}
		else {
			Serial1.println("Q3 NEED TO ROTATE LEFT");
			Kick(KickDirection::Left, KICK_MAGNITUDE);

		}
	}
			break;
	case 4: while (smart_line_s01 == false) {
		//  Sensor::PollSensors(Sensors, Front, 2);
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (Sensors[0].Boolian != (Sensors[1].Boolian)) {
			smart_line_s01 = true;
		}
		else {
			Serial1.println("Q4 NEED TO ROTATE LEFT");
			Kick(KickDirection::Left, KICK_MAGNITUDE);
		}
	}
			break;
	default:
		break;
	}
}
//Identifies whether sensors 0 and 1 have passed the node in the forward motion
void passedNote() {
	bool passed_s01 = false;
	while (passed_s01 == false) {
		Sensor::PollSensors(Sensors, Front, 2);
		//Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (reachedDestination() == true) {
			return;
		}
		if ((Sensors[0].Boolian != starting_intersection[0]) && (Sensors[1].Boolian != starting_intersection[1])) {
			passed_s01 = true;
		}
		else {
			Serial1.println("Need to kick forward");
			if (LineCorrect() == false) {
				Kick(KickDirection::Forward, KICK_MAGNITUDE);
			}
		}
	}
}
void reachNote() {
	bool passed_s23 = false;
	while (passed_s23 == false) {
		//Sensor::PollSensors(Sensors, FrontM, 2);
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (reachedDestination() == true) {
			return;
		}
		if ((Sensors[2].Boolian != starting_intersection[0]) && (Sensors[3].Boolian != starting_intersection[1])) {
			passed_s23 = true;
		}
		else {
			Serial1.println("Need to kick forward on reach_node");
			if (LineCorrect() == false) {
				Kick(KickDirection::Forward, KICK_MAGNITUDE);
			}
		}
	}
}

//Smart Alignment Function for Forward movement.
void smartAlignmentForward() {
	bool perfect_intersection = false;
	whereAmI();
	while (perfect_intersection == false) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if ((Sensors[0].Boolian != starting_intersection[0]) && (Sensors[1].Boolian != starting_intersection[1]) && (Sensors[2].Boolian != starting_intersection[2]) &&
			(Sensors[3].Boolian != starting_intersection[3]) && (Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5])) {
			perfect_intersection = true;
			Serial1.println(" FINISHED");
		}
		else {
			findLineS23();
			findLineS01();
			// on_line_all();
			if (reachedDestination() == true) {
				return;
			}
			passedNote();
			if (reachedDestination() == true) {
				return;
			}
			//reached_destination()
			if (reachedDestination() == true) {
				return;
			}
			reachNote();
			if (reachedDestination() == true) {
				return;
			}
		}
	}
}
//----------------ROTATION ALIGNMENT----------------------------------------------------------------------------------------------------------------------------------------------------
//Rotation strategy
//1) Get back to the line with s0 and s1
//2) Follow the line by going forwards  to align the entire buggy with it
//3) steadily go backwards, avoid drift away from the line to find the intersection

//// Brings the buggy back to line at the rotation movement --Horizontal alignment
void getBackToLineRotation() {
	bool back_to_line = false;
	while (back_to_line == false) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (Sensors[0].Boolian == Sensors[1].Boolian) {
			if ((Sensors[0].Boolian == starting_intersection[0])) {
				Serial1.println("I MUST ROTATE RIGHT_get_back_to_line");
				Kick(KickDirection::Right, KICK_MAGNITUDE);
			}
			else {
				Serial1.println("I MUST ROTATE LEFT");
				Kick(KickDirection::Left, KICK_MAGNITUDE);
			}
		}
		else {
			back_to_line = true;
		}
	}
}


//Aligns the middle sensors s2 and s3 with the line
void forwardAlignmentOnRotation() {
	bool s2s3aligned = false;
	while (s2s3aligned == false) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (Sensors[2].Boolian != Sensors[3].Boolian) {
			s2s3aligned = true;
			Serial1.println("Back On Track!");
		}
		else {
			if (LineCorrect() == false) {
				Kick(KickDirection::Forward, KICK_MAGNITUDE);
			}
		}
	}
}
//Kicks back until s2 and s3 are behind the node
void backwardsToIntersection() {
	bool s2s3aligned = false;
	while (s2s3aligned == false) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if ((Sensors[2].Boolian != Sensors[3].Boolian) && (Sensors[2].Boolian == starting_intersection[2])) {
			s2s3aligned = true;
			Serial1.print("Need to go forward to pass the node");
		}
		else {
			if (LineCorrect() == false) {
				Kick(KickDirection::Backward, KICK_MAGNITUDE);
			}
		}
	}
}
//Kick forward until s2 and s3 have passed the intersection
void forwardsToIntersection() {
	bool s2s3aligned = false;
	while (s2s3aligned == false) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if ((Sensors[2].Boolian != Sensors[3].Boolian) && (Sensors[2].Boolian != starting_intersection[2])) {
			s2s3aligned = true;
			Serial1.println("Finished Movement");
		}
		else {
			if (LineCorrect() == false) {
				Kick(KickDirection::Forward, KICK_MAGNITUDE);
			}
		}
	}
}

//Function to align the buggy upon rotation.
void smartAlignmentRotation() {
	bool alignmentComplete = false;
	while (alignmentComplete == false) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (reachedDestination() == true) {
			Serial1.println("FINISHED");
			alignmentComplete = true;
		}
		else {
			getBackToLineRotation();
			if (reachedDestination() == true) {
				return;
			}
			forwardAlignmentOnRotation();
			if (reachedDestination() == true) {
				return;
			}
			backwardsToIntersection();
			if (reachedDestination() == true) {
				return;
			}
			forwardsToIntersection();
			if (reachedDestination() == true) {
				return;
			}
		}
	}
}
//------BACKWARD ALIGNMENT---------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Function to flag sensor events during backwards alignment.
void sensorEventsB() {
	if (Sensors[4].Boolian != starting_intersection[4]) {
		sensor4_event = true;
		flag4 = true;
	}
	if (Sensors[5].Boolian != starting_intersection[5]) {
		sensor5_event = true;
		flag5 = true;
	}
}

//Function to define whether or not the buggy has passed an interesction backwards.
//It currently does a serial print to define whether or not it has.
//May be better to change this function to a bool type and then print to the terminal if the return is true/false.
//Also the the variable passed_intersection_lineb may then not need to be alive for as long?
void didIPassIntersectionLineB() {
	Sensor::PollSensors(Sensors, DefaultOrder, 6);
	//ideal-case Both sensors have passed the intersection line normally
	if ((Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5]) || ((sensor4_event == true) && (sensor5_event == true)))
	{
		passed_intersection_lineb = true;
		Serial1.println("Yes I passed the intersection line");
	}
	else {
		passed_intersection_lineb = false;
		Serial1.println("No I havent passed intersection line");
	}
}
// Function to identify the quadrant where the buggy is at the moment, when going backwards.
//Not sure that this really needs to be a seperate function to whereAmI, see slack suggestion for combined function.
void whereAmIB() {
	didIPassIntersectionLineB();
	if (Sensors[4].Boolian == Sensors[5].Boolian) {
		if (passed_intersection_lineb == true) {
			if (Sensors[4].Boolian != starting_intersection[4]) {
				Serial1.println("I am at Q4");
				quadrant = 4;
			}
			else {
				Serial1.println(" I am at Q1");
				quadrant = 1;
			}

		}
		else {
			if (Sensors[4].Boolian != starting_intersection[4]) {
				Serial1.println("I am at Q2");
				quadrant = 2;
			}
			else {
				Serial1.println("I am at Q3");
				quadrant = 3;
			}
		}
	}
	else {
		quadrant = 0; //S4 and S5 on line
	}
}
//Identifies whether sensors 4 and 5 have passed the node in the backward motion
void reachedNoteB() {
	bool passed_s45 = false;
	while (passed_s45 == false) {
		//Sensor::PollSensors(Sensors, Front, 2);
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (reachedDestination() == true) {
			return;
		}
		if ((Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5])) {
			passed_s45 = true;
		}
		else {
			Serial1.println("Need to kick steadily backward");
			if (LineCorrect() == false) {
				Kick(KickDirection::Backward, KICK_MAGNITUDE);
			}

		}
	}
}

//Funtion to figure out if passed the node backwards.
//If it has reached the node it will stop. If gone past it, it will kick forwards until reached.
void passedNoteB() {
	bool passed_s23 = false;
	while (passed_s23 == false) {
		//Sensor::PollSensors(Sensors, FrontM, 2);
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (reachedDestination() == true) {
			return;
		}
	}
	if ((Sensors[2].Boolian != starting_intersection[0]) && (Sensors[3].Boolian != starting_intersection[1])) {
		passed_s23 = true;
	}
	else {
		Serial1.println("Need to kick forward on reach_nodeb");
		if (LineCorrect() == false) {
			Kick(KickDirection::Forward, KICK_MAGNITUDE);
		}

	}
}

//Function to run the smart alignment when traveling backwards.
//Not currently being called by anything!
void SmartAlignmentB() {
	bool perfect_intersection = false;
	whereAmIB();
	while (perfect_intersection == false) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if ((Sensors[0].Boolian != starting_intersection[0]) && (Sensors[1].Boolian != starting_intersection[1]) && (Sensors[2].Boolian != starting_intersection[2]) &&
			(Sensors[3].Boolian != starting_intersection[3]) && (Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5])) {
			perfect_intersection = true;
			Serial1.println(" FINISHED");
		}
		else {
			findLineS23();
			if (reachedDestination() == true) {
				return;
			}
			findLineS01();
			if (reachedDestination() == true) {
				return;
			}
			reachedNoteB();
			if (reachedDestination() == true) {
				return;
			}
			passedNoteB();
			if (reachedDestination() == true) {
				return;
			}
		}
	}
}
//---------------------------------------------------------------------------

//Not currently being called:
//Not sure why, assuming this is a function to move backwards one node.
void moveBackwards() {
	digitalWrite(M1, LOW);
	digitalWrite(M2, LOW);
	analogWrite(E1, 70);
	analogWrite(E2, 70);
	while (true) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		sensorEventsB();
		didIPassIntersectionLineB();
		if (passed_intersection_line == true) {
			analogWrite(E1, 0);
			analogWrite(E2, 0);
			return;
		}
		else {
		}
	}
}

//Function to move forwards one node
void moveForward() {
	digitalWrite(M1, HIGH);
	digitalWrite(M2, HIGH);
	analogWrite(E1, 70);
	analogWrite(E2, 70);
	while (true) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		sensorEvents();
		didIPassIntersectionLine();
		if (passed_intersection_line == true) {
			analogWrite(E1, 0);
			analogWrite(E2, 0);
			return;
		}
		else {
		}
	}
}

//Function to turn left.
void TurnLeft() {
	Kick(KickDirection::Left, KICK_MAGNITUDE, (uint16_t)10);
	while (true) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		//Sensor::SelectSensor(1);
		//Sensors[1].GetReading();
		//Sensor::LogicCheck(Sensors);
		if (Sensors[1].Boolian != starting_intersection[1]) {
			analogWrite(E1, 0);
			analogWrite(E2, 0);
			return;
		}
		else {
			Kick(KickDirection::Left, KICK_MAGNITUDE, (uint16_t)10);
		}
	}
}

//Function to turn right.
void TurnRight() {
	Kick(KickDirection::Right, KICK_MAGNITUDE, (uint16_t)10);
	while (true) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (Sensors[0].Boolian != starting_intersection[0]) {
			analogWrite(E1, 0);
			analogWrite(E2, 0);
			return;
		}
		else {
			Kick(KickDirection::Right, KICK_MAGNITUDE, (uint16_t)10);
		}
	}
}
