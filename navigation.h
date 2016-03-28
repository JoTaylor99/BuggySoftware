#ifndef _NAVIGATION_H
#define _NAVIGATION_H

#include "Config.h"
#include "buggyMotion.h"
#include "Sensor.h"
#include "box.h"
#include <NewPing.h>


class navigation : private buggyMotion {

	//Test commit changes.
public:
	/* Navigation constructor*/
	navigation();

	/* Navigation destructor*/
	~navigation();

	/* Navigation init function
	*  Void return type
	*  Will depends on communications class for sending error codes or debug statements
	*  Sets up
	*/
	void initNavigation();

	/* navigate function
	* return type is byte
	*
	*/
	void navigate(String str);

#ifdef QTRSINUSE
	Sensor Sensors[9] = { FRPIN, LTRPIN, LTLPIN, FLPIN, MRPIN, MLPIN, BRPIN, BLPIN, { (unsigned int)7, sensorConfig::QTR } };
#else
	Sensor Sensors[8] = { FRPIN, LTRPIN, LTLPIN, FLPIN, MRPIN, MLPIN, BRPIN, BLPIN };
#endif
	
private:
	
#ifdef SENSOR_MEMORY_SAVE
	bool startValIs(sC::sensorNumber position);
	uint8_t startingValues = 0;
#else
	bool startingValues[NUM_SENSORS] = {};
#endif

	static bool _navInitComplete;

	void boxApproach();

	void start(); //Captures and stores in an array all the sensor values at the initial node position

	void turnLeft();
	void turnRight();
	void moveForward();
	void moveBackward();
	
};

#endif

// Functions removed 25/03/16
/*void sensorEvents(); //Checks if there was an event in the values of the first two sensors during the forward motion
bool didIPassIntersectionLine(Direction Dir); //Checks if the buggy has passed the line of the destination intersection
											  //This function has been optimised by having it return the passed_line variable as a bool.
											  //Also, the backwards version has been merged with the forwards version.
uint8_t whereAmI(Direction Dir); // Function to identify the quadrant where the buggy is at the moment
								 //This function has been optimised to return a uint8_t with the value of the quadrant variable.
								 //Also, the backwards version has been merged with the forward version through the use of
								 //an indentification variable "Dir"
void findLineS23(uint8_t quadrant);
void findLineS01(uint8_t quadrant);  // Based on the quadrant the buggy rotates left or right to get back to the motion
									 //line with sensors 1 and 2
void passedNote();  //Identifies whether sensors 0 and 1 have passed the node in the forward motion
void reachedNode(Direction Dir); //Optimised; Merged forward & backward versions.
void smartAlignment(Direction Dir);  //Smart Alignment Function for Forward/Backward movement.
									 //Optimised by merging forward & backward versions.
//Rotation strategy
//1) Get back to the line with s0 and s1
//2) Follow the line by going forwards  to align the entire buggy with it
//3) steadily go backwards, avoid drift away from the line to find the intersection
void getBackToLineRotation(); // Brings the buggy back to line at the rotation movement --Horizontal alignment
void forwardAlignmentOnRotation(); //Aligns the middle sensors s2 and s3 with the line
void backwardsToIntersection(); //Kick back until S2 and S3 are behind the node
void forwardsToIntersection(); //Kick forward until s2 and s3 have passed the intersection
void smartAlignmentRotation();  //Function to align the buggy upon rotation.
void sensorEventsB(); //Function to flag sensor events during backwards alignment.
void passedNoteB(); //Same as previous, but backwards
//void SmartAlignmentB(); //Same as previous, but backwards
//bool LineCorrect();	//Bool function to indicate if the destination intersection has reached. True if the middle sensors are in the intersection or if perfect intersection achieved
//Detects if you are drifting while kicking and corrects it. Returns false
bool reachedDestination();//Function to define whether or not the destination node has been reached.
//If so it returns true and prints "Destination Reached" to the terminal,
//if not returns false.
//Based on the quadrant the buggy rotates in a backward motion to get back
//to the motion line with sensors 4 and 5
uint8_t Counter = 0;
byte LeftSpeed = 70;
byte RightSpeed = 70;
bool LastCorrectLeft = false;
bool LastCorrectRight = false;
bool sensor0_event = false;
bool sensor1_event = false;
bool sensor4_event = false;
bool sensor5_event = false;
bool flag0 = false;
bool flag1 = false;
bool flag4 = false;
bool flag5 = false;
//byte quadrant = 0;
using buggyMotion::KickDirection;
enum Direction : uint8_t {
Forward, Backward
};
//*Quadrants Description
//* Forward  S0.S2 are in Q4, S1 and S3 are inQ1              //Backwards
//* S4 Q3 and S5 Q2                            S4 at Q4, S5 at Q1, S3S1 at Q2, S2S0 at Q3
//* Destination Note                            Destination Note
//Q2|Q1                                            Q1|Q2
//----|-----     Q0 is the on line case              --|--
//Q3|Q4                                            Q4 Q3
//


*/

/* UNUSED VARIABLES
*
*	bool MoveComplete was declared as a global variable but never set or referenced
*	bool MoveComplete = false;
*
*	uint16_t full was declared as a global variable but never set or referenced
*	uint16_t full = 0;
*
*	int IntersectionCount was declared as a global variable but never set or referenced
*	int IntersectionCount = 0;
*
*
*
*
*UNUSED ENUMS
*
*OutputMode, used to declare an object OMode in global scope that is never referenced
* typedef enum {
*	RAW, BW
*} OutputMode;
*
*
* State, used to declare an object Current in global scope that is never referenced
*typedef enum {
*Initial, Transistion, Destination
*} State;
*
*
*
* UNUSED FUNCTIONS
*
*function  declared in global scope but not defined or ever called:
*void DriftCorrect(DriftDirection Dir);
*
*/

/* UNUSED OBJECTS
*
*Object declared in global scope but never referenced
* OutputMode OMode = BW;
*
*Object declared in global scope but never referenced
*State Current = Initial;
*
*
*/
