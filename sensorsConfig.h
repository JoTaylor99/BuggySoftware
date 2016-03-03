#ifndef _SENSORS_H
#define _SENSORS_H
#include <NewPing.h>
#include <Wire.h>
#include "TSL2561.h"


//Definitions for sensors:
//Change these depending on what pins each sensor's address pin is connected to9
#define SENSOR1 8                                                                                 /*       _ _front _ _    */
#define SENSOR2 9                                                                                 /*       / s1 | s0 \ */
#define SENSOR3 10                                                                                 /*      |     |     |*/
#define SENSOR4 11                                                                                 /*      |_ s3_|_s2_ |*/
#define SENSOR5 12                                                                                 /*      |  s5 | s4  |*/
#define SENSOR6 13                                                                                /*       \_____|_____/*/
#define UPPER_THRESHOLD 50    //In percent
#define LOWER_THRESHOLD 30
#define TRIGGER A0
#define ECHO A1
#define MAXDISTANCE 100


#define LOGIC_THRESHOLD 400 // Minimum difference in Normalised value to detect opposite colour
#define NORMALISED_MAX 1000
#define MAX_DEFAULT 100
#define MIN_DEFAULT 100
#define MAX_SPEED 100
#define MIN_SPEED 65
#define TURNING_SPEED 60
#define CORRECTION_MAG 5
#define KICK_MAGNITUDE 255
#define DEFAULT_KICK_TIME 5
#define COUNTER_LEN 2

//Enumerator definitions for sensors:
typedef enum {
	RAW, BW
} OutputMode;


//Whether connecting via the Xbee of the USB connection.
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
	Forward, Backward, Left, Right
} KickDirection;

//Constant variables for sensors:
const int E1 = 5;     //M1 Speed Control
const int E2 = 6;     //M2 Speed Control
const int M1 = 4;    //M1 Direction Control           //MOTORS
const int M2 = 7;
const int M1Speed = 0;
const int M2Speed = 0;

//Other definitions:



#endif
