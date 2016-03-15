// sensor.h

#ifndef _SENSOR_H
#define _SENSOR_H

#include "Config.h"
#include <TSL2561.h>
#include <NewPing.h>
#include <QTRSensors.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include "Comms.h"

class Sensor {

public:
	/* Sensor constructor*/
	Sensor(uint16_t Pin);
	Sensor(uint16_t Pin, sensorConfig::SensorType S);

	
	/* Sensor destructor*/
	~Sensor();

	enum DriftDirection {
		DLeft, DRight, None, NotInTransition, Stop
	};

	double Normalised;
	bool Boolian;
	bool PreviousBool;
	static bool values[6];

	static const byte DefaultOrder[6];
	static const byte Front[2];
	static const byte Back[2];
	static const byte FrontM[2];
	static const byte FrontNMiddle[4];

	static void initSensors();
	
	static DriftDirection Drifting(Sensor *sens, bool lastCorrectLeft, bool lastCorrectRight);
	static void PollSensors(Sensor *sens, const byte *order, byte OrderLength);

private:
	uint16_t _pin;
	uint16_t Max;
	uint16_t Min;
	uint16_t Raw;
	sensorConfig::SensorType _s;
	

	bool GetReading();
	static void SelectSensor(uint8_t sensor_number);
	static void LogicCheck(Sensor *sens);
	void ReadRaw();
	void UpdateMaximum();
	void UpdateMinimum();
	void Normalise();
	void NormalToBool();
	void ThresholdCheck();
	static void printbw(bool *values);
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

