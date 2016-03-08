#ifndef _CONFIG_H
#define _CONFIG_H
#include <Arduino.h>
//Sensor Defines
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
#define FIVEVOLTOUTPUTPIN 1
#define ADCINPUTPIN	   2

#define BOXNUM		   4
#define BOXINV		   0

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






namespace boxConfig {

	enum boxSettings {
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

	namespace sensorConfig {
	
		enum SensorType {
			TSL, QTR
		};

	};

	namespace motorConfig {

		enum Direction {
			F, B, S
		};

	};
#endif