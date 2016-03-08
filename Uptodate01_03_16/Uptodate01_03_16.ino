

#include <QTRSensors.h>
#include <Adafruit_MCP23017.h>
#include <NewPing.h>
#include <Wire.h>
#include "TSL2561.h"

//Change these depending on what pins each sensor's address pin is connected to9
#define SENSOR1 8                                                                                 /*       _ _front _ _    */
#define SENSOR2 9                                                                              /*       / s1 | s0 \ */
#define SENSOR3 10                                                                                 /*      |     |     |*/
#define SENSOR4 11                                                                                 /*      |_ s3_|_s2_ |*/
#define SENSOR5 12                                                                                 /*      |  s5 | s4  |*/
#define SENSOR6 13                                                                                /*       \_____|_____/*/
#define UPPER_THRESHOLD 55     //In percent
#define LOWER_THRESHOLD 20
#define TRIGGER A0
#define ECHO A1
#define MAXDISTANCE 100

#define LOGIC_THRESHOLD 200 // Minimum difference in Normalised value to detect opposite colour
#define NORMALISED_MAX 1000
#define MAX_DEFAULT 100
#define MIN_DEFAULT 100
#define MAX_SPEED 100
#define MIN_SPEED 65
#define TURNING_SPEED 60
#define CORRECTION_MAG 5
#define KICK_MAGNITUDE 200
#define DEFAULT_KICK_TIME 15

#define COUNTER_LEN 2

typedef enum {
	Forward, Backward, Left, Right
} KickDirection;
int Counter = 0;
void Kick(KickDirection dir, int Magnitude);
void Kick(KickDirection dir, int Magnitude, uint16_t time);
void Kick(KickDirection dir, int LeftMagnitude, int RightMagnitude);
void Kick(KickDirection LeftDir, KickDirection RightDir, int LeftMag, int RightMag);
typedef enum {
	RAW, BW
} OutputMode;

typedef enum {
	Xbee, USB
} SerialMode;

typedef enum {
	F, B, S
} Direction;

typedef enum {
	Initial, Transistion, Destination
} State;

typedef enum {
	DLeft, DRight, None, NotInTransition, Stop
} DriftDirection;
typedef enum {
	TSL, QTR
}SensorType;

//CLASS----------------------------------------------------------
class Sensor {
public:
	Sensor(uint16_t Pin);
	Sensor(uint16_t Pin, SensorType S);
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
	static void DiffCheck(Sensor *sens);
private:
	uint16_t _pin;
	SensorType _s;
	void ReadRaw();
	void UpdateMaximum();
	void UpdateMinimum();
	void Normalise();
	void NormalToBool();
	void ThresholdCheck();
};
//---------------------------------------------------------------
OutputMode OMode = BW;
SerialMode SMode = Xbee;
TSL2561 tsl(TSL2561_ADDR_LOW);
Adafruit_MCP23017 GP;
QTRSensorsRC RC[2];
Sensor Sensors[7] = { SENSOR1 , SENSOR2, SENSOR3, SENSOR4, SENSOR5, SENSOR6, {(int)7,SensorType::QTR} };
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

int M1Speed = 0;
int M2Speed = 0;
// Motor 1
int dir1PinA = 4;
int dir2PinA = 5;
int speedPinA = 6; // Needs to be a PWM pin to be able to control motor speed

				   // Motor 2
