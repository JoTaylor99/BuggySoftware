#ifndef _CONFIG_H
#define _CONFIG_H
#include <Arduino.h>
#include <avr/io.h>
#include <avr/wdt.h>
#define Reset_AVR() wdt_enable(WDTO_30MS); while(1) {}
//Toggle this statement to enable global debug statements, for example "Setup complete" in buggyTop
//Note this can also be used to do things such as test timings while leaving more in depth debug prints disabled.
//#define DEBUG

//#define SENSOR_DEBUG
//#define NAV_DEBUG
//#define MOT_DEBUG
//#define BOX_DEBUG

//#define SEN_INFO
//#define NAV_INFO
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

//#define DISPLAYERRORS

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
#ifndef SENSOR_MEMORY_SAVE
#define SVAL(sensorNumber, value) Sensor::values[sensorNumber] = value
#define RVAL(sensorNumber)	Sensor::values[sensorNumber]
#define STARTVAL(sensorNumber)	navigation::starting_values[sensorNumber]
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

#define NUM_SENSORS 8

//Change these depending on what pins each sensor's address pin is connected to
#define FRPIN 7                                                                                 /*       _ _front _ _    */
#define LTRPIN 6                                                                              /*       / s1 | s0 \ */
#define LTLPIN 5                                                                                 /*      |     |     |*/
#define FLPIN 4                                                                                 /*      |_ s3_|_s2_ |*/
#define MRPIN 3                                                                                 /*      |  s5 | s4  |*/
#define MLPIN 2                                                                                /*       \_____|_____/*/
#define BRPIN 1
#define BLPIN 0

#define UPPER_THRESHOLD 55     //In percent
#define LOWER_THRESHOLD 20
#define TRIGGER A0
#define ECHO A1
#define MAXDISTANCE 100

#define SENSOR1INTPIN 8                                                                                 /*       _ _front _ _    */
#define SENSOR2INTPIN 9                                                                              /*       / s1 | s0 \ */
#define SENSOR3INTPIN 10                                                                                 /*      |     |     |*/
#define SENSOR4INTPIN 11                                                                                 /*      |_ s3_|_s2_ |*/
#define SENSOR5INTPIN 12                                                                                 /*      |  s5 | s4  |*/
#define SENSOR6INTPIN 13
#define SENSOR7INTPIN 14
#define SENSOR8INTPIN 15

#define LOGIC_THRESHOLD 200 // Minimum difference in Normalised value to detect opposite colour
#define NORMALISED_MAX 1000
#define MAX_DEFAULT 100
#define MIN_DEFAULT 100

//Sensor interrupt thesholds

#define BLACKLOWTHRESHOLDLOWBYTE	(0x00)
#define BLACKLOWTHRESHOLDHIGHBYTE	(0x00)
#define BLACKHIGHTHRESHOLDLOWBYTE	(0xF4)
#define BLACKHIGHTHRESHOLDHIGHBYTE	(0x01)
#define WHITELOWTHRESHOLDLOWBYTE	(0xF4)
#define WHITELOWTHRESHOLDHIGHBYTE	(0x01)
#define WHITEHIGHTHRESHOLDLOWBYTE	(0xBA)
#define WHITEHIGHTHRESHOLDHIGHBYTE	(0x13)

//Motor Defines
#define DIR1PINA 4
#define DIR2PINA 5
#define SPEEDPINA 6
#define DIR1PINB 6
#define DIR2PINB 7
#define SPEEDPINB 5

#define MAX_SPEED 100
#define MIN_SPEED 65
#define TURNING_SPEED 60
#define CORRECTION_MAG 5
#define KICK_MAGNITUDE 200
#define DEFAULT_KICK_TIME 15

//Nav::navigation defines
#define COUNTER_LEN 2

//Box Defines
#define FIVEVOLTOUTPUTPIN 2
#define ADCINPUTPIN	   16

#define BOXNUM		   4
#define BOXINV		   0

#define P1BC		0x2000
#define P15V        0x1000
#define P1ADC       0x800
#define P2GND       0x400
#define P25V        0x200
#define P2Rd        0x100
#define P2Rk        0x80
#define P2ADC       0x40
#define GNDADC		0x20
#define GNDGND      0x10
#define GND5V       0x08
#define P1P2SWITCH  0x04
#define BC          0x02    //both ADCswitch and BC to be put on 1 pin in next circuit rev
#define ADCSWITCH   0x01

#define P1BCPIN        3
#define P15VPIN        8
#define P1ADCPIN       9
#define P2GNDPIN       4
#define P25VPIN        2
#define P2RdPIN        1
#define P2RkPIN        0
#define P2ADCPIN       13
#define GNDADCPIN     10
#define GNDGNDPIN      11
#define GND5VPIN       12
#define P1P2SWITCHPIN  7
#define BCPIN          5    //both ADCswitch and BC to be put on 1 pin in next circuit rev
#define ADCSWITCHPIN   14

#define NUMADCREADINGS 50

//change to bC
namespace boxConfig {
	enum boxSettings : uint16_t {
		box1 = 1,
		box2a = 2,
		box2b = 3,
		box3a = 4,
		box3b = 5,
		box4a = 6,
		box4b = 7,
		box5a = 8,
		box5b = 9,
		box6a = 10,
		box6b = 11,
		box7a = 12,
		box7b = 13
	};

	//temporary variables to be removed when comms between buggy and PC are finalised.
	const byte boxNumber = BOXNUM;
	const bool boxInverted = BOXINV;
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

namespace mC {
	enum Direction : uint8_t {
		F, B, S
	};
};

namespace nC {
	enum Direction : uint8_t {
		Forward, Backwards, Left, Right
	};

	enum Drift : uint8_t {
		leftDrift, rightDrift
	};
};

#endif