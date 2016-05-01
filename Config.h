#ifndef _CONFIG_H
#define _CONFIG_H
#include <Arduino.h>
#include <avr/io.h>
#include <avr/wdt.h>

/// <summary>
/// Arduino Watchdog timer reset.  This macro will result in an emergency stop of the buggy and reset of the microcontroller 15 millseconds after it is called.
/// </summary>
#define RESET_AVR2() wdt_enable(WDTO_15MS); while(1) {}

//Toggle this statement to enable global debug statements, for example "Setup complete" in buggyTop
//Note this can also be used to do things such as test timings while leaving more in depth debug prints disabled.
#define DEBUG

//#define SENSOR_DEBUG
//#define NAV_DEBUG
//#define MOT_DEBUG
//#define BOX_DEBUG

//#define SEN_INFO
#define NAV_INFO
//#define MOT_INFO
//#define BOX_INFO

/// <summary>
/// Conditional debug defines
/// When writing debug prints without variables (constant strings such as "Setup Complete" or "Sensor 1 value ="  then use DEBUG_PRINT or DEBUG_PRINTLN
/// For any debug print of a variable (any time you are not using double quotes) then instead use DEBUG_VPRINT or DEBUG_VPRINTLN
/// </summary>
#ifdef DEBUG
#define DEBUG_PRINTLN(x)  Serial.println (F(x))
#define DEBUG_PRINT(x)	Serial.print(F(x))
#define DEBUG_VPRINT(x)	Serial.print(x)
#define DEBUG_VPRINTLN(x)	Serial.println(x)
#define COMPILE_DATE Serial.println(__DATE__)
#define COMPILE_TIME Serial.println(__TIME__)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#define DEBUG_VPRINT(x)
#define DEBUG_VPRINTLN(x)
#define COMPILE_DATE
#define COMPILE_TIME
#endif

#define DISPLAYERRORS

#ifdef DISPLAYERRORS
#define ERROR_PRINTLN(x)  Serial.println (F(x))
#define ERROR_PRINT(x)	Serial.print(F(x))
#else
#define ERROR_PRINTLN(x)
#define ERROR_PRINT(x)
#endif



#ifdef SEN_INFO
#define SEN_PRINTLN(x)  Serial.println (F(x))
#define SEN_PRINT(x)	Serial.print(F(x))
#define SEN_VPRINT(x)	Serial.print(x)
#define SEN_VPRINTLN(x)	Serial.println(x)
#else
#define SEN_PRINTLN(x)
#define SEN_PRINT(x)
#define SEN_VPRINT(x)
#define SEN_VPRINTLN(x)
#endif

#ifdef NAV_INFO
#define NAV_PRINTLN(x)  Serial.println (F(x))
#define NAV_PRINT(x)	Serial.print(F(x))
#define NAV_VPRINT(x)	Serial.print(x)
#define NAV_VPRINTLN(x)	Serial.println(x)
#else
#define NAV_PRINTLN(x)
#define NAV_PRINT(x)
#define NAV_VPRINT(x)
#define NAV_VPRINTLN(x)
#endif

#ifdef MOT_INFO
#define MOT_PRINTLN(x)  Serial.println (F(x))
#define MOT_PRINT(x)	Serial.print(F(x))
#define MOT_VPRINT(x)	Serial.print(x)
#define MOT_VPRINTLN(x)	Serial.println(x)
#else
#define MOT_PRINTLN(x)
#define MOT_PRINT(x)
#define MOT_VPRINT(x)
#define MOT_VPRINTLN(x)
#endif

#ifdef BOX_INFO
#define BOX_PRINTLN(x)  Serial.println (F(x))
#define BOX_PRINT(x)	Serial.print(F(x))
#define BOX_VPRINT(x)	Serial.print(x)
#define BOX_VPRINTLN(x)	Serial.println(x)
#else
#define BOX_PRINTLN(x)
#define BOX_PRINT(x)
#define BOX_VPRINT(x)
#define BOX_VPRINTLN(x)
#endif


//#define QTRSINUSE
//#define SENSOR_MEMORY_SAVE