int dir1PinB = 6;
int dir2PinB = 7;
int speedPinB = 5; // Needs to be a PWM pin to be able to control motor speed
int LeftSpeed = 70;
int RightSpeed = 70;
uint32_t lum = 0;
uint16_t ir = 0, full = 0;
bool LastCorrectLeft = false;
bool LastCorrectRight = false;
void MotorRamp(Direction Dir, int LMag, int RMag);
void DriftCorrect(DriftDirection Dir);
void MotorControl(Direction LDir, Direction RDir, int LSpeed, int RSpeed);
Sensor::Sensor(uint16_t Pin) {
	_pin = Pin;
	Max = MAX_DEFAULT;
	Min = MIN_DEFAULT;
	Normalised = 0;
	Boolian = false;
	PreviousBool = false;
	_s = SensorType::TSL;
}
Sensor::Sensor(uint16_t index, SensorType S) {
	_pin = index;
	Max = MAX_DEFAULT;
	Min = MIN_DEFAULT;
	Normalised = 0;
	Boolian = false;
	PreviousBool = false;
	_s = S;

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
	if (_s == SensorType::TSL) {
		tsl.begin();
		lum = tsl.getFullLuminosity();
		ir = lum >> 16;
		Raw = lum & 0xFFFF;
	}
	else {
		unsigned int *a;
		RC[_pin].readCalibrated(a);
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
	Serial.println("In SensorCheck");
	Serial.print(LeftTarget); Serial.print("\t"); Serial.println(RightTarget);
	Serial.print(sens[1].Boolian); Serial.print("\t"); Serial.println(sens[0].Boolian);
	if (sens[0].Boolian == RightTarget && sens[1].Boolian == LeftTarget && sens[0].Boolian != sens[1].Boolian) {
		Serial.println("In if");

		//passed_intersection_line = true;
		return true;
	}
	else {
		Serial.println("Else of if");
		// passed_intersection_line = false;
		return false;
	}
}

//Detects if the buggy is drifting from a line
DriftDirection Sensor::Drifting(Sensor *sens) {
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
				
			}

			return CurrentDrift;
		
		}
		else {
			return DriftDirection::None;
		}
	}
}

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
}
void Kick(KickDirection dir, int Magnitude) {
	if (dir == KickDirection::Forward) {
		MotorControl(Direction::F, Direction::F, Magnitude, Magnitude);
		delay(DEFAULT_KICK_TIME);
		MotorControl(Direction::S, Direction::S, Magnitude, Magnitude);
	}
	else if (dir == KickDirection::Backward) {
		MotorControl(Direction::B, Direction::B, Magnitude, Magnitude);

		delay(DEFAULT_KICK_TIME);
		MotorControl(Direction::S, Direction::S, Magnitude, Magnitude);

	}
	else if (dir == KickDirection::Left) {
		MotorControl(Direction::B, Direction::F, Magnitude, Magnitude);

		delay(DEFAULT_KICK_TIME);
		MotorControl(Direction::S, Direction::S, Magnitude, Magnitude);

	}
	else if (dir == KickDirection::Right) {
		MotorControl(Direction::F, Direction::B, Magnitude, Magnitude);

		delay(DEFAULT_KICK_TIME);
		MotorControl(Direction::S, Direction::S, Magnitude, Magnitude);

	}
	return;
}
void Kick(KickDirection dir, int Magnitude, uint16_t time) {
	if (dir == KickDirection::Forward) {
		MotorControl(Direction::F, Direction::F, Magnitude, Magnitude);

		delay(time);
		MotorControl(Direction::S, Direction::S, Magnitude, Magnitude);

	}
	else if (dir == KickDirection::Backward) {
		MotorControl(Direction::B, Direction::B, Magnitude, Magnitude);

		delay(time);
		MotorControl(Direction::S, Direction::S, Magnitude, Magnitude);

	}
	else if (dir == KickDirection::Left) {
		MotorControl(Direction::B, Direction::F, Magnitude, Magnitude);

		delay(time);
		MotorControl(Direction::S, Direction::S, Magnitude, Magnitude);

	}
	else if (dir == KickDirection::Right) {
		MotorControl(Direction::F, Direction::B, Magnitude, Magnitude);

		delay(time);
		MotorControl(Direction::S, Direction::S, Magnitude, Magnitude);

	}
	return;
}
void Kick(KickDirection dir, int LeftMagnitude, int RightMagnitude) {
	if (dir == KickDirection::Forward) {
		MotorControl(Direction::F, Direction::F, LeftMagnitude, RightMagnitude);

		delay(DEFAULT_KICK_TIME);
		MotorControl(Direction::S, Direction::S, LeftMagnitude, RightMagnitude);

	}
	else if (dir == KickDirection::Backward) {
		MotorControl(Direction::B, Direction::B, LeftMagnitude, RightMagnitude);
		delay(DEFAULT_KICK_TIME);
		//analogWrite(E1, 0);
		//digitalWrite(M1, HIGH);
		//analogWrite(E2, 0);
		//digitalWrite(M2, HIGH);
		MotorControl(Direction::S, Direction::S, LeftMagnitude, RightMagnitude);

	}
	else if (dir == KickDirection::Left) {
		MotorControl(Direction::B, Direction::F, LeftMagnitude, RightMagnitude);
		delay(DEFAULT_KICK_TIME);
		//analogWrite(E1, 0);
		//digitalWrite(M1, HIGH);
		//analogWrite(E2, 0);
		//digitalWrite(M2, HIGH);
		MotorControl(Direction::S, Direction::S, LeftMagnitude, RightMagnitude);

	}
	else if (dir == KickDirection::Right) {
		MotorControl(Direction::F, Direction::B, LeftMagnitude, RightMagnitude);
		delay(DEFAULT_KICK_TIME);
		//analogWrite(E1, 0);
		//digitalWrite(M1, HIGH);
		//analogWrite(E2, 0);
		//digitalWrite(M2, HIGH);
		MotorControl(Direction::S, Direction::S, LeftMagnitude, RightMagnitude);

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
//----------------------------------------------------
int IntersectionCount = 0;
uint16_t quadrant = 0;  //
						//Polls all given sensors in the order specified.
void Sensor::PollSensors(Sensor *sens, int *order, int OrderLength) {
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
void Sensor::DiffCheck(Sensor *sens) {

}
void MoveForward();
void TurnLeft();
void setup(void) {
	Serial.begin(115200); //Setup serial conenction over USB
	//Serial.begin(112500); // Setup serial connection over Xbee
						   //-------Setup Pins used for sensor select as outputs---------------
	GP.begin();      // use default address 0
	
	
	pinMode(SENSOR1, OUTPUT);
	pinMode(SENSOR2, OUTPUT);
	pinMode(SENSOR3, OUTPUT);
	pinMode(SENSOR4, OUTPUT);
	pinMode(SENSOR5, OUTPUT);
	pinMode(SENSOR6, OUTPUT);

	pinMode(A0, OUTPUT);
	pinMode(A1, INPUT);

	pinMode(speedPinA, OUTPUT);

	pinMode(speedPinB, OUTPUT);

	GP.pinMode(dir1PinA, OUTPUT);
	GP.pinMode(dir2PinA, OUTPUT);
	GP.pinMode(dir1PinB, OUTPUT);
	GP.pinMode(dir2PinB, OUTPUT);

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
	if (SMode == USB) {
		Serial.print("Setup Complete!");
	}
	else {
		Serial.print("Setup Complete!");
	}
}

void BoxApproach() {
	int Distance = 0;
	LeftSpeed = 70;
	RightSpeed = 70;
	MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
	while (true) {
		Distance = Ultrasonic.ping_cm();
		Serial.println(Distance);
		if (Distance > 3) {
			Serial.println("Approaching Box");
			Sensor::PollSensors(Sensors, Front, 2);
			if (Sensors[0].Boolian == Sensors[1].Boolian) {
				DriftDirection Dir = Sensor::Drifting(Sensors);
				if (Dir == DriftDirection::DRight) {
					Serial.println("Drifting Right");
					LeftSpeed -= CORRECTION_MAG;
					//RightSpeed += CORRECTION_MAG;
					MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
				}
				else if (Dir == DriftDirection::DLeft) {
					Serial.println("Drifting Left");
					//LeftSpeed += CORRECTION_MAG;
					RightSpeed -= CORRECTION_MAG;
					MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
				}
			}
		}
		else {
			Serial.println("I Have Reached the box");
			MotorControl(Direction::S, Direction::S, 0, 0);
			if (Distance == 0) {
				Serial.println("Either too far or too close");
			}
			return;
		}
		delay(50);
	}
}

void loop(void) {
	/*
	for (int i = 0; i < 6; i++) {
	Sensor::SelectSensor(i);
	values[i] = Sensors[i].GetReading();
	//Serial.println(Sensors[i].Normalised);
	}*/
	//int DefaultOrder[6] = { 1,2,3,4,5,6 };
	Sensor::PollSensors(Sensors, DefaultOrder, 6);
	
	//printbw(values);
	//sensor_events();
	//Serial.println();

	//Serial.println("---------------------");
	//delay(50);

	if (Serial.available() > 0) { // If a command has been sent
		String str = Serial.readString();
		if (str == "F") {
			sensor0_event = false;
			sensor1_event = false;
			flag0 = false;
			flag1 = false;
			start();
			MoveForward();
			smartAlignmentForward();
		}
		if (str == "B") {
			sensor4_event = false;
		    sensor5_event = false;
			flag4 = false;
			flag5 = false;
			start();
			MoveBackward();
			SmartAlignmentB();
		}
		if (str == "R") {
			start();
			//RotateR();
			TurnRight();
			//smartAlignmentRotation();
		}
		if (str == "L") {
			start();
			//RotateL();
			TurnLeft();
			//smartAlignmentRotation();
		}
		if (str == "S") {

		}

		/*if (str == "D") {
			where_am_i();
			sensor0_event = false;
			sensor1_event = false;
			sensor4_event = false;
			sensor5_event = false;
			bool flag0 = false;
			bool flag1 = false;
			bool flag4 = false;
			bool flag5 = false;
		}
		if (str == "A") {
			smart_alignment();
		}*/
		if (str == "G") {
			BoxApproach();
		}
	}
}
//Bool function to indicate if the destination intersection has reached. True if the middle sensors are in the intersection or if perfect intersection achieved

//Detects if you are drifting while kicking and corrects it. Returns false
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
//Based on the quadrant the buggy rotates in a backward motion to get back to the motion line with sesnors 4 and 5
bool reachedDestination() {
	if ((Sensors[0].Boolian != starting_intersection[0]) && (Sensors[1].Boolian != starting_intersection[1]) && (Sensors[2].Boolian != starting_intersection[2]) &&
		(Sensors[3].Boolian != starting_intersection[3]) && (Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5])) {
		Serial.println("Destination Reached");
		return true;
	}
	//  else if ((Sensors[2].Boolian != starting_intersection[2]) && (Sensors[3].Boolian != starting_intersection[3])
	//           && (Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5])) {
	//    Serial.println("Destination Reached2");
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
	Serial.println("I have the starting POSITION");
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
		Serial.println("Yes I passed the intersection line");
	}
	else {
		passed_intersection_line = false;
		Serial.println("No I havent passed intersection line");
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
				Serial.println("I am at Q4");
				quadrant = 4;
			}
			else {
				Serial.println(" I am at Q1");
				quadrant = 1;
			}
		}
		else {
			if (Sensors[0].Boolian != starting_intersection[0]) {
				Serial.println("I am at Q2");
				quadrant = 2;
			}
			else {
				Serial.println("I am at Q3");
				quadrant = 3;
			}
		}
	}
	else {
		quadrant = 0; //S0 and S1 on line
	}
}
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
			Serial.println("Q1 NEED TO KICK BACKWARDS RIGHT");
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
			Serial.println("Q2 NEED TO KICK BACKWARDS RIGHT");
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
			Serial.println("Q3 NEED TO KICK BACKWARDS LEFT");
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
			Serial.println("Q4 NEED TO KICK BACKWARDS LEFT");
			Kick(KickDirection::Forward, KickDirection::Backward, 0, KICK_MAGNITUDE);

		}
	}
			 break;
	default:
		break;
	}
}
// Based on the quadrant the buggy rotates left or right to get back to the motion line with sesnors 1 and 2
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
			Serial.println("Q1 NEED TO ROTATE RIGHT");
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
			Serial.println("Q2 NEED TO ROTATE RIGHT");
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
			Serial.println("Q3 NEED TO ROTATE LEFT");
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
			Serial.println("Q4 NEED TO ROTATE LEFT");
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
			Serial.println("Need to kick forward");
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
			Serial.println("Need to kick forward on reach_node");
			if (LineCorrect() == false) {
				Kick(KickDirection::Forward, KICK_MAGNITUDE);
			}
		}
	}
}
void smartAlignmentForward() {
	bool perfect_intersection = false;
	whereAmI();
	while (perfect_intersection == false) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if ((Sensors[0].Boolian != starting_intersection[0]) && (Sensors[1].Boolian != starting_intersection[1]) && (Sensors[2].Boolian != starting_intersection[2]) &&
			(Sensors[3].Boolian != starting_intersection[3]) && (Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5])) {
			perfect_intersection = true;
			Serial.println(" FINISHED");
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
				Serial.println("I MUST ROTATE RIGHT_get_back_to_line");
				Kick(KickDirection::Right, KICK_MAGNITUDE);
			}
			else {
				Serial.println("I MUST ROTATE LEFT");
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
			Serial.println("Back On Track!");
		}
		else {
			if (LineCorrect() == false) {
				Kick(KickDirection::Forward, KICK_MAGNITUDE);
			}
		}
	}
}
//Kick back until s2s3 are behind the note
void backwardsToIntersection() {
	bool s2s3aligned = false;
	while (s2s3aligned == false) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if ((Sensors[2].Boolian != Sensors[3].Boolian) && (Sensors[2].Boolian == starting_intersection[2])) {
			s2s3aligned = true;
			Serial.print("Need to go forward to pass the node");
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
			Serial.println("Finished Movement");
		}
		else {
			if (LineCorrect() == false) {
				Kick(KickDirection::Forward, KICK_MAGNITUDE);
			}
		}
	}
}
void smartAlignmentRotation() {
	bool alignmentComplete = false;
	while (alignmentComplete == false) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (reachedDestination() == true) {
			Serial.println("FINISHED");
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
void didIPassIntersectionLineB() {
	Sensor::PollSensors(Sensors, DefaultOrder, 6);
	//ideal-case Both sensors have passed the intersection line normally
	if ((Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5]) || ((sensor4_event == true) && (sensor5_event == true)))
	{
		passed_intersection_lineb = true;
		Serial.println("Yes I passed the intersection line");
	}
	else {
		passed_intersection_lineb = false;
		Serial.println("No I havent passed intersection line");
	}
}
void whereAmIB() {
	didIPassIntersectionLineB();
	if (Sensors[4].Boolian == Sensors[5].Boolian) {
		if (passed_intersection_lineb == true) {
			if (Sensors[4].Boolian != starting_intersection[4]) {
				Serial.println("I am at Q4");
				quadrant = 4;
			}
			else {
				Serial.println(" I am at Q1");
				quadrant = 1;
			}

		}
		else {
			if (Sensors[4].Boolian != starting_intersection[4]) {
				Serial.println("I am at Q2");
				quadrant = 2;
			}
			else {
				Serial.println("I am at Q3");
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
			Serial.println("Need to kick steadily backward");
			if (LineCorrect() == false) {
				Kick(KickDirection::Backward, KICK_MAGNITUDE);
			}

		}
	}
}
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
		Serial.println("Need to kick forward on reach_nodeb");
		if (LineCorrect() == false) {
			Kick(KickDirection::Forward, KICK_MAGNITUDE);
		}

	}
}

