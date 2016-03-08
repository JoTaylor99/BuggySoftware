#ifndef _NAVIGATION_H
#define _NAVIGATION_H

#include "Config.h"
#include "buggyMotion.h"
#include "Sensor.h"
#include "box.h"

class navigation : private buggyMotion {

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

	Sensor Sensors[7] = { SENSOR1, SENSOR2, SENSOR3, SENSOR4, SENSOR5, SENSOR6, { (unsigned int)7, sensorConfig::QTR } };

private:
	using buggyMotion::KickDirection;

	static bool _navInitComplete;

	int Counter = 0;

	byte LeftSpeed = 70;
	byte RightSpeed = 70;

	bool LastCorrectLeft = false;
	bool LastCorrectRight = false;

	bool starting_intersection[6] = { false, false, false, false, false, false };

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

	byte quadrant = 0;

	void BoxApproach();
	bool LineCorrect();
	bool reachedDestination();
	void start();
	void sensorEvents();
	void didIPassIntersectionLine();
	void whereAmI();
	void findLineS23();
	void findLineS01();
	void passedNote();
	void reachNote();
	void smartAlignmentForward();
	void getBackToLineRotation();
	void forwardAlignmentOnRotation();
	void backwardsToIntersection();
	void forwardsToIntersection();
	void smartAlignmentRotation();
	void sensorEventsB();
	void didIPassIntersectionLineB();
	void whereAmIB();
	void reachedNoteB();
	void passedNoteB();
	void SmartAlignmentB();
	void TurnLeft();
	void TurnRight();
	void MoveForward();
	void MoveBackward();



};

#endif

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







