// sensor.h

#ifndef _SENSOR_H
#define _SENSOR_H


#include "TSL2561NR.h"
#ifdef QTRSINUSE
#include <QTRSensors.h>
#endif
#include "Config.h"
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include "Comms.h"

/*Sensor mode setup flow control:
*	Create one sensor object per sensor, passing the address select pin as an argument
*	Set all address pinModes as output
*	Write all address pins high
*	Declare 1 analog pin mode in and 1 analog pin mode out for ultrasonic
*
*
*
*
*
*
*
*
*
*
*
*
*
*
*
*/


class Sensor {

public:
	/* Sensor constructor*/
	Sensor(uint16_t Pin);
	Sensor(uint16_t Pin, sensorConfig::SensorType S);

	
	/* Sensor destructor*/
	~Sensor();

	typedef enum sensorNumber {
		junctRight = 0,
		junctLeft = 1,
		frontRight = 2,
		frontLeft = 3,
		middleRight = 4,
		middleLeft = 5,
		backRight = 6,
		backLeft = 7,
		invalid = 8
	};

	enum DriftDirection {
		DLeft, DRight, None, NotInTransition, Stop
	};

	double Normalised;
	bool Boolian;
	bool PreviousBool;
	static bool values[8];

	static const byte DefaultOrder[8];
	static const byte Front[2];
	static const byte Back[2];
	static const byte FrontM[2];
	static const byte FrontNMiddle[4];
	static const byte Junction[2];

	/*static const sensorNumber DefaultOrder[8];
	static const sensorNumber Front[2];
	static const sensorNumber Back[2];
	static const sensorNumber FrontM[2];
	static const sensorNumber FrontNMiddle[4];
	static const sensorNumber Junction[2];*/

	static void initSensors();
	
	static DriftDirection Drifting(Sensor *sens, bool lastCorrectLeft, bool lastCorrectRight);
	static void PollSensors(Sensor *sens, const byte *order, byte OrderLength);
	static void PollSensors(Sensor *sens);

private:
	uint16_t _pin;
	uint16_t Max;
	uint16_t Min;
	uint16_t Raw;
	sensorConfig::SensorType _s;
	

	bool GetReading();
	static void SelectSensor(uint8_t sensor_number);
	static void LogicCheck(Sensor *sens);
	void ReadRaw(); //Reads the Raw value from whichever sensor is enabled
	void UpdateMaximum(); //Update's the sensor's maximum value for correct scaling
	void UpdateMinimum(); //Update's the sensor's minimum value for correct scaling
	void Normalise(); //Normalise the sensor's reading on a scale between it's minimum and maximum
	void NormalToBool(); //Determins weither a detacted sensor reading is Black or White
	void ThresholdCheck(); //Checks the Normalised sensor readings against their thresholds
	static void printbw(bool *values); /*Being called by PollSensors & loop1
This handles printing of the boolean values of the sensor values (x6), to either the XBee or the USB
Prints boolean values, presently there is no material difference between the conditions,
so one has been commented out until such time as communication code is finalised.*/
	static bool _sensorInitComplete;  

};


/* UNUSED VARIABLES
*
* int TransitionOrder declared in global scope and not presently referenced
* int TransistionOrder[10] = { 1, 2, 1, 2, 1, 2, 3, 4, 5, 6 };
*
*int FrontNBack[4] declared in global scope and not presently referenced
*int FrontNBack[4] = { 1, 2, 5, 6 };
*
*int Middle[4] declared in global scope and not presently referenced
*int Middle[4] = { 3, 4, 5, 6 }; 
*
*bool logic[6] declared in global scope and not presently referenced
*bool logic[6] = { false, false, false, false, false, false };
*
*
*
*UNUSED FUNCTIONS
*
*public static function declared in scope Sensors but never called:
* 	//Checks if buggy is on an intersection
*	static bool OnIntersection(Sensor *sens);
*
*public static function declared in scope Sensors but never called:
*static bool OnInitialIntersection(Sensor *sens, bool *start);
*
*public static function declared in scope Sensors but never called:
*static bool OnLine(Sensor *sens);
*
*public static function  declared in scope Sensors but not defined or ever called:
*static bool CompareBools(Sensor * sens, bool *bools);
*
*public static function  declared in scope Sensors but not defined or ever called:
*static bool StartComparison(bool *start, Sensor *sens);
*
*public static function  declared in scope Sensors but not defined or ever called:
*static bool AnyDifferent(bool *start, Sensor *sens);
*
*public static function declared in scope Sensors but never called:
*static bool SensorsCrossedIntersectionCheck(Sensor *sens, bool *start);
*
*public static function declared in scope Sensors but never called:
*static void DiffCheck(Sensor *sens);
*/


#endif