/// <summary>
/// Preprocessor switch to toggle sensor value storage between an array of boolean elements (each of length 8 bits) or storage in a single byte.
/// </summary>
#ifndef SENSOR_MEMORY_SAVE
#define SVAL(sensorNumber, value) Sensor::values[sensorNumber] = value
#define RVAL(sensorNumber)	Sensor::values[sensorNumber]
#define STARTVAL(sensorNumber)	navigation::startingValues[sensorNumber]
#define RLASTVAL(sensorNumber)	Sensor::lastValues[sensorNumber]
#define SLASTVAL(sensorNumber, lastValue)	Sensor::lastValues[sensorNumber] = lastValue
#else
#define SVAL(sensorNumber, tileColour) Sensor::setVal(sensorNumber, tileColour)
#define RVAL(sensorNumber)	(Sensor::valIs(sensorNumber))
#define STARTVAL(sensorNumber)	(navigation::startValIs(sensorNumber))
#define RLASTVAL(sensorNumber)	(Sensor::lastValIs(sensorNumber))
#define SLASTVAL(sensorNumber, lastValue)	Sensor::setLastVal(sensorNumber, lastValue)
#endif
//Sensor Defines

/// <summary>
/// Number of sensors in use
/// </summary>
#define NUM_SENSORS 8

/// <summary>
/// Sensor pins
/// </summary>
//Change these depending on what pins each sensor's address pin is connected to
#define FRPIN 7
#define LTRPIN 6
#define LTLPIN 5
#define FLPIN 4
#define MRPIN 3
#define MLPIN 2
#define BRPIN 1
#define BLPIN 0

/// <summary>
/// Sensor thresholds, maximum normalised range
/// </summary>
#define UPPER_THRESHOLD 55     //In percent
#define LOWER_THRESHOLD 20

#define NORMALISED_MAX 1000

/// <summary>
/// Default raw values for use in normalisation of first set of readings after power on
/// </summary>
#define MAX_DEFAULT 1000
#define MIN_DEFAULT 800

/// <summary>
/// Motor control pins
/// </summary>
#define LEFTMOTORDIR 6
#define RIGHTMOTORDIR 7
#define LEFTMOTOR 9
#define RIGHTMOTOR 3
#define LEFTMOTORCOUNT	8		//PCINT 0			
#define RIGHTMOTORCOUNT	4		//PCINT 20

/// <summary>
/// Stepwise motion adjustment factor to account for occasional missed step and slippage
/// </summary>
#define STEPWISESCALEFACTOR 1.035

//Setting to toggle blocking mode for stepwise operations
#define STEPWISE_BLOCKING

//#define HARDCODEDSTARTVALUES		//NOT USING THIS
//Box Defines


/// <summary>
/// Box analysis constants
/// </summary>
#define NUMADCREADINGS 50
#define BOXONERATIO		0.4528
#define VREF			5
#define ADCMAX			1023

/// <summary>
/// Box analysis known resistors
/// </summary>
#define RK	560
#define Rk7 99.8
//Pullup resistance on P2PIN will vary depending on board.
//Calibrate this with known capacitor.
#define RPULLUP 36.486  //in ohms

/// <summary>
/// Box analyis pins
/// </summary>
#define P1PIN	15 //Pin A1
#define P2PIN	16 //Pin A2
#define GNDPIN	17 //Pin A3
#define RKPIN	14 //Pin A0
#define RK7pin  12

/// <summary>
/// Box analysis GPIO expander pins
/// </summary>
#define P1P2RELAYPIN	0
#define BCPIN			1
#define BCRELAYPIN		2
#define BC_DPDT         3

//change to bC
namespace bC {

	enum inputStatus : uint8_t {
		OFF,
		ON
	};

	enum pinSettings : uint8_t {
		input_adc,
		input,
		high,
		low,
		nop
	};
}

//change to sC
	namespace sC {
		
		#ifdef QTRSINUSE
		enum SensorType : unint8_t {
			TSL, QTR
		};
		#else
		enum SensorType : uint8_t {
			TSL
		};
		#endif		

		typedef enum sensorNumber : uint8_t {
			FR,
			LTR,
			LTL,
			FL,
			MR,
			ML,
			BR,
			BL,
			invalid
		};

	};

	namespace nC{
	
		enum Direction : uint8_t {
			Forward, Backwards, Left, Right, Stop, RightForwardOnly, RightBackwardsOnly,LeftForwardOnly, LeftBackwardsOnly
		};

		enum Drift : uint8_t {
			leftDrift, rightDrift , noDrift
		};
	
	};

#endif