void SmartAlignmentB() {
	bool perfect_intersection = false;
	whereAmIB();
	while (perfect_intersection == false) {
		Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if ((Sensors[0].Boolian != starting_intersection[0]) && (Sensors[1].Boolian != starting_intersection[1]) && (Sensors[2].Boolian != starting_intersection[2]) &&
			(Sensors[3].Boolian != starting_intersection[3]) && (Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5])) {
			perfect_intersection = true;
			Serial.println(" FINISHED");
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
void TurnLeft() {
	Sensor::PollSensors(Sensors, DefaultOrder, 6);
	MotorControl(Direction::B, Direction::F, 70, 70);

	//digitalWrite(M1, HIGH);
	//digitalWrite(M2, LOW);
	//analogWrite(E1, 65);
	//analogWrite(E2, 65);
	while (true) {
		Sensor::PollSensors(Sensors, Front, 2);

		//Sensor::SelectSensor(1);
		//Sensors[1].GetReading();
		//Serial.println(Sensors[1].Boolian);
		//Sensor::LogicCheck(Sensors);
		if (Sensors[1].Boolian != Sensors[1].PreviousBool) {
			MotorControl(Direction::S, Direction::S, 0, 0);

			//analogWrite(E1, 0);
			//analogWrite(E2, 0);
			return;
		}
		else {
		}
	}
}
void TurnRight() {
	//digitalWrite(M1, LOW);
	//digitalWrite(M2, HIGH);
	//analogWrite(E1, 65);
	//analogWrite(E2, 65);
	Sensor::PollSensors(Sensors, DefaultOrder, 6);
	MotorControl(Direction::F, Direction::B, 75, 75);
	while (true) {
		Sensor::PollSensors(Sensors, Front, 2);
		if (Sensors[0].Boolian != starting_intersection[0]) {
			MotorControl(Direction::S, Direction::S, 0, 0);

			//analogWrite(E1, 0);
			//analogWrite(E2, 0);
			return;
		}
		else {
		}
	}
}

void MoveForward() {
	Serial.println("Moving Now!");
	int RCnt = 0;
	int LCnt = 0;
	//int cnt = 0;
	//LeftSpeed = 100;
	//RightSpeed = 100;
	//MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
	//delay(30);
	//LeftSpeed = 60;
	//RightSpeed = 60;
	//MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
	while (true) {
		Sensor::PollSensors(Sensors, FrontNMiddle,4);
		sensorEvents();
		if ((Sensors[2].Boolian != starting_intersection[2] && Sensors[3].Boolian != starting_intersection[3])
			)
		{
			//Don't kick
			Serial.println("I have stopped");
			MotorControl(Direction::S, Direction::S, 0, 0);
			return;
		}
		else {
			if (Sensors[0].Boolian != Sensors[1].Boolian) {
				//On line
				LastCorrectLeft = Sensors[1].Boolian;
				LastCorrectRight = Sensors[0].Boolian;
				Kick(KickDirection::Forward, 200);//Kick forward but with a long kick
				RCnt = 0;
				LCnt = 0;

			}
			//LeftSpeed++;
			//RightSpeed++;
		}
		if (Sensors[0].Boolian == Sensors[1].Boolian) {
			if (Sensors[2].Boolian == Sensors[3].Boolian) {
				//Following wrong line case
				//LastCorrectLeft = !LastCorrectLeft;
				//LastCorrectRight = !LastCorrectRight;


			}
			if (RCnt > 20 || LCnt > 20) {
				RCnt = 0;
				LCnt = 0;
				//Kick(KickDirection::Forward, 200);//Kick forward but with a long kick
												  LastCorrectLeft = !starting_intersection[1];
												  LastCorrectRight = !starting_intersection[0];
			}
			else {


				DriftDirection Dir = Sensor::Drifting(Sensors);

				if (Dir == DriftDirection::DRight) {
					RCnt++;
					Serial.println("Drifting Right");
					LeftSpeed -= CORRECTION_MAG;
					RightSpeed += CORRECTION_MAG;
					//MotorControl(Direction::F, Direction::F, LeftSpeed,RightSpeed);
					Kick(KickDirection::Backward, KickDirection::Forward, 200, 200);//Kick forward but with a long kick

				}
				else if (Dir == DriftDirection::DLeft) {
					LCnt++;
					Serial.println("Drifting Left");
					LeftSpeed += CORRECTION_MAG;
					RightSpeed -= CORRECTION_MAG;
					//MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
					Kick(KickDirection::Forward, KickDirection::Backward, 200, 200);//Kick forward but with a long kick

				}
			}
		}
		else {

		}
	}

}
void MoveBackward() {
	int RCnt = 0;
	int LCnt = 0;
	//int cnt = 0;
	//LeftSpeed = 100;
	//RightSpeed = 100;
	//MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
	//delay(30);
	//LeftSpeed = 60;
	//RightSpeed = 60;
	//MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
	while (true) {
		Sensor::PollSensors(Sensors, FrontNMiddle, 4);
		sensorEventsB();
		if ((Sensors[2].Boolian != starting_intersection[2] && Sensors[3].Boolian != starting_intersection[3])
			)
		{
			//Don't kick
			Serial.println("I have stopped");
			MotorControl(Direction::S, Direction::S, 0, 0);
			return;
		}
		else {
			if (Sensors[0].Boolian != Sensors[1].Boolian) {
				//On line
				LastCorrectLeft = Sensors[1].Boolian;
				LastCorrectRight = Sensors[0].Boolian;
				Kick(KickDirection::Backward, 200);//Kick Backward but with a long kick
				RCnt = 0;
				LCnt = 0;

			}
			//LeftSpeed++;
			//RightSpeed++;
		}
		if (Sensors[0].Boolian == Sensors[1].Boolian) {
			if (Sensors[2].Boolian == Sensors[3].Boolian) {
				//Following wrong line case
				//LastCorrectLeft = !LastCorrectLeft;
				//LastCorrectRight = !LastCorrectRight;


			}
			if (RCnt > 20 || LCnt > 20) {
				RCnt = 0;
				LCnt = 0;
				//Kick(KickDirection::Backward, 200);//Kick Backward but with a long kick
				LastCorrectLeft = !starting_intersection[1];
				LastCorrectRight = !starting_intersection[0];
			}
			else {


				DriftDirection Dir = Sensor::Drifting(Sensors);

				if (Dir == DriftDirection::DRight) {
					RCnt++;
					Serial.println("Drifting Right");
					LeftSpeed -= CORRECTION_MAG;
					RightSpeed += CORRECTION_MAG;
					//MotorControl(Direction::F, Direction::F, LeftSpeed,RightSpeed);
					Kick(KickDirection::Forward, KickDirection::Backward, 200, 200);//Kick Backward but with a long kick

				}
				else if (Dir == DriftDirection::DLeft) {
					LCnt++;
					Serial.println("Drifting Left");
					LeftSpeed += CORRECTION_MAG;
					RightSpeed -= CORRECTION_MAG;
					//MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
					Kick(KickDirection::Backward, KickDirection::Forward, 200, 200);//Kick Backward but with a long kick

				}
			}
		}
		else {

		}
	}

}

void MotorControl(Direction LDir, Direction RDir, int LSpeed, int RSpeed) {
	if (LSpeed < 0) {
		LSpeed = 0;
	}
	else if (LSpeed >255) {
		LSpeed = 255;
	}
	if (RSpeed < 0) {
		RSpeed = 0;
	}
	else if (RSpeed >255) {
		RSpeed = 255;
	}
	if (LDir == Direction::F) {
		
		GP.digitalWrite(dir1PinB, LOW);
		GP.digitalWrite(dir2PinB, HIGH);
		analogWrite(speedPinB, LSpeed);

	}
	else if (LDir == Direction::B) {
		
		GP.digitalWrite(dir1PinB, HIGH);
		GP.digitalWrite(dir2PinB, LOW);
		analogWrite(speedPinB, LSpeed);

	}
	else {
		GP.digitalWrite(dir1PinB, LOW);
		GP.digitalWrite(dir2PinB, LOW);
		analogWrite(speedPinB, 0);
	}


	if (RDir == Direction::F) {
		GP.digitalWrite(dir1PinA, HIGH);
		GP.digitalWrite(dir2PinA, LOW);
		analogWrite(speedPinA, RSpeed);
	}
	else if (RDir == Direction::B) {
		GP.digitalWrite(dir1PinA, LOW);
		GP.digitalWrite(dir2PinA, HIGH);
		analogWrite(speedPinA, RSpeed);
	}
	else {
		GP.digitalWrite(dir1PinA, LOW);
		GP.digitalWrite(dir2PinA, LOW);
		analogWrite(speedPinA, 0);

	}

}
