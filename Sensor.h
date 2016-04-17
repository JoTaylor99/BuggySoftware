// sensor.h

#ifndef _SENSOR_H
#define _SENSOR_H


#include <SparkFunTSL2561.h>
#include "Config.h"
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include "Comms.h"

#ifdef QTRSINUSE
#include <QTRSensors.h>
#endif



class Sensor {

public:
	/* Sensor constructor*/
	Sensor(uint16_t Pin);
	Sensor(uint16_t Pin, sC::SensorType S);

	
	/* Sensor destructor*/
	~Sensor();
		
	static const sC::sensorNumber DefaultOrder[8];
	static const sC::sensorNumber Front[4];
	static const sC::sensorNumber Back[2];
	static const sC::sensorNumber Middle[2];
	static const sC::sensorNumber FrontNMiddle[6];
	
	static void initSensors();
	
	
	static void PollSensors(Sensor *sens, const sC::sensorNumber *order = Sensor::DefaultOrder, const byte OrderLength = NUM_SENSORS);

	#ifndef SENSOR_MEMORY_SAVE
		static void printCurrent(); 
	#else
		static void printCurrent(); 
	#endif

#ifdef SENSOR_MEMORY_SAVE
	private:
		static void setVal(sC::sensorNumber position, bool tileColour);
		static void setLastVal(sC::sensorNumber position, bool lastValue);
		static uint8_t lastValues;

	public:
		static bool valIs(sC::sensorNumber position);
		static bool lastValIs(sC::sensorNumber position);
		static uint8_t values;
#else
	public:
		static bool values[8];
		static bool lastValues[8];
#endif


private:
	bool tileWhite;
	uint16_t _pin;
	uint16_t Max;
	uint16_t Min;
	uint16_t Raw;
	double Normalised;
	sC::SensorType _s;
	
	
	bool GetReading();
	static void SelectSensor(byte sensorNumber);
	void ReadRaw(); //Reads the Raw value from whichever sensor is enabled
	void UpdateRange(); //Update's the sensor's range of values for correct scaling
	void Normalise(); //Normalise the sensor's reading on a scale between it's minimum and maximum
	void toTileColour(); //Checks the Normalised sensor readings against their thresholds
#ifndef SENSOR_MEMORY_SAVE
	static void printbw(bool *values); 
#else
	static void printbw(uint8_t values); 
#endif

	static bool _sensorInitComplete;  

};

//from changes 25/03
/*

enum DriftDirection : uint8_t {
DLeft, DRight, None, NotInTransition, Stop
};

static DriftDirection Drifting(Sensor *sens, bool lastCorrectLeft, bool lastCorrectRight);

static void LogicCheck(Sensor *sens);
*/









//from original INO
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

